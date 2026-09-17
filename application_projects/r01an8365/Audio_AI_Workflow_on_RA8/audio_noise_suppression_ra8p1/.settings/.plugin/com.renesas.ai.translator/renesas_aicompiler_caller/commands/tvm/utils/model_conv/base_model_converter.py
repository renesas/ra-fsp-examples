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
from abc import ABC, abstractmethod
from typing import Dict, List

from renesas_aicompiler_caller.utils import LogFactory

__all__ = ['BaseModelConverter']

logger = LogFactory().add_console_handler() \
                     .build(__name__)


class BaseModelConverter(ABC):

    def __init__(self):
        logger.info(f"{self.__class__.__name__} is initialized...")

    @abstractmethod
    def execute_frontend(self,
                         framework: str,
                         input_model: str,
                         shape_dict: Dict[str, List[int]],
                         **kwargs):
        raise NotImplementedError("Must implement `execute_frontend()` method in subclass.")

    @abstractmethod
    def execute_backend(self):
        raise NotImplementedError("Must implement `execute_backend()` method in subclass.")
