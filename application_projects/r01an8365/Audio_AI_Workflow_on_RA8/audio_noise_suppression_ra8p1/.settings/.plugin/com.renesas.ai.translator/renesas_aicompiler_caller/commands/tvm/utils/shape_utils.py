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
# Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
#######################################################################################################################
import json
from renesas_aicompiler_caller.utils import LogFactory

__all__ = ['get_onnx_shape_dict',
           'convert_shape_dict2list_of_tuple',
           'load_shape_dict_json',
           'get_onnx_input_name',
           '_validate_shape_dict_w_model_shape_dict',
           '_validate_shape_dict',
           '_validate_datareader_inputs',
           '_validate_channel_num']

logger = LogFactory().add_console_handler() \
                     .build(__name__)

BATCH_SIZE = 1


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

    # Validate shape dict
    shape_dict = _validate_onnx_shape(shape_dict)
    logger.info(f"Validated shape_dict: {shape_dict}")

    return shape_dict


def get_onnx_input_name(onnx_model):
    """Get onnx input node name

    Args:
        onnx_model: ModelProto
    """
    input_all = [node.name for node in onnx_model.graph.input]
    input_initializer = [node.name for node in onnx_model.graph.initializer]
    model_feed_input = list(set(input_all) - set(input_initializer))
    return model_feed_input


def load_shape_dict_json(shape_dict_json):
    """Load shape_dict json

    Args:
        shape_dict_json: Path to input shape configuration (*.json).

    Returns:
        shape_dict: Dictionary that input shape to the graph
    """
    # Load preprocess_config_json as dict format
    with open(shape_dict_json) as json_file:
        shape_dict = json.load(json_file)

    # Validate loaded dict data
    _validate_shape_dict(shape_dict)

    return shape_dict


def convert_shape_dict2list_of_tuple(shape_dict):
    """Convert shape dictionary to list of tuple

    Args:
        shape_dict: The input shape to the graph

    Returns:
        shape_list: Graph level input shape list.

    Notes:
        The following is example.
        shape_dict:
            {
                "input0":[
                    1,2
                ],
                "input1":[
                    3,4
                ]
            }
        shape_list:
            [('input0', (1, 2)), ('input1', (3, 4))]
    """
    shape_list = []

    for node_name, shape in shape_dict.items():
        input_shape_list = (node_name, tuple(shape))
        shape_list.append(input_shape_list)

    return shape_list


def _validate_onnx_shape(shape_dict):

    for input_name, shape in shape_dict.items():
        # Check batch dimension
        if shape[0] != BATCH_SIZE:
            logger.warning(f"Dynamic shape is detected at batch dimension of `{input_name}` node. "
                           "Batch dimension is forced to change to 1.")
            shape_dict[input_name] = [1, *shape[1:]]

        # Check other dimention
        if any(size <= 0 for size in shape[1:]):
            logger.warning(f"Invalid shape is detected at `{input_name}` node. "
                           "Model shape must be greater than 0. "
                           "This warning may appear if dynamic shape are used in the model.")
    return shape_dict


def _validate_shape_dict_w_model_shape_dict(shape_dict, model_shape_dict):
    """Validate that the user's input shape_dict matches the shape_dict given by model

    Args:
        shape_dict: user's input shape_dict
        model_shape_dict: shape_dict given by model
    """
    for input_name, shape in shape_dict.items():
        if input_name not in model_shape_dict.keys():
            raise ValueError(f'`{input_name}` is not input nodes of model. ',
                             f'Input nodes of model are ({model_shape_dict.keys()}). '
                             f'Please change `{input_name}` to the following node name.\n'
                             f'    {model_shape_dict.keys()}')

        if shape != model_shape_dict[input_name]:
            logger.warning(f'The shape of `{input_name}` is mismatch with model. '
                           f'The specified shape is {shape}. '
                           f'But the model shape is {model_shape_dict[input_name]}.')


def _validate_shape_dict(shape_dict):

    for input_name, shape in shape_dict.items():
        # Check batch dimension
        if shape[0] != BATCH_SIZE:
            logger.warning(f"Invalid shape is detected at batch dimension of `{input_name}` node. "
                           "Batch dimension is forced to change to 1.")
            shape_dict[input_name] = [1, *shape[1:]]

        # Check other dimention
        if any(size <= 0 for size in shape[1:]):
            raise ValueError(f"Invalid shape is detected at `{input_name}` node. "
                             "Model shape must be greater than 0. ")
    return shape_dict


def _validate_datareader_inputs(inputs):

    if not isinstance(inputs, dict):
        raise RuntimeError("inputs is invalid. inputs must be dictionary type.")

    if "key" not in inputs.keys():
        raise RuntimeError("inputs is invalid. inputs must have key named `key`.")

    if "value" not in inputs.keys():
        raise RuntimeError("inputs is invalid. inputs must have key named `value`.")


def _validate_channel_num(list_to_be_valid, expect_channel_num, variable_name):
    if len(list_to_be_valid) != expect_channel_num:
        raise RuntimeError(f"Number of `{variable_name}` elements is invalid. "
                           f"Number of `{variable_name}` elements must be {expect_channel_num}. "
                           f"Check number of elements set for `{variable_name}` on the GUI. ")
