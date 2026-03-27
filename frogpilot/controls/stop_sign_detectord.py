#!/usr/bin/env python3
"""
Stop sign detection daemon for FrogPilot.
Processes road camera frames and publishes stop sign detections via cereal.
Uses YOLO26n pre-trained on COCO (stop sign = class 11).
"""
import time
import numpy as np

import cereal.messaging as messaging
from cereal.messaging import PubMaster, SubMaster
from msgq.visionipc import VisionIpcClient, VisionStreamType
from openpilot.common.realtime import Ratekeeper, config_realtime_process, Priority
from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.swaglog import cloudlog
from openpilot.common.params import Params

from openpilot.frogpilot.controls.lib.stop_sign_model import StopSignModel

PROCESS_NAME = "frogpilot.controls.stop_sign_detectord"

# Target detection rate
DETECTION_HZ = 5
# Camera frame rate
CAMERA_HZ = 20
# Process every Nth frame
FRAME_DECIMATION = CAMERA_HZ // DETECTION_HZ  # = 4


def main():
  cloudlog.info("stop_sign_detectord starting")
  config_realtime_process(3, Priority.CTRL_LOW)

  # Load the YOLO26n model
  model = StopSignModel()
  if not model.available:
    cloudlog.warning("Stop sign model not available, daemon will publish empty detections")

  # Wait for car params
  params = Params()
  params.get("CarParams", block=True)

  # Setup messaging
  pm = PubMaster(["frogpilotStopSign"])
  sm = SubMaster(["deviceState", "frogpilotPlan"])

  # Setup visionipc client for road camera
  vipc_client = VisionIpcClient("camerad", VisionStreamType.VISION_STREAM_ROAD, True)
  while not vipc_client.connect(False):
    time.sleep(0.1)
  cloudlog.info(f"stop_sign_detectord connected to camerad: {vipc_client.width}x{vipc_client.height}")

  # Temporal smoothing filter for detection confidence
  confidence_filter = FirstOrderFilter(0.0, 0.5, 1.0 / DETECTION_HZ)
  distance_filter = FirstOrderFilter(0.0, 0.3, 1.0 / DETECTION_HZ)

  frame_count = 0
  last_detection = {
    'detected': False,
    'confidence': 0.0,
    'distance': 0.0,
    'bounding_box': [0.0, 0.0, 0.0, 0.0],
  }

  rk = Ratekeeper(CAMERA_HZ, print_delay_threshold=None)

  while True:
    buf = vipc_client.recv()
    if buf is None:
      continue

    frame_count += 1
    sm.update(0)

    # Only process every Nth frame
    if frame_count % FRAME_DECIMATION == 0 and model.available:
      # Convert visionipc buffer to numpy array
      frame = np.frombuffer(buf.data[:buf.uv_offset], dtype=np.uint8).reshape(
        (vipc_client.height, vipc_client.width, -1)
      )

      # Check if it's nighttime for adjusted thresholds
      night_mode = False
      if sm.valid['deviceState']:
        # Use light sensor or time-based heuristic
        # For now, we don't have direct access, so default to day mode
        night_mode = False

      # Run detection
      detection = model.detect(frame, night_mode=night_mode)

      # Temporal smoothing
      confidence_filter.update(detection['confidence'] if detection['detected'] else 0.0)
      if detection['detected'] and detection['distance'] > 0:
        distance_filter.update(detection['distance'])

      # Use smoothed values for stable output
      smoothed_conf = confidence_filter.x
      smoothed_detected = smoothed_conf > 0.3  # Lower threshold after smoothing

      last_detection = {
        'detected': bool(smoothed_detected),
        'confidence': float(smoothed_conf),
        'distance': float(distance_filter.x) if smoothed_detected else 0.0,
        'bounding_box': detection['bounding_box'] if smoothed_detected else [0.0, 0.0, 0.0, 0.0],
      }

    # Publish at camera rate (but detection only updates every Nth frame)
    if frame_count % FRAME_DECIMATION == 0:
      msg = messaging.new_message('frogpilotStopSign')
      stop_sign = msg.frogpilotStopSign
      stop_sign.detected = last_detection['detected']
      stop_sign.confidence = last_detection['confidence']
      stop_sign.distance = last_detection['distance']
      stop_sign.boundingBox = last_detection['bounding_box']
      pm.send('frogpilotStopSign', msg)

    rk.keep_time()


if __name__ == "__main__":
  main()
