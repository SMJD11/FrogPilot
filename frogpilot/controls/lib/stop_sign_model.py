#!/usr/bin/env python3
"""
YOLOv11n-based stop sign detection model wrapper for comma 3x.
Loads an ONNX model and runs inference on road camera frames.
"""
import numpy as np
from pathlib import Path

from openpilot.common.swaglog import cloudlog

STOP_SIGN_MODEL_PATH = Path("/data/models/stop_sign_yolo26n.onnx")
STOP_SIGN_MODEL_URL = "https://raw.githubusercontent.com/SMJD11/FrogPilot-Resources/main/models/stop_sign_yolo26n.onnx"

# COCO class ID for stop sign
STOP_SIGN_CLASS_ID = 11

# Real-world stop sign dimensions (US standard: 75cm height)
STOP_SIGN_REAL_HEIGHT_M = 0.75

# Comma 3x road camera approximate focal length in pixels
# (for 1164x874 resolution, f ≈ 950px)
CAMERA_FOCAL_LENGTH_PX = 950.0
CAMERA_IMG_HEIGHT = 874
CAMERA_IMG_WIDTH = 1164

# Model input size
MODEL_INPUT_SIZE = 320

# Detection thresholds
CONFIDENCE_THRESHOLD = 0.5
CONFIDENCE_THRESHOLD_NIGHT = 0.35


class StopSignModel:
  def __init__(self):
    self.model = None
    self.available = False
    self._load_model()

  def _load_model(self):
    """Load the ONNX model. Model must be pre-downloaded via the UI."""
    if not STOP_SIGN_MODEL_PATH.exists():
      cloudlog.warning(f"Stop sign model not found at {STOP_SIGN_MODEL_PATH}. Download it from Settings.")
      return

    try:
      import onnxruntime as ort

      # Use CPU provider to avoid contention with the driving model on GPU
      providers = ['CPUExecutionProvider']
      sess_options = ort.SessionOptions()
      sess_options.inter_op_num_threads = 1
      sess_options.intra_op_num_threads = 2
      sess_options.graph_optimization_level = ort.GraphOptimizationLevel.ORT_ENABLE_ALL

      self.model = ort.InferenceSession(
        str(STOP_SIGN_MODEL_PATH),
        sess_options=sess_options,
        providers=providers
      )
      self.available = True
      cloudlog.info("Stop sign detection model loaded successfully")
    except Exception as e:
      cloudlog.error(f"Failed to load stop sign model: {e}")
      self.model = None
      self.available = False

  def preprocess(self, frame: np.ndarray) -> np.ndarray:
    """
    Preprocess a camera frame for YOLO26n inference.
    Args:
      frame: Raw camera frame (H, W, 3) in BGR uint8
    Returns:
      Preprocessed tensor (1, 3, 320, 320) float32
    """
    import cv2

    # Resize to model input size with letterboxing
    h, w = frame.shape[:2]
    scale = min(MODEL_INPUT_SIZE / h, MODEL_INPUT_SIZE / w)
    new_w, new_h = int(w * scale), int(h * scale)

    resized = cv2.resize(frame, (new_w, new_h), interpolation=cv2.INTER_LINEAR)

    # Create padded image
    padded = np.full((MODEL_INPUT_SIZE, MODEL_INPUT_SIZE, 3), 114, dtype=np.uint8)
    pad_h = (MODEL_INPUT_SIZE - new_h) // 2
    pad_w = (MODEL_INPUT_SIZE - new_w) // 2
    padded[pad_h:pad_h + new_h, pad_w:pad_w + new_w] = resized

    # BGR to RGB, HWC to CHW, normalize to [0, 1]
    blob = padded[:, :, ::-1].transpose(2, 0, 1).astype(np.float32) / 255.0
    return np.expand_dims(blob, axis=0), scale, pad_w, pad_h

  def detect(self, frame: np.ndarray, night_mode: bool = False) -> dict:
    """
    Run stop sign detection on a camera frame.
    Args:
      frame: Raw camera frame (H, W, 3) BGR uint8
      night_mode: If True, use lower confidence threshold
    Returns:
      dict with keys: detected (bool), confidence (float),
                      distance (float, meters), bounding_box (list[float] normalized)
    """
    result = {
      'detected': False,
      'confidence': 0.0,
      'distance': 0.0,
      'bounding_box': [0.0, 0.0, 0.0, 0.0],
    }

    if not self.available or self.model is None:
      return result

    try:
      preprocessed, scale, pad_w, pad_h = self.preprocess(frame)

      # Run inference
      input_name = self.model.get_inputs()[0].name
      outputs = self.model.run(None, {input_name: preprocessed})

      # Parse YOLO26n output
      # Output shape: (1, num_detections, 6) where each detection is [x1, y1, x2, y2, confidence, class_id]
      # Or for some YOLO exports: (1, 6, num_detections) — we handle both
      detections = outputs[0]

      if detections.ndim == 3 and detections.shape[1] == 6 and detections.shape[2] > 6:
        # Transposed format: (1, 6, N) -> (1, N, 6)
        detections = detections.transpose(0, 2, 1)

      if detections.ndim == 3:
        detections = detections[0]  # Remove batch dim -> (N, 6+)

      if len(detections) == 0:
        return result

      threshold = CONFIDENCE_THRESHOLD_NIGHT if night_mode else CONFIDENCE_THRESHOLD

      # Filter for stop signs (class 11) above confidence threshold
      best_conf = 0.0
      best_box = None

      for det in detections:
        if len(det) < 6:
          continue

        # Handle different output formats
        if len(det) == 6:
          x1, y1, x2, y2, conf, cls_id = det
        else:
          # Some models output class scores separately
          x1, y1, x2, y2 = det[:4]
          scores = det[4:]
          cls_id = np.argmax(scores)
          conf = scores[int(cls_id)]

        if int(cls_id) == STOP_SIGN_CLASS_ID and conf > threshold and conf > best_conf:
          best_conf = float(conf)
          best_box = [float(x1), float(y1), float(x2), float(y2)]

      if best_box is None:
        return result

      # Convert box from padded/scaled coords back to original image coords
      x1 = (best_box[0] - pad_w) / scale
      y1 = (best_box[1] - pad_h) / scale
      x2 = (best_box[2] - pad_w) / scale
      y2 = (best_box[3] - pad_h) / scale

      # Clamp to image bounds
      x1 = float(max(0, min(x1, CAMERA_IMG_WIDTH)))
      y1 = float(max(0, min(y1, CAMERA_IMG_HEIGHT)))
      x2 = float(max(0, min(x2, CAMERA_IMG_WIDTH)))
      y2 = float(max(0, min(y2, CAMERA_IMG_HEIGHT)))

      bbox_height_px = y2 - y1
      if bbox_height_px < 3:
        return result

      # Estimate distance using pinhole camera model
      # distance = (real_height * focal_length) / pixel_height
      distance = (STOP_SIGN_REAL_HEIGHT_M * CAMERA_FOCAL_LENGTH_PX) / bbox_height_px

      # Normalize bounding box to [0, 1]
      norm_box = [
        x1 / CAMERA_IMG_WIDTH,
        y1 / CAMERA_IMG_HEIGHT,
        x2 / CAMERA_IMG_WIDTH,
        y2 / CAMERA_IMG_HEIGHT,
      ]

      result['detected'] = True
      result['confidence'] = best_conf
      result['distance'] = float(distance)
      result['bounding_box'] = norm_box

    except Exception as e:
      cloudlog.error(f"Stop sign detection error: {e}")

    return result
