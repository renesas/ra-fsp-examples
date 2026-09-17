#######################################################################################################################
# DISCLAIMER
# This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
# other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
# applicable laws, including copyright laws.
# THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
# THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
# EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
# SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
# THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
# Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
# this software. By using this software, you agree to the additional terms and conditions found by accessing the
# following link:
# http://www.renesas.com/disclaimer
#
# Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
#######################################################################################################################
import os
import sys
import argparse
import json
from pathlib import Path

import renesas_aicompiler_caller # noqa
from renesas_aicompiler_caller.utils import LogFactory, VariableContainer

logger = LogFactory().add_console_handler() \
                     .build(__name__)
vars = VariableContainer()

BATCH_SIZE = 1


def parse_args(args):
    """Parse command line arguments

    Args:
        None

    Returns:
        args: Arguments from command line.
    """
    parser = argparse.ArgumentParser(description='Get input node name from model')
    parser.add_argument('--framework',
                        required=True,
                        type=str,
                        choices=["onnx", "pytorch", "tflite", "executorch"],
                        help='Framework of input model file. This option can be set onnx, pytorch or tflite.')
    parser.add_argument('--input_model',
                        required=True,
                        type=lambda x: _validate_extension(parser,
                                                           (".onnx",
                                                            ".pt",
                                                            ".tflite",
                                                            ".pte"), x),
                        help='Path to input model file.')
    parser.add_argument('--output_shape_dict_jsonpath',
                        required=True,
                        type=str,
                        help='Path to output `shape directory` json')
    parser.add_argument('--output_model_info_jsonpath',
                        required=True,
                        type=str,
                        help='Path to output `model information` json')
    parser.add_argument('--accelerator_type',
                        default="drpai",
                        choices=["ethos-u55", "drpai"],
                        help='Model to be deployed')

    parser.add_argument('--input_shape_string',
                        type=str,
                        help='The input shape of model. '
                             'This option should be list of tuple, like [(B, C, H, W), (B, C, H, W), ...]. '
                             'The following is the setting example.\n'
                             '--input_shape_string "[(1,3,256,256),(1,3,416,416)]"')
    args = parser.parse_args(args)
    return args


def _validate_args(args):
    # Check framework compatible with input model
    ext = Path(args.input_model).suffix
    if args.framework == "pytorch" and ext != ".pt":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of pytorch, `input_model` extension must be '.pt'.")
    elif args.framework == "onnx" and ext != ".onnx":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of onnx, `input_model` extension must be '.onnx'.")
    elif args.framework == "tflite" and ext != ".tflite":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of tflite, `input_model` extension must be '.tflite'.")
    elif args.framework == "executorch" and ext != ".pte":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of execuTorch, `input_model` extension must be '.pte'.")

    if args.framework == "pytorch" and args.input_shape_string is None:
        raise ValueError("If `framework` specify as `pytorch`, `input_shape_string` must be set.")

    if args.framework in ["onnx", "tflite", "executorch"] and args.input_shape_string is not None:
        raise ValueError(f"If `framework` specify as `{args.framework}`, `input_shape_string` must not be set.")


def _validate_extension(parser, ext_choices, filename):
    ext = Path(filename).suffix
    if ext not in ext_choices:
        parser.error(f"Unsupported file is input:{filename}. "
                     f"This tool supports the file that has the extension {ext_choices}.")
    return filename


def flatten(nested_array):
    """Flattens a nested array."""
    for item in nested_array:
        if isinstance(item, (list, tuple)):
            yield from flatten(item)
        else:
            yield item


def _validate_input_shape(input_shape):
    if not isinstance(input_shape, list):
        raise TypeError(
            "`input_shape` is invalid. "
            "`input_shape` should be a list of tuple, like [(B, C, H, W)]."
        )
    if not isinstance(input_shape[0], tuple):
        raise TypeError(
            "`input_shape` is invalid. "
            "`input_shape` should be a list of tuple, like [(B, C, H, W)]."
        )
    if any(size <= 0 for size in flatten(input_shape)):
        raise ValueError("`input_shape` is invalid. Negative size found in `input_shape`.")


def _decode_type(n):
    """Porting from tvm

    Reference:
        https://github.com/apache/tvm/blob/046910a4100c5a822133ade5dfe851a1eb0ad95a/python/tvm/relay/frontend/tflite.py#L3930C1-L3943C24
    """
    _tflite_m = {
        0: "float32",
        1: "float16",
        2: "int32",
        3: "uint8",
        4: "int64",
        5: "string",
        6: "bool",
        7: "int16",
        8: "complex64",
        9: "int8",
    }
    return _tflite_m[n]


def tflite_input_type(model):
    """Porting from tvm

    Reference:
        https://github.com/apache/tvm/blob/046910a4100c5a822133ade5dfe851a1eb0ad95a/python/tvm/relay/frontend/tflite.py#L3930C1-L3943C24
    """
    subgraph_count = model.SubgraphsLength()
    assert subgraph_count > 0
    shape_dict = {}
    dtype_dict = {}
    for subgraph_index in range(subgraph_count):
        subgraph = model.Subgraphs(subgraph_index)
        inputs_count = subgraph.InputsLength()
        assert inputs_count >= 1
        for input_index in range(inputs_count):
            input_ = subgraph.Inputs(input_index)
            assert subgraph.TensorsLength() > input_
            tensor = subgraph.Tensors(input_)
            input_shape = tuple(tensor.ShapeAsNumpy())
            tensor_type = tensor.Type()
            input_name = tensor.Name().decode("utf8")
            shape_dict[input_name] = input_shape
            dtype_dict[input_name] = _decode_type(tensor_type)

    return shape_dict, dtype_dict


def is_onnx_quantized(model_path: str) -> bool:
    """
    Determines if an ONNX model is quantized by checking:
    - Quantized ops (QuantizeLinear, DequantizeLinear, QLinearConv, QLinearMatMul, DynamicQuantizeLinear),
    or
    - INT8 or UINT8 initializers.
    """
    import onnx
    model = onnx.load(model_path)
    graph = model.graph

    # Updated the quantization related operator
    quant_ops = {"QuantizeLinear", "DequantizeLinear", "QLinearConv", "QLinearMatMul", "DynamicQuantizeLinear"}
    for node in graph.node:
        if node.op_type in quant_ops:
            return True

    # Check if any initializer contains int8/uint8
    for initializer in graph.initializer:
        if initializer.data_type in (onnx.TensorProto.INT8, onnx.TensorProto.UINT8):
            return True


def is_tflite_quantized(model_path: str) -> bool:
    """
    Determines if a TFLite model is quantized by checking the datatype
    of input and output tensors.
    - If input/output tensors' datatype is int8/uint8, The tflite will be distincted as int8 tflite file.
    """
    import tensorflow as tf
    import numpy as np
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()

    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    logger.info("=== Input Details ===")
    for i, detail in enumerate(input_details):
        logger.info(f"Input tensor {i}:")
        logger.info(f"  name: {detail['name']}")
        logger.info(f"  index: {detail['index']}")
        logger.info(f"  shape: {detail['shape']}")
        logger.info(f"  dtype: {detail['dtype']}")
        # The following fields can be useful for quantized models:
        logger.info(f"  quantization (deprecated)_soon): {detail['quantization']}")
        logger.info(f"  quantization_parameters: {detail['quantization_parameters']}")
        logger.info(f"  sparsity_parameters: {detail['sparsity_parameters']}")

    logger.info("=== Output Details ===")
    for i, detail in enumerate(output_details):
        logger.info(f"Output tensor {i}:")
        logger.info(f"  name: {detail['name']}")
        logger.info(f"  index: {detail['index']}")
        logger.info(f"  shape: {detail['shape']}")
        logger.info(f"  dtype: {detail['dtype']}")
        # Same optional fields
        logger.info(f"  quantization (deprecated_soon): {detail['quantization']}")
        logger.info(f"  quantization_parameters: {detail['quantization_parameters']}")
        logger.info(f"  sparsity_parameters: {detail['sparsity_parameters']}")

    for detail in interpreter.get_tensor_details():
        if detail['dtype'] in (np.int8, np.uint8):
            return True

    return False


def is_pte_quantized(model_path: str) -> bool:
    """
    Check if a PTE model is quantized (INT8) or not.
    Args:
        model_path (str): Path to the PTE model file (.pte)
    Returns:
        bool: True if quantized, False if float model
    """
    from mera.mera_model import ModelLoader
    loader = ModelLoader()
    model = loader.from_executorch(model_path)
    read_input_des = model._read_input_desc()
    for inp in read_input_des.all_inputs.values():
        logger.info("Input %s: dtype=%s", inp.input_name, inp.input_type)
        if str(inp.input_type).lower() in ["int8", "uint8"]:
            return True
    return False


def is_toco_converted_tflite(model_path: str) -> bool:
    """
    Determines if a TFLite model was converted using TocoConverter by inspecting
    the description field in the model's flatbuffer.
    If the description equals "TOCO Converted.", the model is considered to be converted using TocoConverter.
    """
    import tflite
    with open(model_path, "rb") as f:
        buf = f.read()
    model = tflite.Model.GetRootAsModel(buf, 0)
    desc = model.Description().decode('utf-8') if model.Description() else ""
    return desc == "TOCO Converted."


def get_onnx_input_name(onnx_model):
    """Get onnx input node name

    Args:
        onnx_model: ModelProto
    """
    input_all = [node.name for node in onnx_model.graph.input]
    input_initializer = [node.name for node in onnx_model.graph.initializer]
    model_feed_input = list(set(input_all) - set(input_initializer))
    return model_feed_input


def get_onnx_shape_dict(onnx_model):
    """Get onnx input shape dictionary

    Args:
        onnx_model: ModelProto

    Returns:
        shape_dict: Dictionary that has input shape information of model.
    """
    input_names = get_onnx_input_name(onnx_model)

    # Create input shape dict
    shape_dict = {}
    for _input in onnx_model.graph.input:
        # Skip appending if _input node is not model input.
        if _input.name not in input_names:
            continue
        shape_dict[_input.name] = [d.dim_value for d in _input.type.tensor_type.shape.dim]

    logger.info(f"shape_dict: {shape_dict}")

    # Validate shape dict in terms of batch dimension
    for input_name, shape in shape_dict.items():
        # Check batch dimension
        if shape[0] != BATCH_SIZE:
            logger.warning(f"Dynamic shape is detected at batch dimension of `{input_name}` node. "
                           "Batch dimension is forced to change to 1.")
            shape_dict[input_name] = [1, *shape[1:]]

    logger.info(f"Validated shape_dict: {shape_dict}")

    return shape_dict


def conv_input_shape_string2shape_dict(input_shape_string, prefix='input'):
    """Convert input_shape_string to shape_dict

    Args:
        input_shape_string: The input shape of model as string.

    Returns:
        shape_dict: The input shape to the graph.
    """

    # Convert string to list of tuple
    try:
        input_shape = eval(input_shape_string)
    except Exception as e:
        raise ValueError("Could not convert `input_shape_string` to list of tuple. "
                         "Please check that the `input_shape_string` argument is correct."
                         f"Error: {e}")

    _validate_input_shape(input_shape)

    # Create shape_dict
    shape_dict = {}
    for i, shape in enumerate(input_shape):
        shape_dict[f"{prefix}{i}"] = shape

    logger.info(f"shape_dict: {shape_dict}")

    return shape_dict


def get_tflite_shape_dict(input_model):
    """Get tflite input shape as dictionary

    Args:
        input_model: Path to tflite model file.

    Returns:
        shape_dict: Dictionary that has input shape information of model.
    """
    import tflite

    # 2. Load tflite model
    tflite_model_buf = open(input_model, "rb").read()
    tflite_model = tflite.Model.GetRootAsModel(tflite_model_buf, 0)

    # 2.2 Set input data information
    tflite_shape_dict, _ = tflite_input_type(tflite_model)
    logger.info(f"tflite_shape_dict: {tflite_shape_dict}")

    # Convert int type from int32 to output json file.
    shape_dict = {}
    for key, val in tflite_shape_dict.items():
        shape_dict[key] = [int(v) for v in val]
    logger.info(f"shape_dict: {shape_dict}")

    return shape_dict


def get_pte_shape_dict(pte_model_path):
    """
    Get input shape dictionary from a .pte (ExecuTorch)
    Args:
        pte_model_path (str): path to the .pte file
    Returns:
        shape_dict: Dictionary that has input shape information of model.
    """
    from mera.mera_model import ModelLoader

    loader = ModelLoader()
    model = loader.from_executorch(pte_model_path)
    read_input_des = model._read_input_desc()
    shape_dict = {}
    for inp in read_input_des.all_inputs.values():
        shape_dict[inp.input_name] = inp.input_shape
    logger.info(f"shape_dict: {shape_dict}")
    return shape_dict


def make_shape_dict_from_model(framework,
                               input_model,
                               input_shape_string=None):
    """Make shape_dict from model

    Args:
        framework: Framework of input model file. This option can be set onnx, pytorch or tflite.
        input_model: Path to input model file.
        input_shape_string: The input shape of model as string.

    Returns:
        shape_dict: The input shape to the graph.
    """
    logger.info("Make shape_dict from model.")

    if "onnx" == framework:
        import onnx
        onnx_model = onnx.load_model(input_model)
        shape_dict = get_onnx_shape_dict(onnx_model)
        return shape_dict

    elif "pytorch" == framework:
        shape_dict = conv_input_shape_string2shape_dict(input_shape_string)
        return shape_dict

    elif "tflite" == framework:
        shape_dict = get_tflite_shape_dict(input_model)
        return shape_dict

    elif "executorch" == framework:
        shape_dict = get_pte_shape_dict(input_model)
        return shape_dict

    else:
        raise NotImplementedError("Unsupported framework. `framework` can be set to onnx, pytorch or tflite.")


def make_model_info_dict_from_model(framework,
                                    input_model,
                                    accelerator_type):
    """Make shape_dict from model

    Args:
        framework: Framework of input model file. This option can be set onnx, pytorch or tflite.
        input_model: Path to input model file.
        accelerator_type: Accelerator type to be utilized by model

    Returns:
        model_info_dict: The model information dictionary
    """
    if accelerator_type == "drpai":
        return {"is_quantized": None}

    if "onnx" == framework:
        if is_onnx_quantized(input_model):
            raise ValueError(
                "Mera cannot handle the quantized onnx model. "
                "Please input the float32 model which is before applying quantization."
            )
        return {"is_quantized": False}

    elif "tflite" == framework:
        # Firstly, check if the TFLite model was converted with TocoConverter.
        if is_toco_converted_tflite(input_model):
            raise ValueError(
                "Mera cannot handle tflite models converted using TocoConverter, "
                "please convert the model using the tf.lite.TFLiteConverter and then try again. "
            )

        model_status = is_tflite_quantized(input_model)
        return {"is_quantized": model_status}

    elif "executorch" == framework:
        if is_pte_quantized(input_model):
            raise ValueError(
                "Mera cannot handle the quantized executorch model. "
                "Please input the float32 model which is before applying quantization."
            )
        return {"is_quantized": False}

    else:
        raise NotImplementedError("Unsupported framework. `framework` can be set to "
                                  f"onnx or tflite for {accelerator_type}.")


def write_dict2json(dict_data, o_json_file):
    """Write dict to json file format

    Args:
        dict_data: The dictionary to the graph.
        o_json_file: Path to output shape directory json

    Returns:
        None
    """
    logger.info("Write dictionary to json file.")

    # create the save directory
    output_dir = os.path.dirname(os.path.abspath(o_json_file))
    if not os.path.exists(output_dir):
        os.makedirs(output_dir, exist_ok=True)

    with open(o_json_file, "w") as outfile:
        json.dump(dict_data, outfile)


################################################################################
# Main function
################################################################################
def main(args):
    # Parse command line arguments
    args = parse_args(args)
    _validate_args(args)

    logger.info("[Parsing input model node start...]")
    # Make shape_dict from input_model or input_shape_string
    shape_dict = make_shape_dict_from_model(args.framework,
                                            args.input_model,
                                            args.input_shape_string)

    # Make model_info_dict from input_model
    model_info_dict = make_model_info_dict_from_model(args.framework,
                                                      args.input_model,
                                                      args.accelerator_type)

    # Write shape dict as json
    write_dict2json(shape_dict, args.output_shape_dict_jsonpath)
    write_dict2json(model_info_dict, args.output_model_info_jsonpath)
    logger.info("[Parsing input model node finished]")


if __name__ == "__main__":
    main(sys.argv[1:])
