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
from typing import List
from ._registry import code_generator_entrypoint
from .code_gen_utils import CodeGenMode

__all__ = ['create_code_generator']


def create_code_generator(
        code_generator_name: str,
        filename: str,
        drpai_tvm_model_obj_dir: str,
        preprocess_obj_dirs: List[str],
        preprocess_configs: List[str],
        code_gen_mode: CodeGenMode,
        **kwargs
):
    """Create a code generator

    Lookup code geneartor's entrypoint class and pass relevant args to create a code generator

    Args:
        code_generator_name: Name of a code generator to instantiate.
        filename: Path to output file.
        drpai_tvm_model_obj_dir: Path to DRP-AI TVM Model Object directory.
        preprocess_obj_dirs: List containing path to preprocess object directory
        code_gen_mode: Value for code gen mode to output sample code

    Returns:
        instance: Instance of a code generator to be specified.

    Reference:
        - https://github.com/huggingface/pytorch-image-models/blob/88889de923149533593482879a564e7f7e0dabbd/timm/models/_factory.py#L38  # noqa
    """
    code_generator_cls = code_generator_entrypoint(code_generator_name)
    instance = code_generator_cls(
                    filename=filename,
                    drpai_tvm_model_obj_dir=drpai_tvm_model_obj_dir,
                    preprocess_obj_dirs=preprocess_obj_dirs,
                    preprocess_configs=preprocess_configs,
                    code_gen_mode=code_gen_mode,
                    **kwargs
                )

    return instance
