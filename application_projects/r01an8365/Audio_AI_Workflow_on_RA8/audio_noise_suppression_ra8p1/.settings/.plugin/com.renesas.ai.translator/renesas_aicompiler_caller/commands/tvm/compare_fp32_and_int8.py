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
import os
import argparse
import sys
import numpy as np
import tvm

import renesas_aicompiler_caller # noqa
from renesas_aicompiler_caller.utils import LogFactory
from renesas_aicompiler_caller.commands.tvm.utils import load_shape_dict_json

logger = LogFactory().add_console_handler() \
                     .add_file_handler(filename=f"{os.path.basename(__file__)}.log") \
                     .build(__name__)

CALC_COS_SIM_ERROR = -10000


def parse_args(args):
    """Parse command line arguments

    Args:
        args (list): Command-line arguments, typically sys.argv[1:].

    Returns:
        args: Arguments from command line.
    """
    parser = argparse.ArgumentParser(description='Compare FP32 results and INT8 results')
    parser.add_argument('--fp32_dir',
                        required=True,
                        help='Path to the directory containing the FP32 object files converted by TVM')
    parser.add_argument('--int8_dir',
                        required=True,
                        help='Path to the directory containing the INT8 object files converted by TVM')
    parser.add_argument('--shape_dict_json',
                        type=str,
                        required=True,
                        help='Json files that have the input node name and its corresponding input shape. '
                             'Json contents example is '
                             '''
                             {
                                    "input1":[1,3,224,224],
                                    "input2":[1,3,416,416]
                             }
                             ''')
    parser.add_argument('--num_inferences',
                        type=int,
                        default=3,
                        help='Number of inference to compare the FP32 and INT8 model output')
    args = parser.parse_args(args)
    return args


def get_rt_mod(conv_result_dir):
    """Get TVM runtime module

    Args:
        conv_result_dir: Path to the directory containing the object files converted by TVM.

    Returns:
        rt_mod: TVM Runtime Module
    """
    logger.info(f"Load {conv_result_dir} ...")

    loaded_lib = tvm.runtime.load_module(os.path.join(conv_result_dir, 'deploy.so'))
    logger.info("    lib loaded ...")

    loaded_json = open(os.path.join(conv_result_dir, 'deploy.json')).read()
    logger.info("    json loaded ...")

    loaded_params = bytearray(open(os.path.join(conv_result_dir, 'deploy.params'), "rb").read())
    logger.info("    params loaded ...")

    ctx = tvm.runtime.cpu()
    rt_mod = tvm.contrib.graph_executor.create(loaded_json, loaded_lib, ctx)
    rt_mod.load_params(loaded_params)

    return rt_mod


def cos_sim(v1, v2):
    """Calculate cosine similarity

    Args:
        v1: Vector
        v2: Vector

    Returns:
        Cosine similarity
    """
    dominator = (np.linalg.norm(v1) * np.linalg.norm(v2))
    numerator = np.dot(v1, v2)

    # Output from the anormaly detection model may cause the cosine similarity
    # to be incorrectly calculated, returning error value
    if dominator == 0.0 and numerator == 0.0:
        logger.error("Failed to calculate cosine similarity since dominator and numerator almost equal zero.")
        return CALC_COS_SIM_ERROR

    return numerator / dominator


def main(args):
    # Parse command line arguments
    args = parse_args(args)

    shape_dict = load_shape_dict_json(args.shape_dict_json)

    # Get runtime module
    fp32_rt_mod = get_rt_mod(args.fp32_dir)
    int8_rt_mod = get_rt_mod(args.int8_dir)

    np.random.seed(0)
    failed_to_calc_cos_sim = False
    total_sim = 0
    total_mse = 0
    for num_inf in range(args.num_inferences):
        logger.info(f"Start {num_inf} inference ...")

        # Set input to start inference
        for name, shape in shape_dict.items():
            test_data = np.random.uniform(-1.0, 1.0, shape).astype(np.float32)
            fp32_rt_mod.set_input(name, test_data.copy())
            int8_rt_mod.set_input(name, test_data.copy())

        # Perform inference
        fp32_rt_mod.run()
        int8_rt_mod.run()

        # Get inference result
        assert (fp32_rt_mod.get_num_outputs() == int8_rt_mod.get_num_outputs())
        for i in range(fp32_rt_mod.get_num_outputs()):
            logger.info(f"  Get {i}th output...")
            fp32_output = fp32_rt_mod.get_output(i).numpy()
            int8_output = int8_rt_mod.get_output(i).numpy()

            mse = np.mean(np.square(fp32_output - int8_output))
            logger.info(f"    MSE: {mse:.6f}")
            total_mse += mse

            sim = cos_sim(fp32_output.flatten(), int8_output.flatten())
            if sim == CALC_COS_SIM_ERROR:
                failed_to_calc_cos_sim = True
            logger.info(f"    Cosine similarity: {sim:.6f}")
            total_sim += sim

    total_mse /= (fp32_rt_mod.get_num_outputs() * args.num_inferences)
    total_sim /= (fp32_rt_mod.get_num_outputs() * args.num_inferences)
    logger.info(f"Total MSE              : {total_mse:.6f}")

    if failed_to_calc_cos_sim:
        raise RuntimeError("Failed to calculate cosine similarity")
    else:
        logger.info(f"Total cosine similarity: {total_sim:.6f}")


################################################################################
# Main function
################################################################################
if __name__ == "__main__":
    main(sys.argv[1:])
