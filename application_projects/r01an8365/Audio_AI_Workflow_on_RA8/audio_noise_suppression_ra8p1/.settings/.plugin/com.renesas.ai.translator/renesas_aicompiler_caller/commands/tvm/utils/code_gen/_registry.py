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

__all__ = [
    "code_generator_entrypoint", "register_code_generator"
]

logger = LogFactory().add_console_handler() \
                     .build(__name__)

_code_generator_entrypoints: Dict[str, Callable] = {}  # mapping of code generator name to actual entrypoint class


def code_generator_entrypoint(code_generator_name: str) -> Callable:
    """Fetch a sample code generator entrypoint for specified name

    Reference:
        - https://github.com/huggingface/pytorch-image-models/blob/88889de923149533593482879a564e7f7e0dabbd/timm/models/_registry.py#L283 # noqa
    """
    if code_generator_name not in _code_generator_entrypoints:
        logger.error(f"Internal variable output: {_code_generator_entrypoints=}")
        raise RuntimeError(f"[Internal Error] Failed to find {code_generator_name=} in _code_generator_entrypoints. "
                           "Check if the Converter has been added with @register_model_converter.")

    return _code_generator_entrypoints[code_generator_name]


def register_code_generator(code_gen_cls: Callable) -> Callable:
    """Register a code generator

    Reference:
        - https://github.com/huggingface/pytorch-image-models/blob/88889de923149533593482879a564e7f7e0dabbd/timm/models/_registry.py#L75 # noqa
    """
    code_gen_cls_name = code_gen_cls.__name__

    pattern = r'^[A-Z0-9]+SampleAppCppWriterForV\d+$'
    if not re.match(pattern, code_gen_cls_name):
        raise ValueError(f"Failed to register a code generator ({code_gen_cls_name=}) "
                         "Class name must be `[Device name]SampleAppCppWriterFor[Version]` format.")

    # add entries to registry dict/sets
    if code_gen_cls_name in _code_generator_entrypoints:
        logger.warning(
            f'Overwriting {code_gen_cls_name} in registry with {code_gen_cls.__module__}.{code_gen_cls_name}. '
            'This is because the name being '
            'registered conflicts with an existing name. Please check if this is not expected.',
            stacklevel=2,
        )
    _code_generator_entrypoints[code_gen_cls_name] = code_gen_cls

    return code_gen_cls
