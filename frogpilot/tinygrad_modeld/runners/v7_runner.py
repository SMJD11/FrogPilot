
from openpilot.system.hardware import TICI

USBGPU = "USBGPU" in __import__('os').environ

import pickle
import sys
import numpy as np
from pathlib import Path
from msgq.visionipc import VisionBuf
from openpilot.frogpilot.tinygrad_modeld.parse_model_outputs import Parser
from openpilot.frogpilot.tinygrad_modeld.models.commonmodel_pyx import DrivingModelFrame, CLContext

from openpilot.frogpilot.common.frogpilot_variables import METADATAS_PATH, MODELS_PATH

SEND_RAW_PRED = __import__('os').getenv('SEND_RAW_PRED')

# Path to April12th's tinygrad, used exclusively for V7 model loading.
# The NTS model.pkl was pickled with this version and is incompatible with
# FrogPilot's newer tinygrad (different module structure: tinygrad.ops vs tinygrad.uop.ops).
TINYGRAD_V7_PATH = str(Path(__file__).parent.parent / "tinygrad_v7")


def _swap_to_v7_tinygrad():
  """Replace FrogPilot's tinygrad with April12th's version in this process.

  The V7 process only runs V7 models, so the swap is permanent.
  This must be called BEFORE any tinygrad objects are created for V7.
  """
  # Remove all cached FrogPilot tinygrad modules
  to_remove = [k for k in sys.modules if k == 'tinygrad' or k.startswith('tinygrad.')]
  for k in to_remove:
    del sys.modules[k]

  # Prepend April12th's tinygrad so it takes priority
  sys.path.insert(0, TINYGRAD_V7_PATH)


class ModelStateV7:
  """V7-compatible model state for single-network models (e.g. Not Too Shabby).

  V7 models use a single .pkl network (not split vision/policy) and use
  the older supercombo_metadata_v7.pkl for input/output shape definitions.
  Buffer sizes are loaded directly from metadata to avoid shape mismatches
  with FrogPilot's constants (which are tuned for the split architecture).
  """
  frames: dict[str, DrivingModelFrame]
  inputs: dict[str, np.ndarray]
  output: np.ndarray
  prev_desire: np.ndarray  # for tracking the rising edge of the pulse

  def __init__(self, context: CLContext, model: str, model_version: str):
    # V7 models process every frame (no temporal skipping), so temporal_skip=1
    self.frames = {'input_imgs': DrivingModelFrame(context, 1), 'big_input_imgs': DrivingModelFrame(context, 1)}
    self.prev_desire = np.zeros(8, dtype=np.float32)  # DESIRE_LEN = 8

    # Load metadata — shapes come from here, NOT from ModelConstants
    metadata_path = METADATAS_PATH / f'supercombo_metadata_{model_version}.pkl'
    if not metadata_path.exists():
      metadata_path = MODELS_PATH / f'supercombo_metadata_{model_version}.pkl'

    with open(metadata_path, 'rb') as f:
      model_metadata = pickle.load(f)
    self.input_shapes = model_metadata['input_shapes']

    self.output_slices = model_metadata['output_slices']
    net_output_size = model_metadata['output_shapes']['outputs'][1]
    self.output = np.zeros(net_output_size, dtype=np.float32)
    self.parser = Parser()

    # Initialize numpy input buffers from metadata shapes (not ModelConstants)
    # This avoids the FULL_HISTORY_BUFFER_LEN mismatch (100 in FrogPilot vs 99 in V7)
    self.numpy_inputs = {}
    for k, shape in self.input_shapes.items():
      if k not in ('input_imgs', 'big_input_imgs'):
        self.numpy_inputs[k] = np.zeros(shape, dtype=np.float32)

    # Expose interface attributes needed by tinygrad_modeld.py main loop
    self.vision_input_names = ['input_imgs', 'big_input_imgs']
    self.desire_type = 'desire'
    self.use_lateral_control_params = 'lateral_control_params' in self.input_shapes

    model_path = MODELS_PATH / f'{model}.pkl'

    if TICI:
      # Swap to April12th's tinygrad for V7 model compatibility.
      # The NTS pickle was created with April12th's tinygrad and can only
      # be loaded with that exact version. This process only runs V7.
      _swap_to_v7_tinygrad()

      from tinygrad.tensor import Tensor
      from tinygrad.dtype import dtypes
      from tinygrad.helpers import to_mv
      self.Tensor = Tensor
      self.dtypes = dtypes
      self.to_mv = to_mv

      self.tensor_inputs = {k: Tensor(v, device='NPY').realize() for k, v in self.numpy_inputs.items()}
      with open(model_path, "rb") as f:
        self.model_run = pickle.load(f)

  def slice_outputs(self, model_outputs: np.ndarray) -> dict[str, np.ndarray]:
    parsed_model_outputs = {k: model_outputs[np.newaxis, v] for k, v in self.output_slices.items()}
    if SEND_RAW_PRED:
      parsed_model_outputs['raw_pred'] = model_outputs.copy()
    return parsed_model_outputs

  def run(self, bufs: dict[str, VisionBuf], transforms: dict[str, np.ndarray],
                inputs: dict[str, np.ndarray], prepare_only: bool) -> dict[str, np.ndarray] | None:
    # Model decides when action is completed, so desire input is just a pulse triggered on rising edge
    inputs['desire'][0] = 0
    new_desire = np.where(inputs['desire'] - self.prev_desire > .99, inputs['desire'], 0)
    self.prev_desire[:] = inputs['desire']

    self.numpy_inputs['desire'][0, :-1] = self.numpy_inputs['desire'][0, 1:]
    self.numpy_inputs['desire'][0, -1] = new_desire

    self.numpy_inputs['traffic_convention'][:] = inputs['traffic_convention']
    if self.use_lateral_control_params:
      self.numpy_inputs['lateral_control_params'][:] = inputs['lateral_control_params']
    imgs_cl = {'input_imgs': self.frames['input_imgs'].prepare(bufs['input_imgs'], transforms['input_imgs'].flatten()),
               'big_input_imgs': self.frames['big_input_imgs'].prepare(bufs['big_input_imgs'], transforms['big_input_imgs'].flatten())}

    if TICI and not USBGPU:
      # The imgs tensors are backed by opencl memory, only need init once
      for key in imgs_cl:
        if key not in self.tensor_inputs:
          # inline qcom_tensor_from_opencl_address logic using V7 to_mv and Tensor
          cl_buf_desc_ptr = self.to_mv(imgs_cl[key].mem_address, 8).cast('Q')[0]
          rawbuf_ptr = self.to_mv(cl_buf_desc_ptr, 0x100).cast('Q')[20]
          self.tensor_inputs[key] = self.Tensor.from_blob(rawbuf_ptr, self.input_shapes[key], dtype=self.dtypes.uint8, device='QCOM')
    elif USBGPU:
      for key in imgs_cl:
        frame_input = self.frames[key].buffer_from_cl(imgs_cl[key]).reshape(self.input_shapes[key])
        self.tensor_inputs[key] = self.Tensor(frame_input, dtype=self.dtypes.uint8).realize()

    if prepare_only:
      return None

    if TICI or USBGPU:
      self.output = self.model_run(**self.tensor_inputs).numpy().flatten()

    outputs = self.parser.parse_outputs_v7(self.slice_outputs(self.output))

    self.numpy_inputs['features_buffer'][0, :-1] = self.numpy_inputs['features_buffer'][0, 1:]
    self.numpy_inputs['features_buffer'][0, -1] = outputs['hidden_state'][0, :]

    # TODO model only uses last value now
    self.numpy_inputs['prev_desired_curv'][0, :-1] = self.numpy_inputs['prev_desired_curv'][0, 1:]
    self.numpy_inputs['prev_desired_curv'][0, -1, :] = outputs['desired_curvature'][0, :]
    return outputs
