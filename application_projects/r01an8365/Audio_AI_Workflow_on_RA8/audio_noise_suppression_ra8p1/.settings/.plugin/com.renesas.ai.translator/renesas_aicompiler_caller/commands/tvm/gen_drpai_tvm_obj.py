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
import json
import sys

import renesas_aicompiler_caller  # noqa
from renesas_aicompiler_caller.utils import VariableContainer, \
                                     LogFactory
from renesas_aicompiler_caller.commands.tvm.utils import create_model_converter, \
                                                  load_shape_dict_json

vars = VariableContainer()

logger = LogFactory().add_console_handler() \
                     .build(__name__)

TARGET_ARCHITECTURE = "arm"
INTERPRETER = False


def parse_args(args):
    """Parse command line arguments

    Args:
        args (list): Command-line arguments, typically sys.argv[1:].

    Returns:
        args: Arguments from command line.
    """
    parser = argparse.ArgumentParser(description='Generate DRP-AI TVM Model object')
    parser.add_argument('--framework',
                        required=True,
                        type=str,
                        choices=["onnx", "pytorch", "tflite"],
                        help='Framework of input model file. This option can be set onnx, pytorch or tflite.')
    parser.add_argument('--input_model',
                        required=True,
                        type=lambda x: _validate_extension(parser,
                                                           ("onnx",
                                                            "pt",
                                                            "tflite"), x),
                        help='Path to input model file.')
    parser.add_argument('--optimization_level',
                        type=int,
                        default=1,
                        choices=[0, 1],
                        help='Optimization level at compile. "1" is the default and '
                             'compiles with optimal settings. '
                             'If "0" is set to this option, complex models can be deployed, '
                             'but inference speed is slower.')
    parser.add_argument('--use_only_cpu',
                        action='store_true',
                        help='Use only cpu. When specify this option, converted model will run on cpu, not DRP-AI.')
    parser.add_argument('--addr_map_start',
                        type=int,
                        default=0x00000000,
                        help='`addr_map_start` is the memory address that DRP-AI TVM Model Object is deployed.')
    parser.add_argument('--toolchain_dir',
                        type=str,
                        default=vars.get("TRANSLATOR"),
                        help='DRP-AI Translator root directory')
    parser.add_argument('--sdk_root',
                        type=str,
                        default=vars.get("SDK"),
                        help='Cross-compilation toolchain root directory')
    parser.add_argument('--output_dir',
                        required=True,
                        type=str,
                        help='Path to output directory')

    parser.add_argument('--shape_dict_json',
                        type=str,
                        help='Json files that have the input node name and its corresponding input shape. '
                             'Json contents example is '
                             '''
                             {
                                    "input1":[1,3,224,224],
                                    "input2":[1,3,416,416]
                             }
                             ''')
    parser.add_argument('--quant_config_json',
                        type=str,
                        help='Json file that has the quantization settings. ')
    parser.add_argument('--quantization_tool_dir',
                        type=str,
                        default=vars.get("QUANTIZER"),
                        help='Quantizatioin toolchain root directory')

    args = parser.parse_args(args)
    return args


def _validate_args(args):  # noqa
    if args.framework == "pytorch" and args.shape_dict_json is None:
        raise ValueError("If `framework` specify as `pytorch`, `shape_dict_json` must be set.")

    # Check framework compatible with input model
    ext = os.path.splitext(args.input_model)[-1][1:]
    if args.framework == "pytorch" and ext != "pt":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of pytorch, `input_model` extension must be '.pt'.")
    elif args.framework == "onnx" and ext != "onnx":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of onnx, `input_model` extension must be '.onnx'.")
    elif args.framework == "tflite" and ext != "tflite":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of tflite, `input_model` extension must be '.tflite'.")

    # Check sdk root
    if not os.path.isfile(f"{args.sdk_root}/sysroots/x86_64-pokysdk-linux"
                          "/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++"):
        raise ValueError(f"`sdk_root`({args.sdk_root}) is invalid path. "
                         "Please check if the cross compiler is installed in the `sdk_root` path.")

    # Check toolchain_dir
    if vars.get("PRODUCT").lower() == "v2l" and \
       os.path.isdir(os.path.join(args.toolchain_dir, "DRP-AI_translator")) is False:
        raise ValueError(f"`toolchain_dir`({args.toolchain_dir}) is invalid path. "
                         "Please check if the DRP-AI Translator is installed in the `toolchain_dir` path.")

    if vars.get("PRODUCT").lower() == "v2h" and \
       os.path.isdir(os.path.join(args.toolchain_dir, "DRP-AI_Translator")) is False:
        raise ValueError(f"`toolchain_dir`({args.toolchain_dir}) is invalid path. "
                         "Please check if the DRP-AI Translator is installed in the `toolchain_dir` path.")

    # Check quantization settings
    if vars.get("PRODUCT").lower() == "v2h" and args.quant_config_json is None:
        raise ValueError("`quant_config_json` is invalid. "
                         "`--quant_config_json` must be set for V2H.")

    if vars.get("PRODUCT").lower() == "v2h" and args.quantization_tool_dir is None:
        raise ValueError("`quantization_tool_dir` is invalid. "
                         "`--quantization_tool_dir` must be set for V2H.")

    # Check addr_map_start
    BYTE_ALIGN = 0x3F
    if 0 != (args.addr_map_start & BYTE_ALIGN):
        offset = args.addr_map_start % 64
        suggest_addrmap_start = (args.addr_map_start - offset) + 64
        raise ValueError("`addr_map_start` must be 64 byte alignment. "
                         f"`addr_map_start` was set to {args.addr_map_start}. "
                         f"Please set `addr_map_start` to {suggest_addrmap_start}.")


def _validate_extension(parser, ext_choices, filename):
    ext = os.path.splitext(filename)[-1][1:]
    if ext not in ext_choices:
        parser.error("Unsupported file is input. "
                     f"This tool supports the file that has the extension {ext_choices}.")
    return filename


def load_quant_configs_json(quant_configs_json):
    """Load quantization configs json

    Args:
        quant_config_json: Path to quantization configuration (*.json).

    Returns:
        quant_configs: Dictionary containing quantization settings
    """
    # Load preprocess_config_json as dict format
    with open(quant_configs_json) as json_file:
        quant_configs = json.load(json_file)

    return quant_configs


def main(args):
    # Parse command line arguments
    args = parse_args(args)
    _validate_args(args)

    # Load shape_dict json
    if args.shape_dict_json is None:
        shape_dict = None
    else:
        shape_dict = load_shape_dict_json(args.shape_dict_json)

    # Load quant_config json if Product is v2h or v2n
    quant_configs_dict = {}
    if vars.get("PRODUCT").lower() == "v2h" or vars.get("PRODUCT").lower() == "v2n":
        quant_configs_dict = load_quant_configs_json(args.quant_config_json)
        quant_configs_dict.update({"quantization_tool_dir": args.quantization_tool_dir})

    logger.info("[TVM compile start...]")

    # Create converter name
    version_str = "".join(vars.get("DRPAI_TVM_VERSION").split('.')).upper()  # e.g. v1.1.1 => V111
    product_str = vars.get("PRODUCT").upper()  # e.g. v2l => V2L
    converter_name_str_list = []
    converter_name_str_list.append(version_str)
    converter_name_str_list.append("Rz")
    converter_name_str_list.append(product_str if args.use_only_cpu is False else "CPU")
    converter_name_str_list.append("Model")
    converter_name_str_list.append("Converter")
    converter_name = ''.join(s.capitalize() for s in converter_name_str_list)
    logger.info(f"Used converter: {converter_name=}")

    converter = create_model_converter(converter_name)

    # Make output directory
    os.makedirs(args.output_dir, exist_ok=True)

    # Run TVM frontend compiler
    mod, params, shape_dict = converter.execute_frontend(
        framework=args.framework,
        input_model=args.input_model,
        shape_dict=shape_dict
    )

    # Run TVM backend compiler
    converter.execute_backend(
        mod,
        params,
        args.output_dir,
        target_architecture=TARGET_ARCHITECTURE,
        disable_concat=False if args.optimization_level == 1 else True,
        interpreter=INTERPRETER,
        addr_map_start=args.addr_map_start,
        toolchain_dir=args.toolchain_dir,
        sdk_root=args.sdk_root,
        use_only_cpu=args.use_only_cpu,
        shape_dict=shape_dict,
        trans_nhwc2nchw=False if args.framework == "tflite" else True,
        **quant_configs_dict
    )

    logger.info("[TVM compile finished]")
    logger.info("Please check {} directory".format(args.output_dir))


################################################################################
# Main function
################################################################################
if __name__ == "__main__":
    main(sys.argv[1:])
