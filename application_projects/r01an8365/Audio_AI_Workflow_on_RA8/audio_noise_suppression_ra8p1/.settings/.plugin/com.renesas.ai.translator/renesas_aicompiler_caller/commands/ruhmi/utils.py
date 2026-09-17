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
# Copyright (C) 2023-2024 Renesas Electronics Corporation. All rights reserved.
#######################################################################################################################
from renesas_aicompiler_caller.utils import LogFactory

logger = LogFactory().add_console_handler() \
                     .build(__name__)


def get_onnx_symbolic_shape_mapping(model_fp32, input_shape_dict):
    """Get symbolic shape mapping from onnx model

    Args:
        model_fp32: Path to the onnx fp32 model
        input_shape_dict: Dictoionary contains the input node name and its corresponding input shape.
        e.g.
        {
            "input1":[1,3,224,224],
            "input2":[1,3,416,416]
        }
        This data will be used to override symbolic default shape 1.

    Returns:
        symbolic_shape_mapping: Dictionary contains symbolic shape name and its corresponding number
    """
    import onnx
    onnx_model = onnx.load(model_fp32)

    symbolic_shape_mapping = {}
    for input_tensor in onnx_model.graph.input:
        if input_tensor.name not in input_shape_dict:
            continue

        logger.info(f"Check input name: {input_tensor.name} ...")
        shape = input_tensor.type.tensor_type.shape
        for dim, override_dim in zip(shape.dim, input_shape_dict[input_tensor.name]):
            if dim.dim_param:
                logger.info(f"Detects dynamic dimension name: {dim.dim_param}")
                symbolic_shape_mapping[dim.dim_param] = override_dim
    return symbolic_shape_mapping
