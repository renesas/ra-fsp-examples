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
import re
from typing import Dict, Callable

from renesas_aicompiler_caller.utils import LogFactory
from .base_model_converter import BaseModelConverter

__all__ = ['register_model_converter']

logger = LogFactory().add_console_handler() \
                     .build(__name__)

_model_converter_entrypoints: Dict[str, Callable] = {}  # mapping of model converter name to actual entrypoint class
_model_converter_to_module: Dict[str, str] = {}  # mapping of model converter names to module names


def model_converter_entrypoint(converter_name: str) -> Callable:
    """Fetch a model converter entrypoint for specified name
    """
    if converter_name not in _model_converter_entrypoints:
        logger.error(f"Internal variable output: {_model_converter_entrypoints=}")
        raise RuntimeError(f"[Internal Error] Failed to find {converter_name=} in _model_converter_entrypoints. "
                           "Check if the Converter has been added with @register_model_converter.")

    return _model_converter_entrypoints[converter_name]


def register_model_converter(model_converter_cls: Callable) -> Callable:
    cls_name = model_converter_cls.__name__
    module_name_split = model_converter_cls.__module__.split('.')
    module_name = module_name_split[-1] if len(module_name_split) else ''

    pattern = r'^V\d{3}RzV\d+[a-z]+ModelConverter$|^V\d{3}RzCpuModelConverter'
    if not re.match(pattern, cls_name):
        raise ValueError(f"Failed to register a code generator ({cls_name=}) "
                         "Class name must be `[DRP-AI TVM Version]Rz[Device name]ModelConverter` format "
                         "or [DRP-AI TVM Version]RzCpuModelConverter.")

    if not issubclass(model_converter_cls, BaseModelConverter):
        raise RuntimeError(f"{cls_name} must be subclass of `BaseModelConverter`. "
                           "Please check if the {cls_name} is a subclass of `BaseModelConverter`.")

    # add entries to registry dict/sets
    if cls_name in _model_converter_entrypoints:
        logger.warning(
            f'Overwriting {cls_name} in registry with {model_converter_cls.__module__}.{cls_name}. '
            'This is because the name being '
            'registered conflicts with an existing name. Please check if this is not expected.',
            stacklevel=2,
        )
    _model_converter_entrypoints[cls_name] = model_converter_cls
    _model_converter_to_module[cls_name] = module_name

    return model_converter_cls
