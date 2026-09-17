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
from abc import ABC, abstractmethod
from renesas_aicompiler_caller.utils import LogFactory
from ._factory import create_code_generator

__all__ = ["SampleAppCppWriter"]

logger = LogFactory().add_console_handler() \
                     .build(__name__)


class BaseSampleAppCppWriter(ABC):
    @abstractmethod
    def write_code(self):
        pass

    @abstractmethod
    def close(self):
        pass


class SampleAppCppWriter(BaseSampleAppCppWriter):
    def __init__(self,
                 product,
                 drpai_tvm_version,
                 filename,
                 drpai_tvm_model_obj_dir,
                 preprocess_obj_dirs,
                 code_gen_mode,
                 preprocess_configs):

        product_str = product.upper()  # e.g. v2l => V2L
        version_str = "".join(drpai_tvm_version.split('.')).upper()  # e.g. v1.1.1 => V111
        code_generator_name = "{}SampleAppCppWriterFor{}".format(product_str, version_str)
        try:
            self._writer = create_code_generator(code_generator_name=code_generator_name,
                                                 filename=filename,
                                                 drpai_tvm_model_obj_dir=drpai_tvm_model_obj_dir,
                                                 preprocess_obj_dirs=preprocess_obj_dirs,
                                                 preprocess_configs=preprocess_configs,
                                                 code_gen_mode=code_gen_mode)
        except Exception:
            raise NotImplementedError("[Internal error] Contact to Renesas with the following error message. \n"
                                      "Failed to initialize SampleAppCppWriter. "
                                      "Unsupported product and version are specified. \n"
                                      f"Product: {product}\n"
                                      f"Version: {drpai_tvm_version}")

    def write_code(self):
        self._writer.write_code()

    def close(self):
        self._writer.close()
