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
import sys
import argparse
from pathlib import Path
import platform

# To append the drpai tvm plug path
sys.path.append(str(Path(__file__).resolve().parent.parent))
import renesas_aicompiler_caller  # noqa

registered_commands = None
if platform.system() == "Windows":
    import commands.ruhmi.deploy_model  # noqa
    import commands.ruhmi.quantize_model  # noqa
    import commands.ruhmi.get_ethos_config  # noqa
    import commands.common.get_model_info  # noqa
    registered_commands = {
        'ruhmi deploy': ("Deploy a model with RUHMI", commands.ruhmi.deploy_model.main),
        'ruhmi quantize': ("Quantize a model with RUHMI", commands.ruhmi.quantize_model.main),
        'ruhmi get_ethos_config': ("Get a ethos system config from RUHMI", commands.ruhmi.get_ethos_config.main),
        'comm get_model_info': ("Get a model information", commands.common.get_model_info.main)
    }
elif platform.system() == "Linux":
    # To avoid the segmentation fault, firstly import tensorflow library
    import torch  # noqa
    import tensorflow as tf  # noqa
    import commands.common.get_model_info  # noqa
    import commands.tvm.gen_drpai_tvm_obj  # noqa
    import commands.tvm.gen_preprocess_obj  # noqa
    import commands.tvm.gen_drpai_tvm_sample_app  # noqa
    import commands.tvm.compare_fp32_and_int8  # noqa
    import commands.common.pth2pt  # noqa
    registered_commands = {
        'comm get_model_info': ("Get a model information", commands.common.get_model_info.main),
        'tvm gen_drpai_tvm_obj': ("Generate  a DRP-AI TVM Model Object", commands.tvm.gen_drpai_tvm_obj.main),
        'tvm gen_preprocess_obj': ("Generate  a DRP-AI Preprocess Runtime Object",
                                   commands.tvm.gen_preprocess_obj.main),
        'tvm gen_drpai_tvm_sample_app': ("Generate  a sample code for DRP-AI TVM",
                                         commands.tvm.gen_drpai_tvm_sample_app.main),
        'tvm compare_fp32_and_int8': ("Compare outputs between fp32 and int8 models",
                                      commands.tvm.compare_fp32_and_int8.main),
        'comm convert_pth2pt': ("Convert the model format from pth to pt",  commands.common.pth2pt.main)
    }
else:
    raise RuntimeError(f"Unsupported system was detected: {platform.system()}")


def build_parser():
    desc = "Available commands:\n"
    for cmd, (desc_text, _) in registered_commands.items():
        desc += f"  {cmd:<18} {desc_text}\n"

    parser = argparse.ArgumentParser(
        prog="cli.py",
        description=desc,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("top_command", nargs="?", help="Main command (e.g. ruhmi)")
    parser.add_argument("sub_command", nargs="?", help="Subcommand (e.g. deploy, quantize, get_ethos_config)")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="Arguments for subcommand")
    return parser


def dispatch(argv):

    parser = build_parser()
    args = parser.parse_args(argv)

    if not args.top_command or not args.sub_command:
        parser.print_help()
        return

    command_key = f"{args.top_command} {args.sub_command}"

    if command_key not in registered_commands:
        print(f"Unknown command: {command_key}")
        parser.print_help()
        sys.exit(1)

    _, handler = registered_commands[command_key]

    # Run the main() function for specified command
    handler(args.args)


if __name__ == '__main__':
    dispatch(sys.argv[1:])
