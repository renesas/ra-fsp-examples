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
import os
from abc import ABC, abstractmethod
import renesas_aicompiler_caller
from renesas_aicompiler_caller.utils import LogFactory, VariableContainer, SRC_CMAKE_FILES
from renesas_aicompiler_caller.commands.tvm.utils import rel2abspath

logger = LogFactory().add_console_handler() \
                     .build(__name__)
vars = VariableContainer()


class BaseCopyFilesFactory(ABC):
    @abstractmethod
    def create_copy_files_dict_for_exe(self):
        """Create copy files dictionary for exe directory

        Args:
            None

        Returns:
            None
        """
        raise NotImplementedError

    @abstractmethod
    def create_copy_files_dict_for_src(self):
        """Create copy files dictionary for src directory

        Args:
            None

        Returns:
            None
        """
        raise NotImplementedError


class CopyFilesDirector:
    def __init__(self,
                 builder: BaseCopyFilesFactory) -> None:
        self._builder = builder

    def construct(self):
        self._builder.create_copy_files_dict_for_exe()
        self._builder.create_copy_files_dict_for_src()


class V2LCopyFilesFactory(BaseCopyFilesFactory):
    def __init__(self,
                 output_dir,
                 drpai_tvm_model_obj_dir,
                 preprocess_obj_dirs):
        self._output_dir = output_dir
        self._drpai_tvm_model_obj_dir = rel2abspath(drpai_tvm_model_obj_dir)
        self._preprocess_obj_dirs = rel2abspath(preprocess_obj_dirs)
        self._copy_files_dict_for_exe = None
        self._copy_files_dict_for_src = None

    def create_copy_files_dict_for_exe(self):
        abs_drpai_tvm_model_obj_dir = self._drpai_tvm_model_obj_dir
        output_dst_dir = self._output_dir
        drpai_tvm_model_obj_dirname = os.path.basename(abs_drpai_tvm_model_obj_dir)
        copy_file_dict = {
            f"{abs_drpai_tvm_model_obj_dir}/deploy.json": f"{output_dst_dir}/{drpai_tvm_model_obj_dirname}",
            f"{abs_drpai_tvm_model_obj_dir}/deploy.params": f"{output_dst_dir}/{drpai_tvm_model_obj_dirname}",
            f"{abs_drpai_tvm_model_obj_dir}/deploy.so": f"{output_dst_dir}/{drpai_tvm_model_obj_dirname}",
        }

        if self._preprocess_obj_dirs is None:
            self._copy_files_dict_for_exe = copy_file_dict
            return

        for abs_preprocess_obj_dir in self._preprocess_obj_dirs:
            if abs_preprocess_obj_dir is None:
                continue
            dirname = os.path.basename(abs_preprocess_obj_dir)
            # Concat drpai_tvm_model obj directory name and preprocess obj directory name
            dst_dirname = os.path.join(drpai_tvm_model_obj_dirname, dirname)

            temp_dict = {
                f"{abs_preprocess_obj_dir}/aimac_desc.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_desc.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_param_info.txt": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_param.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/pp_addrmap_intm.txt": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/pp_drpcfg.mem": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/pp_weight.dat": f"{output_dst_dir}/{dst_dirname}",
            }
            copy_file_dict.update(temp_dict)

        self._copy_files_dict_for_exe = copy_file_dict

    def create_copy_files_dict_for_src(self):
        output_dst_dir = self._output_dir
        mod_path = os.path.dirname(os.path.abspath(renesas_aicompiler_caller.__file__))
        TVM_ROOT = os.getenv("TVM_ROOT", None)
        if TVM_ROOT is None:
            raise ValueError("No environment variable(TVM_ROOT). "
                             "Please set environment variable(TVM_ROOT) to your DRP-AI TVM root directory.")
        TVM_ROOT = os.path.abspath(TVM_ROOT)

        drpai_tvm_version = vars.get("DRPAI_TVM_VERSION")
        if drpai_tvm_version not in SRC_CMAKE_FILES['V2L']:
            raise RuntimeError("[Internal Error] The specified version was not found within SRC_CMAKE_FILES. "
                               "Please ensure that a version is specified in SRC_CMAKE_FILES. "
                               f"{drpai_tvm_version=} {SRC_CMAKE_FILES['V2L'].keys()=}")

        copy_file_dict = {
            f"{TVM_ROOT}/apps/MeraDrpRuntimeWrapper.cpp": f"{output_dst_dir}",
            f"{TVM_ROOT}/apps/MeraDrpRuntimeWrapper.h": f"{output_dst_dir}",
            f"{TVM_ROOT}/apps/PreRuntime.cpp": f"{output_dst_dir}",
            f"{TVM_ROOT}/apps/PreRuntime.h": f"{output_dst_dir}",
            f"{mod_path}/{SRC_CMAKE_FILES['V2L'][drpai_tvm_version][0]}": f"{output_dst_dir}",
            f"{mod_path}/{SRC_CMAKE_FILES['V2L'][drpai_tvm_version][1]}": f"{output_dst_dir}/toolchain"
        }
        self._copy_files_dict_for_src = copy_file_dict

    def get_copy_files_dict(self, type=None):
        if type == "src":
            return self._copy_files_dict_for_src
        elif type == "exe":
            return self._copy_files_dict_for_exe
        else:
            raise NotImplementedError


class V2HCopyFilesFactory(BaseCopyFilesFactory):
    def __init__(self,
                 output_dir,
                 drpai_tvm_model_obj_dir,
                 preprocess_obj_dirs):
        self._output_dir = output_dir
        self._drpai_tvm_model_obj_dir = rel2abspath(drpai_tvm_model_obj_dir)
        self._preprocess_obj_dirs = rel2abspath(preprocess_obj_dirs)
        self._copy_files_dict_for_exe = None
        self._copy_files_dict_for_src = None

    def create_copy_files_dict_for_exe(self):
        abs_drpai_tvm_model_obj_dir = self._drpai_tvm_model_obj_dir
        output_dst_dir = self._output_dir
        drpai_tvm_model_obj_dirname = os.path.basename(abs_drpai_tvm_model_obj_dir)
        copy_file_dict = {
            f"{abs_drpai_tvm_model_obj_dir}/deploy.json": f"{output_dst_dir}/{drpai_tvm_model_obj_dirname}",
            f"{abs_drpai_tvm_model_obj_dir}/deploy.params": f"{output_dst_dir}/{drpai_tvm_model_obj_dirname}",
            f"{abs_drpai_tvm_model_obj_dir}/deploy.so": f"{output_dst_dir}/{drpai_tvm_model_obj_dirname}",
        }

        if self._preprocess_obj_dirs is None:
            self._copy_files_dict_for_exe = copy_file_dict
            return

        for abs_preprocess_obj_dir in self._preprocess_obj_dirs:
            if abs_preprocess_obj_dir is None:
                continue
            dirname = os.path.basename(abs_preprocess_obj_dir)
            # Concat drpai_tvm_model obj directory name and preprocess obj directory name
            dst_dirname = os.path.join(drpai_tvm_model_obj_dirname, dirname)

            temp_dict = {
                f"{abs_preprocess_obj_dir}/aimac_desc.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/aimac_cmd.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/aimac_param_desc.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/aimac_param_cmd.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_desc.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_param.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_config.mem": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/weight.bin": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/drp_param_info.txt": f"{output_dst_dir}/{dst_dirname}",
                f"{abs_preprocess_obj_dir}/addr_map.txt": f"{output_dst_dir}/{dst_dirname}",
            }
            copy_file_dict.update(temp_dict)

        self._copy_files_dict_for_exe = copy_file_dict

    def create_copy_files_dict_for_src(self):
        output_dst_dir = self._output_dir
        mod_path = os.path.dirname(os.path.abspath(renesas_aicompiler_caller.__file__))
        TVM_ROOT = os.getenv("TVM_ROOT", None)
        if TVM_ROOT is None:
            raise ValueError("No environment variable(TVM_ROOT). "
                             "Please set environment variable(TVM_ROOT) to your DRP-AI TVM root directory.")

        TVM_ROOT = os.path.abspath(TVM_ROOT)
        drpai_tvm_version = vars.get("DRPAI_TVM_VERSION")
        PRODUCT = vars.get("PRODUCT")
        if drpai_tvm_version not in SRC_CMAKE_FILES[PRODUCT]:
            raise RuntimeError("[Internal Error] The specified version was not found within SRC_CMAKE_FILES. "
                               "Please ensure that a version is specified in SRC_CMAKE_FILES. "
                               f"{drpai_tvm_version=} {SRC_CMAKE_FILES[PRODUCT].keys()=}")

        copy_file_dict = {
            f"{TVM_ROOT}/apps/MeraDrpRuntimeWrapper.cpp": f"{output_dst_dir}",
            f"{TVM_ROOT}/apps/MeraDrpRuntimeWrapper.h": f"{output_dst_dir}",
            f"{TVM_ROOT}/apps/PreRuntimeV2H.cpp": f"{output_dst_dir}",
            f"{TVM_ROOT}/apps/PreRuntime.h": f"{output_dst_dir}",
            f"{mod_path}/{SRC_CMAKE_FILES[PRODUCT][drpai_tvm_version][0]}": f"{output_dst_dir}",
            f"{mod_path}/{SRC_CMAKE_FILES[PRODUCT][drpai_tvm_version][1]}": f"{output_dst_dir}/toolchain"
        }
        self._copy_files_dict_for_src = copy_file_dict

    def get_copy_files_dict(self, type=None):
        if type == "src":
            return self._copy_files_dict_for_src
        elif type == "exe":
            return self._copy_files_dict_for_exe
        else:
            raise NotImplementedError
