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
import argparse
import sys
import tempfile
import shutil
import subprocess
from distutils.dir_util import copy_tree
from pathlib import Path

import renesas_aicompiler_caller # noqa
from renesas_aicompiler_caller.commands.tvm.gen_preprocess_obj import load_preprocess_config_json
from renesas_aicompiler_caller.utils import LogFactory, \
                                     VariableContainer
from renesas_aicompiler_caller.commands.tvm.utils import SampleAppCppWriter, \
                                                  reorder_pre_settings, \
                                                  get_input_order_from_depjson, \
                                                  get_code_gen_mode, \
                                                  CodeGenMode, \
                                                  V2LCopyFilesFactory, \
                                                  V2HCopyFilesFactory, \
                                                  CopyFilesDirector

logger = LogFactory().add_console_handler() \
                     .build(__name__)
vars = VariableContainer()

SINGLE_INPUT = 1


def parse_args(args):
    """Parse command line arguments

    Args:
        args (list): Command-line arguments, typically sys.argv[1:].

    Returns:
        args: Arguments from command line.
    """
    parser = argparse.ArgumentParser(description='Generate DRP-AI TVM sample',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('--output_dir',
                        required=True,
                        type=str,
                        help='Path to output directory')
    parser.add_argument('--drpai_tvm_model_obj_dir',
                        required=True,
                        help='Path to DRP-AI TVM Model Object directory. \n'
                             'This directory must contain deploy.params, deploy.json and deploy.so.')
    parser.add_argument('--preprocess_config_json',
                        required=True,
                        help='Path to preprocess configuration (*.json). \n'
                             'e.g. --preprocess_config_json Preprocess_input1.json')
    parser.add_argument('--preprocess_obj_dirs',
                        type=str,
                        nargs='+',
                        help='Path to preprocess object directory. \n'
                             'If model has multiple inputs, please specify the same number of paths as model inputs. \n'
                             'e.g. --preprocess_obj_dirs Preprocess_input1 Preprocess_input2 ... ')

    args = parser.parse_args(args)
    return args


def _validate_args(args):
    if vars.get("DBG_CODE_GEN_SNIPPET"):
        return

    if args.preprocess_obj_dirs is None:
        return

    # If directory name is same, raise an error
    # e.g. the following case must be error.
    # - foo/hoo/preprocess
    # - foo/preprocess
    for i in range(len(args.preprocess_obj_dirs)):
        dirname1 = os.path.basename(args.preprocess_obj_dirs[i])
        for j in range(i+1, len(args.preprocess_obj_dirs)):
            dirname2 = os.path.basename(args.preprocess_obj_dirs[j])
            if dirname1 == dirname2:
                raise RuntimeError("Unsupported preprocessing object output directories are input. "
                                   "The directories name containing preprocessing object must use difference name. \n"
                                   "The following directories are the cause. \n"
                                   f"- {args.preprocess_obj_dirs[i]} \n"
                                   f"- {args.preprocess_obj_dirs[j]} \n"
                                   "Change preprocessing object output directories name. ")


def copy_src_file_to_dst_dir(copy_file_dict):
    """Copy source file path to desired folder.

    Args:
        copy_file_dict: Dictionary containing src file path and dst folder path

    Returns:
        None

    Note:
        key of copy_file_dict must be file path.
        value of copy_file_dict must be folder path.
    """
    for src, dst in copy_file_dict.items():
        try:
            os.makedirs(dst, exist_ok=True)
            shutil.copy(src, dst)
        except Exception as e:
            raise RuntimeError(f"Failed to copy: {src} to {dst}. "
                               f"Error details: {e}")


def gen_src_dir_contents(drpai_tvm_model_obj_dir,
                         preprocess_obj_dirs,
                         preprocess_configs,
                         output_dir,
                         code_gen_mode,
                         target_dir="src"):
    """Generate sample application contents for src folder.

    Args:
        drpai_tvm_model_obj_dir: Path to DRP-AI TVM Model Object directory.
        preprocess_obj_dirs: List containing path to preprocess object directory.
        preprocess_configs: List containing preprocess configuration.
        output_dir: Path to the output root directory
        code_gen_mode: Value for code gen mode to output sample code
        target_dir: The name of the folder to be created in output_dir.

    Returns:
        None
    """
    logger.info("Start to generate src folder's contents for sample application...")

    # Make output directory
    output_dst_dir = "{}/{}".format(output_dir, target_dir)
    os.makedirs(output_dst_dir, exist_ok=True)
    logger.debug(f"Temporary src directory: {output_dst_dir}")

    # Copy file from DRP-AI TVM environment or this module
    copy_file_dict = None
    if vars.get('PRODUCT').lower() == "v2l":
        v2l_builder = V2LCopyFilesFactory(output_dst_dir,
                                          drpai_tvm_model_obj_dir,
                                          preprocess_obj_dirs)
        director = CopyFilesDirector(v2l_builder)
        director.construct()
        copy_file_dict = v2l_builder.get_copy_files_dict(type="src")
    elif vars.get('PRODUCT').lower() == "v2h" or vars.get('PRODUCT').lower() == "v2n":
        v2h_builder = V2HCopyFilesFactory(output_dst_dir,
                                          drpai_tvm_model_obj_dir,
                                          preprocess_obj_dirs)
        director = CopyFilesDirector(v2h_builder)
        director.construct()
        copy_file_dict = v2h_builder.get_copy_files_dict(type="src")

    logger.debug(f"copy_file_dict: {copy_file_dict}")
    copy_src_file_to_dst_dir(copy_file_dict)

    # Generate sample_app.cpp file
    writer = SampleAppCppWriter(product=vars.get('PRODUCT'),
                                filename=f"{output_dst_dir}/sample_app.cpp",
                                drpai_tvm_model_obj_dir=drpai_tvm_model_obj_dir,
                                preprocess_obj_dirs=preprocess_obj_dirs,
                                preprocess_configs=preprocess_configs,
                                code_gen_mode=code_gen_mode,
                                drpai_tvm_version=vars.get('DRPAI_TVM_VERSION'))

    try:
        writer.write_code()
    except Exception as e:
        raise RuntimeError("Failed to write a sample code. "
                           f"Error details: {e}")
    finally:
        writer.close()


def gen_exe_dir_contents(drpai_tvm_model_obj_dir,
                         preprocess_obj_dirs,
                         output_dir,
                         target_dir="exe"):
    """Generate sample application contents for exe folder.

    Args:
        drpai_tvm_model_obj_dir: Path to DRP-AI TVM Model Object directory.
        preprocess_obj_dirs: List containing path to preprocess object directory.
        output_dir: Path to the output root directory
        target_dir: The name of the folder to be created in output_dir.

    Returns:
        None

    Notes:
        This function generates the following file sturcture from arguments.
        ```
        output_dir/
            exe/
                drpai_tvm_model_obj_dir/
                    deploy.json
                    deploy.params
                    deploy.so
                    preprocess_obj_dirs[0]/
                        aimac_desc.bin
                        drp_desc.bin
                        drp_param_info.txt
                        drp_param.bin
                        pp_addrmap_intm.txt
                        pp_drpcfg.mem
                        pp_weight.dat
        ```
    """
    logger.info("Start to generate exe folder's contents for sample application...")

    # Make output directory
    output_dst_dir = "{}/{}".format(output_dir, target_dir)
    os.makedirs(output_dst_dir, exist_ok=True)
    logger.debug(f"Temporary exe directory: {output_dst_dir}")

    # Copy file from input directory
    copy_file_dict = None
    if vars.get('PRODUCT').lower() == "v2l":
        v2l_builder = V2LCopyFilesFactory(output_dst_dir,
                                          drpai_tvm_model_obj_dir,
                                          preprocess_obj_dirs)
        director = CopyFilesDirector(v2l_builder)
        director.construct()
        copy_file_dict = v2l_builder.get_copy_files_dict(type="exe")
    elif vars.get('PRODUCT').lower() == "v2h" or vars.get('PRODUCT').lower() == "v2n":
        v2h_builder = V2HCopyFilesFactory(output_dst_dir,
                                          drpai_tvm_model_obj_dir,
                                          preprocess_obj_dirs)
        director = CopyFilesDirector(v2h_builder)
        director.construct()
        copy_file_dict = v2h_builder.get_copy_files_dict(type="exe")
    else:
        raise NotImplementedError("[Internal error] Contact to Renesas with the following error message. \n"
                                  "Failed to create copy_file_dict for exe directory. "
                                  "Unsupported product and version are specified. \n"
                                  f"Product: {vars.get('PRODUCT')}\n"
                                  f"Version: {vars.get('DRPAI_TVM_VERSION')}")

    logger.debug(f"copy_file_dict: {copy_file_dict}")
    copy_src_file_to_dst_dir(copy_file_dict)


def copy_from_tmp_to_dst(tmp_dir, dst_dir):
    """Copy from temporary directory to destination directory.

    Args:
        tmp_dir: Path to temporary directory
        dst_dir: Path to destination directory

    Returns:
        None
    """
    try:
        os.makedirs(dst_dir, exist_ok=True)
        copy_tree(tmp_dir, dst_dir)
    except Exception as e:
        raise RuntimeError(f"Failed to copy from {tmp_dir} to {dst_dir}. "
                           f"Error details: {e}")


def build_generated_src(contents_dir):
    """Build generated source code

    Args:
        contents_dir: Path to directory containing `src` and `exe` directory.

    Returns:
        None
    """
    activator_path = Path(vars.get("SDK")) / "environment-setup-aarch64-poky-linux"
    if activator_path.exists() is False:
        activator_path = Path(vars.get("SDK")) / "environment-setup-cortexa55-poky-linux"

    cmd = (f"cd {contents_dir}/src;"
           f". {activator_path};"
           "mkdir build;"
           "cd build;"
           "cmake -DCMAKE_TOOLCHAIN_FILE=./../toolchain/runtime.cmake ..;"
           "make -j$(nproc);"
           "date;"
           "ls -al sample_app;")

    proc = subprocess.Popen(cmd,
                            shell=True,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)

    # Output build log
    while proc.poll() is None:
        line = proc.stdout.readline().decode("utf-8")
        print(line, end="")

    if proc.returncode != 0:
        raise RuntimeError("Failed to build the generated source code. "
                           "Please contact Renesas.")

    # Move the built app to exe directory.
    build_app = f"{contents_dir}/src/build/sample_app"
    dst_dir = f"{contents_dir}/exe"
    try:
        shutil.move(build_app, dst_dir)
    except Exception as e:
        raise RuntimeError(f"Failed to move from {build_app} to {dst_dir}. "
                           f"Error details: {e}")

    # Delete temporary build directory
    build_dir = f"{contents_dir}/src/build"
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
        logger.info(f"Delete the existed directory: {build_dir}")


def main(args):
    """Generate DRP-AI TVM sample

    When executing this script, the following files are generated...
        ```
        output_dir/
            exe/
                sample_app
                drpai_translator_outputs/
                    deploy.json
                    deploy.params
                    deploy.so
                    Preprocess_XXX/
                        aimac_desc.bin
                        drp_desc.bin
                        drp_param_info.txt
                        drp_param.bin
                        pp_addrmap_intm.txt
                        pp_drpcfg.mem
                        pp_weight.dat
            src/
                sample_app.cpp            // Generate this code
                MeraDrpRuntimeWrapper.cpp // Copy from AI SDK
                MeraDrpRuntimeWrapper.h   // Copy from AI SDK
                PreRuntime.cpp            // Copy from AI SDK
                PreRuntime.h              // Copy from AI SDK
                CMakeLists.txt            // Copy from this module
                toolchain/
                    runtime.cmake         // Copy from this module
        ```
    """

    # Parse command line arguments
    args = parse_args(args)
    logger.info(args)
    _validate_args(args)

    # Load preprocess config json
    preprocess_configs = load_preprocess_config_json(args.preprocess_config_json)
    code_gen_mode = get_code_gen_mode(preprocess_configs["nodes"])
    logger.debug("Code gen mode: {}".format(code_gen_mode))
    if code_gen_mode == CodeGenMode.USE_PREPRO_RUNTIME and \
       len(preprocess_configs["nodes"]) != len(args.preprocess_obj_dirs):
        raise RuntimeError("--preprocess_obj_dirs argument is invalid. "
                           "The number of preprocess_obj_dirs arguments "
                           "must be same for the number of input nodes in preprocess_config_json.")
    elif code_gen_mode == CodeGenMode.DONT_USE_PREPRO_RUNTIME and args.preprocess_obj_dirs is not None:
        raise RuntimeError("--preprocess_obj_dirs argument is invalid. "
                           "The preprocess_obj_dirs arguments must not be set. "
                           "Because the Pre-processing Runtime is not used in preprocess_config_json.")
    elif code_gen_mode == CodeGenMode.MIXED_USE_PREPRO_RUNTIME and \
            len(preprocess_configs["nodes"]) == len(args.preprocess_obj_dirs):
        raise RuntimeError("--preprocess_obj_dirs argument is invalid."
                           "The number of preprocess_obj_dirs arguments "
                           "must not be same for the number of input nodes in preprocess_config_json. "
                           "More specifically, The number of preprocess_obj_dirs arguments "
                           "must be same for the number of input nodes using the Pre-processing Runtime.")

    # Reorder the input files to follow deploy.json spec
    reordered_preprocess_obj_dirs = args.preprocess_obj_dirs
    reordered_preprocess_configs = preprocess_configs["nodes"]
    if (code_gen_mode == CodeGenMode.USE_PREPRO_RUNTIME and len(preprocess_configs["nodes"]) != SINGLE_INPUT) or \
       (code_gen_mode == CodeGenMode.MIXED_USE_PREPRO_RUNTIME and len(preprocess_configs["nodes"]) != SINGLE_INPUT):
        logger.info("Detect a model which has multiple inputs.")
        right_input_order = get_input_order_from_depjson(args.drpai_tvm_model_obj_dir,
                                                         len(preprocess_configs["nodes"]))

        # Append None
        if code_gen_mode == CodeGenMode.MIXED_USE_PREPRO_RUNTIME:
            logger.debug(f"Before appending: {args.preprocess_obj_dirs}")
            diff = len(preprocess_configs["nodes"]) - len(args.preprocess_obj_dirs)
            for i in range(diff):
                args.preprocess_obj_dirs.append(None)
            logger.debug(f"After appending: {args.preprocess_obj_dirs}")

        reordered_preprocess_obj_dirs, reordered_preprocess_configs = reorder_pre_settings(right_input_order,
                                                                                           args.preprocess_obj_dirs,
                                                                                           preprocess_configs["nodes"])

    logger.info("[Start to generate DRP-AI TVM sample...]")
    tmpdir = tempfile.TemporaryDirectory()
    output_dir = "{}/{}".format(tmpdir.name, args.output_dir)
    logger.debug(f"Temporary directory: {output_dir}")

    # Create `src` folder contents for sample application
    gen_src_dir_contents(args.drpai_tvm_model_obj_dir,
                         reordered_preprocess_obj_dirs,
                         reordered_preprocess_configs,
                         output_dir,
                         code_gen_mode)

    # Create `exe` folder contents for sample application
    gen_exe_dir_contents(args.drpai_tvm_model_obj_dir,
                         reordered_preprocess_obj_dirs,
                         output_dir)

    # Build the generated source code
    build_generated_src(output_dir)

    # Copy from temporary directory to output directory
    copy_from_tmp_to_dst(output_dir, args.output_dir)

    logger.info("[Finish to generate DRP-AI TVM sample]")


################################################################################
# Main function
################################################################################
if __name__ == "__main__":
    main(sys.argv[1:])
