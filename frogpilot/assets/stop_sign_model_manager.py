#!/usr/bin/env python3
"""
Stop sign model download manager for FrogPilot.
Handles manual download of the YOLO26n ONNX model, triggered from the UI.
Follows the same pattern as model_manager.py for consistency.
"""
import requests
import tempfile

from pathlib import Path

from openpilot.frogpilot.assets.download_functions import download_file, verify_download
from openpilot.frogpilot.common.frogpilot_utilities import delete_file
from openpilot.frogpilot.common.frogpilot_variables import MODELS_PATH, params, params_memory

STOP_SIGN_MODEL_NAME = "stop_sign_yolo26n.onnx"
STOP_SIGN_MODEL_PATH = MODELS_PATH / STOP_SIGN_MODEL_NAME
STOP_SIGN_MODEL_URL = "https://raw.githubusercontent.com/SMJD11/FrogPilot-Resources/main/models/stop_sign_yolo26n.onnx"

CANCEL_DOWNLOAD_PARAM = "CancelModelDownload"
DOWNLOAD_PROGRESS_PARAM = "StopSignModelDownloadProgress"
DOWNLOAD_PARAM = "DownloadStopSignModel"


class StopSignModelManager:
  @staticmethod
  def download():
    """Download the stop sign detection model. Triggered by UI param."""
    session = requests.Session()
    session.headers.update({"User-Agent": "frogpilot-stop-sign/1.0"})

    print(f"Downloading stop sign detection model from {STOP_SIGN_MODEL_URL}")
    params_memory.put(DOWNLOAD_PROGRESS_PARAM, "Downloading stop sign model...")

    MODELS_PATH.mkdir(parents=True, exist_ok=True)

    download_file(
      CANCEL_DOWNLOAD_PARAM,
      STOP_SIGN_MODEL_PATH,
      DOWNLOAD_PROGRESS_PARAM,
      STOP_SIGN_MODEL_URL,
      DOWNLOAD_PARAM,
      session
    )

    if params_memory.get_bool(CANCEL_DOWNLOAD_PARAM):
      delete_file(STOP_SIGN_MODEL_PATH)
      params_memory.put(DOWNLOAD_PROGRESS_PARAM, "Download cancelled...")
      params_memory.remove(DOWNLOAD_PARAM)
      return

    if verify_download(STOP_SIGN_MODEL_PATH, STOP_SIGN_MODEL_URL, session):
      print("Stop sign model downloaded and verified successfully!")
      params_memory.put(DOWNLOAD_PROGRESS_PARAM, "Downloaded!")
      params_memory.remove(DOWNLOAD_PARAM)
      params.put_bool("StopSignModelDownloaded", True)
    else:
      print("Stop sign model verification failed!")
      delete_file(STOP_SIGN_MODEL_PATH)
      params_memory.put(DOWNLOAD_PROGRESS_PARAM, "Verification failed...")
      params_memory.remove(DOWNLOAD_PARAM)

  @staticmethod
  def is_downloaded():
    """Check if the model is already downloaded."""
    return STOP_SIGN_MODEL_PATH.is_file()
