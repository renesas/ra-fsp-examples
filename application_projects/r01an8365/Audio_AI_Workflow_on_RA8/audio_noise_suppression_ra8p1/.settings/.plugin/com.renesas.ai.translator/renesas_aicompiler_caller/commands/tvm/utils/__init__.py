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
from .file_utils import rel2abspath
from .shape_utils import convert_shape_dict2list_of_tuple, \
                         get_onnx_shape_dict, \
                         load_shape_dict_json, \
                         get_onnx_input_name, \
                         _validate_shape_dict_w_model_shape_dict, \
                         _validate_channel_num, \
                         _validate_datareader_inputs, \
                         _validate_onnx_shape
from .code_gen import SampleAppCppWriter, \
                      reorder_pre_settings, \
                      get_input_order_from_depjson, \
                      get_code_gen_mode, \
                      CodeGenMode, \
                      V2LCopyFilesFactory, \
                      V2HCopyFilesFactory, \
                      CopyFilesDirector
from .model_conv import BaseModelConverter, \
                            run_frontend_compiler, \
                            register_model_converter, \
                            create_model_converter
from .converter import *  # noqa