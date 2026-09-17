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

import renesas_aicompiler_caller # noqa
from renesas_aicompiler_caller.utils import LogFactory, VariableContainer
from drpai_preprocess import preruntime, drpai_param, op

logger = LogFactory().add_console_handler() \
                     .build(__name__)
vars = VariableContainer()


def parse_args(args):
    """Parse command line arguments

    Args:
        args (list): Command-line arguments, typically sys.argv[1:].

    Returns:
        args: Arguments from command line.
    """

    parser = argparse.ArgumentParser(description='Generate preprocess object')
    parser.add_argument('--preprocess_config_json',
                        type=str,
                        required=True,
                        help='Path to preprocess configuration (*.json)')
    args = parser.parse_args(args)
    return args


def load_preprocess_config_json(preprocess_config_json):
    """Load preprocess configration json

    Args:
        preprocess_config_json: Path to preprocess configuration (*.json).

    Returns:
        preprocess_config: Dictionary that contains configuration of preprocess operator.
    """
    # Load preprocess_config_json as dict format
    with open(preprocess_config_json) as json_file:
        preprocess_config = json.load(json_file)

    # Validate loaded dict data
    _validate_preprocess_config(preprocess_config)

    return preprocess_config


def _validate_preprocess_config(preprocess_config):
    """Validate preprocess config

    Args:
        preprocess_config: Dictionary that contains configuration of preprocess operator.

    Returns:
        None
    """

    # Validate preprocess config format
    _validate_preprocess_config_format(preprocess_config)


def _validate_preprocess_config_format(preprocess_config):
    if "nodes" not in preprocess_config.keys():
        raise Exception("Preprocess config must have `nodes` element.")

    # Validate format for each input node
    for node_preprocess_config in preprocess_config["nodes"]:
        # Validate priary element
        if "name" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `name` element.")
        logger.info("Validate node : {}".format(node_preprocess_config["name"]))

        if "output_dir_path" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `output_dir_path` element.")
        if "use_prepro_runtime" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `use_prepro_runtime` element.")
        if "io_type" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `io_type` element.")
        if "input_data_format" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `input_data_format` element.")
        if "output_data_format" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `output_data_format` element.")
        if "preprocess_op" not in node_preprocess_config.keys():
            raise Exception("Preprocess config must have `preprocess_op` element.")

        # Validate preprocess_op config
        preprocess_op_config = node_preprocess_config["preprocess_op"]
        _key_exists_in_config("Crop", preprocess_op_config, "preprocess_op")
        _key_exists_in_config("Resize", preprocess_op_config, "preprocess_op")
        _key_exists_in_config("Normalize", preprocess_op_config, "preprocess_op")

        # Validate Crop op
        crop_op_config = preprocess_op_config["Crop"]
        _key_exists_in_config("use_op", crop_op_config, "Crop")
        _key_exists_in_config("crop_tl_x", crop_op_config, "Crop")
        _key_exists_in_config("crop_tl_y", crop_op_config, "Crop")
        _key_exists_in_config("width", crop_op_config, "Crop")
        _key_exists_in_config("height", crop_op_config, "Crop")

        # Validate Resize op
        resize_op_config = preprocess_op_config["Resize"]
        _key_exists_in_config("use_op", resize_op_config, "Resize")
        _key_exists_in_config("width", resize_op_config, "Resize")
        _key_exists_in_config("height", resize_op_config, "Resize")
        _key_exists_in_config("algorithm", resize_op_config, "Resize")

        # Validate Normalize op
        norm_op_config = preprocess_op_config["Normalize"]
        _key_exists_in_config("use_op", norm_op_config, "Normalize")
        _key_exists_in_config("cof_add", norm_op_config, "Normalize")
        _key_exists_in_config("cof_mul", norm_op_config, "Normalize")


def _key_exists_in_config(key, config, config_name):
    if key not in config.keys():
        raise Exception(f"`{config_name}` element must have `{key}` element.")
    return True


def append_preprocess_op(op_set, preprocess_op_config, preprocess_op_name):
    """Append preprocess operator to op_set

    Args:
        op_set: Preprocess operator set.
        preprocess_op_config: Configuration of preprocess operator
        preprocess_op_name: Preprocess operator name. This argument can be set ["Crop", "Resize", "Normalize"].

    Returns:
        bool: If True, it is successful to append the preprocess_op_name to the op_set.
    """
    op_config = preprocess_op_config[preprocess_op_name]
    logger.info(f"Operator type: {preprocess_op_name}")
    logger.info(f"Operator params: {op_config}")

    use_op = op_config.pop("use_op")
    if use_op.lower() == "true":

        # For Resize operator, replace string to class instance
        if preprocess_op_name == "Resize":
            op_config["algorithm"] = op.Resize.__dict__[op_config["algorithm"]]

        op_set.append(
            op.__dict__[preprocess_op_name](**op_config)
        )
        return True
    return False


def gen_preprocess_obj(preprocess_config, output_dir):
    """Generate preprocess object

    Args:
        preprocess_config: Dictionary that has configuration of preprocess operator.
        output_dir: Path to output directory

    Returns:
        None
    """

    # Make output directory
    os.makedirs(output_dir, exist_ok=True)

    # 4.1. Define the pre-processing data
    config = preruntime.Config()

    # 4.1.1. Define input data of preprocessing
    input_data_format = preprocess_config["input_data_format"]
    config.shape_in = input_data_format["shape_in"]
    config.format_in = drpai_param.FORMAT.__dict__[input_data_format["format_in"]]
    config.order_in = drpai_param.ORDER.__dict__[input_data_format["order_in"]]
    config.type_in = drpai_param.TYPE.__dict__[input_data_format["type_in"]]
    logger.info(f"config.shape_in: {config.shape_in}")
    logger.info(f"config.format_in: {config.format_in}")
    logger.info(f"config.order_in: {config.order_in}")
    logger.info(f"config.type_in: {config.type_in}")

    # 4.1.2. Define output data of preprocessing (Will be model input)
    output_data_format = preprocess_config["output_data_format"]
    config.shape_out = output_data_format["shape_out"]
    config.format_out = drpai_param.FORMAT.__dict__[output_data_format["format_out"]]
    config.order_out = drpai_param.ORDER.__dict__[output_data_format["order_out"]]
    config.type_out = drpai_param.TYPE.__dict__[output_data_format["type_out"]]
    # Note: type_out depends on DRP-AI TVM[*1]. Usually FP32.
    logger.info(f"config.shape_out: {config.shape_out}")
    logger.info(f"config.format_out: {config.format_out}")
    logger.info(f"config.order_out: {config.order_out}")
    logger.info(f"config.type_out: {config.type_out}")

    # 4.1.3. Define operators to be run.
    config.ops = []
    preprocess_op_config = preprocess_config["preprocess_op"]
    append_preprocess_op(config.ops, preprocess_op_config, "Crop")
    append_preprocess_op(config.ops, preprocess_op_config, "Resize")
    append_preprocess_op(config.ops, preprocess_op_config, "Normalize")

    logger.info(f"config.ops: {config.ops}")

    # Run DRP-AI Pre-processing Runtime
    preruntime.PreRuntime(config, output_dir, vars.get("PRODUCT"))


def main(args):
    # Parse command line arguments
    args = parse_args(args)

    # Load preprocess configuration json
    preprocess_config = load_preprocess_config_json(args.preprocess_config_json)

    # Generate preprocess object for each input node
    for node_preprocess_config in preprocess_config["nodes"]:
        logger.info("Generate preprocess object for {}".format(node_preprocess_config["name"]))

        if node_preprocess_config["use_prepro_runtime"].lower() == "false":
            logger.info("Skip to generate preprocess object...")
            continue

        # Generate preprocess object
        gen_preprocess_obj(node_preprocess_config,
                           node_preprocess_config["output_dir_path"])


if __name__ == "__main__":
    main(sys.argv[1:])
