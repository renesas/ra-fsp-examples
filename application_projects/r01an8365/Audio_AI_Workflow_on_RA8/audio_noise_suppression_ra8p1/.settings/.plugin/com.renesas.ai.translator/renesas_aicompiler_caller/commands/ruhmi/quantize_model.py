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
import argparse
import sys
from pathlib import Path
import json
import numpy as np
import re
from typing import List
import tempfile
from PIL import Image
import mera
from mera import Platform
from renesas_aicompiler_caller.utils import LogFactory
from .utils import get_onnx_symbolic_shape_mapping

logger = LogFactory().add_console_handler() \
                     .build(__name__)

MAX_NUM_RANDOM_IMAGES = 5
IMAGE_DIM_LENGTH = 4
np.random.seed(0)


def load_calibration_data(mera_model, quant_config_dict, input_shape_dict, trans_nhwc2nchw) -> dict:
    """Load calibration data into the memory

    Args:
        mera_model: Mera model which is the model loaded by MERA APIs
        quant_config_dict: Dictionary containing the quantization configuration
        input_shape_dict: Dictionary containing the input shape
        trans_nhwc2nchw: If True, transpose generated data from NHWC to NCHW.
                         This option only apply when input data has 4 dimentions.

    Returns:
        Dictionary that contains the calibaration data
    """

    # Generate random data if Calibration data directory is empty
    quant_config_dict = generate_random_data(quant_config_dict=quant_config_dict,
                                             input_shape_dict=input_shape_dict,
                                             trans_nhwc2nchw=trans_nhwc2nchw)
    # Glob the calibaration data file path
    quant_config_dict = glob_calibration_data_file(quant_config_dict)

    # Read calibration data with preprocessing
    calib_data = read_calibration_data(mera_model=mera_model,
                                       quant_config_dict=quant_config_dict,
                                       input_shape_dict=input_shape_dict,
                                       trans_nhwc2nchw=trans_nhwc2nchw)
    return calib_data


def generate_random_data(quant_config_dict, input_shape_dict, trans_nhwc2nchw):

    calibration_info = quant_config_dict["nodes"]
    num_calibrate_samples = quant_config_dict["num_calibrate_samples"]

    updated_nodes = []
    # This loop iterates the number of times equal to the number of input nodes.
    for node_info in calibration_info:
        input_name = node_info["name"]
        if node_info["calibrate_sample_dir"] != "":
            updated_nodes.append(node_info)
            continue

        # Create a directory to store the random image
        temp_dir = tempfile.mkdtemp()
        node_info["calibrate_sample_dir"] = temp_dir

        logger.info(f"Generate sample images with npy extension to {temp_dir=}...")
        generated_sample_size = max(num_calibrate_samples, MAX_NUM_RANDOM_IMAGES)
        mean = node_info["mean"]
        std = node_info["std"]
        try:
            # Generates a specified number of random data
            generate_random_data_as_npy(input_shape_dict[input_name],
                                        mean,
                                        std,
                                        generated_sample_size,
                                        temp_dir,
                                        trans_nhwc2nchw)
        except Exception:
            raise RuntimeError("Failed to generate the random data. "
                               "It may be due to division by zero. "
                               "Check the standard deviation setting on the GUI. "
                               "If the current setting is 0, adjust it to a non-zero value.")
        updated_nodes.append(node_info)

    # Update quantization config to store the random data directory
    if len(updated_nodes) != 0:
        quant_config_dict["nodes"] = updated_nodes

    return quant_config_dict


def glob_calibration_data_file(quant_config_dict):
    calibration_info = quant_config_dict["nodes"]
    num_calibrate_samples = quant_config_dict["num_calibrate_samples"]
    # Get file paths to be used in calibration
    glob_file_ext_pattern = r".*\.(jpg|jpeg|png|gif|bmp|tif|tiff|npy)$"
    new_sample_size = np.iinfo(np.int64).max
    updated_nodes = []
    for node_info in calibration_info:
        temp_path = Path(node_info["calibrate_sample_dir"])
        file_list = [p for p in temp_path.glob("**/*")
                     if re.search(glob_file_ext_pattern, str(p), re.IGNORECASE)]
        node_info.update({"file_list": file_list})

        # Find minimal sample size
        new_sample_size = min(new_sample_size, len(file_list))

        updated_nodes.append(node_info)

    # Update the calibration info to reflect the globbed data
    calibration_info = updated_nodes

    # Update minimal samle size
    if num_calibrate_samples != -1:
        new_sample_size = min(new_sample_size, num_calibrate_samples)

    # Pick up calibration data to align the number of samples to minimal sample size
    all_filepath_list = []
    all_input_name_list = []
    updated_nodes = []
    for node_info in calibration_info:
        new_file_list = np.random.choice(node_info["file_list"],
                                         size=new_sample_size,
                                         replace=False)
        node_info.update({"file_list": new_file_list})
        all_filepath_list.append(new_file_list)
        all_input_name_list.append(node_info["name"])
        updated_nodes.append(node_info)

    # Update the calibration info to reflect adjusted sample data size
    quant_config_dict["node"] = updated_nodes

    logger.info(f"Calibration data size: {new_sample_size}")
    if new_sample_size <= 0:
        raise RuntimeError("Failed to find files to be used in calibration. \n"
                           "Please check whether your calibration data directory contains data or "
                           "please edit the BackendCalibrationDataReader to match your data type. \n"
                           "This data reader supports the following extensions \n"
                           "- gif, jpeg, jpg, png, bmp, tif, tiff, npy")

    quant_config_dict["num_calibrate_samples"] = new_sample_size

    logger.info(f"Updated: {quant_config_dict}")
    return quant_config_dict


def read_calibration_data(mera_model, quant_config_dict, input_shape_dict, trans_nhwc2nchw):
    """Read the calibration data into memory

    Args:
        mera_model: Mera model which is the model loaded by MERA APIs
        quant_config_dict: Dictionary containing the quantization configuration
        input_shape_dict: Dictionary containing the input shape
        trans_nhwc2nchw: If True, transpose generated data from NHWC to NCHW.
                         This option only apply when input data has 4 dimentions.

    Returns:
        List of dictionary of calibration data
        e.g. 2 input model
        [{"name1": np.array, "name2": np.array}, {"name1": np.array, "name2": np.array}, ...]
    """
    calib_data = []
    num_samples = quant_config_dict["num_calibrate_samples"]
    calibration_info = quant_config_dict["nodes"]

    # looping for number of data
    for data_idx in range(num_samples):
        pair_data = {}
        # looping for name
        for node_info in calibration_info:
            input_name = node_info["name"]

            # Get the file path to be loaded
            filepath = node_info["file_list"][data_idx]
            logger.info(f"Load {filepath}...")

            # Load image data
            if filepath.suffix.lower().endswith("npy"):
                logger.warning(f"No preprocessing functions are applied to this file: {filepath}")
                img = np.load(filepath)
                pair_data[input_name] = img.astype(np.float32)

            else:

                mean = node_info["mean"]
                std = node_info["std"]

                if trans_nhwc2nchw:
                    _, c, h, w = input_shape_dict[input_name]
                else:
                    _, h, w, c = input_shape_dict[input_name]

                _validate_channel_num(mean, c, "mean")
                _validate_channel_num(std, c, "standard deviation")

                try:
                    img = preprocess_func(filepath,
                                          width=w, height=h,
                                          mean=mean, std=std,
                                          trans_nhwc2nchw=trans_nhwc2nchw)
                    pair_data[input_name] = img.astype(np.float32)
                except Exception:
                    raise RuntimeError(f"Failed to apply preprocessing to {filepath}. "
                                       "See the above error log for more details. "
                                       "Edit the `preprocess_func` of "
                                       "conversion script to match your data. "
                                       "If that does not help, please contact Renesas.")

        # Store the pair data
        calib_data.append(pair_data)

    # Validate the input name and input shape with mera_model
    sample_calib_data = calib_data[0]
    for name, input_desc in mera_model.input_desc.all_inputs.items():
        if name not in sample_calib_data:
            logger.warning(f"Cannot find name {name} in calibration data. May not be able to quantize correctly."
                           f"shape is: {input_desc.input_shape}")

    return calib_data


def preprocess_func(filepath, width, height,
                    mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225],
                    trans_nhwc2nchw=False):
    """Preprocess function to be applied at get_next() function.
        This function applies the following preprocessing.
        1. Load image
        2. Convert to RGB (Not applicable if the original image is grayscale)
        3. Resize
        4. Normalize

    Args:
        filepath: Path to load the image file
        width: width after applying resize preprocessing
        height: height after applying resize preprocessing
        mean: List containing mean value to be used in normalize preprocessing.
        std: List containing standard deviation value to be used in normalize preprocessing.
        trans_nhwc2nchw: If True, transpose generated data from NHWC to NCHW.

    Returns:
        input_data: numpy array.
    """
    pillow_img = Image.open(filepath).resize((width, height))

    if pillow_img.mode == "L":
        pillow_img = np.expand_dims(pillow_img, axis=-1)
    elif pillow_img.mode != "RGB":
        pillow_img = pillow_img.convert("RGB")

    input_data = np.float32(pillow_img)
    input_data = input_data - mean
    input_data = input_data / std
    input_data = input_data.astype(np.float32)
    input_data = np.expand_dims(input_data, axis=0)
    if trans_nhwc2nchw:
        input_data = input_data.transpose(0, 3, 1, 2)  # NHWC => NCHW
    return input_data


def generate_random_data_as_npy(size: List[int],
                                mean: List[float],
                                std: List[float],
                                num_generated_data: int,
                                output_dir: str,
                                trans_nhwc2nchw: bool) -> None:
    """Generate the random data as npy format file.

    Random data is generated using the following formula:

        x = (x-mean)/std

        x: Data with a range of 0 to 255

    Args:
        size: Generated random data size
        mean: Mean
        std: Standard deviation
        num_generated_data: Number of generated data.
        output_dir: Path to directory to stored generated data.
        trans_nhwc2nchw: If True, transpose generated data from NHWC to NCHW.

    Return:
        None
    """
    assert size[0] == 1   # The dimension of batchsize must be 1.
    size_without_batch = size[1:]

    # To match the dimensions of the generated image, increase the dimensions.
    # mean.shape = [C] => mean.shape = [1,1,...,C]
    num_expansion_dim = len(size_without_batch) - 1
    if num_expansion_dim != 0:
        dim_index = np.arange(num_expansion_dim)
        mean = np.expand_dims(np.array(mean), tuple(dim_index))
        std = np.expand_dims(np.array(std), tuple(dim_index))

    # Transpose mean/std data from HWC to CHW
    if trans_nhwc2nchw is True and len(size) == IMAGE_DIM_LENGTH:
        mean = mean.transpose()
        std = std.transpose()

    for i in range(num_generated_data):
        input_data = np.random.randint(0, 256, size=size_without_batch).astype(np.float32)
        input_data = input_data - mean
        input_data = input_data / std
        input_data = np.expand_dims(input_data, axis=0)
        np.save(Path(output_dir, f"rand{i}.npy"), input_data)


def parse_args(args):
    parser = argparse.ArgumentParser(prog="ruhmi quantize")
    parser.add_argument('--framework',
                        required=True,
                        type=str,
                        choices=["onnx", "tflite", "executorch"],
                        help='Framework of input model file. This option can be set onnx or tflite.')
    parser.add_argument('--input_model',
                        required=True,
                        type=lambda x: _validate_extension(parser,
                                                           (".onnx",
                                                            ".tflite",
                                                            ".pte"), x),
                        help='Path to input model file.')
    parser.add_argument(
        '--output_dir',
        required=True,
        type=str,
        help='Path to output directory'
    )
    parser.add_argument('--quant_config_json',
                        required=True,
                        type=str,
                        help='Json file that has the quantization settings. '
                             '''{
                                    "skip_preprocess": null,  # Unused parameter
                                    "num_calibrate_samples": -1,
                                    "calibrate_method": null,  # Unused parameter
                                    "percentile_value": null,  # Unused parameter
                                    "quant_options": "",
                                    "drp_compiler_version": null,  # Unused parameter
                                    "cpu_data_type": null,  # Unused parameter
                                    "nodes":[
                                        {
                                            "name":"input1",
                                            "calibrate_sample_dir": "tests/calib_data/calibrate_images_voc",
                                            "mean": [0],
                                            "std": [1,1,1]
                                        }
                                    ]
                                }''')
    parser.add_argument('--shape_dict_json',
                        required=True,
                        type=str,
                        help='Json files that have the input node name and its corresponding input shape. '
                             'Json contents example is '
                             '''
                             {
                                    "input1":[1,3,224,224],
                                    "input2":[1,3,416,416]
                             }
                             ''')
    args = parser.parse_args(args)
    return args


def quantize(model_path, output_dir, quant_config_dict, input_shape_dict, platform, trans_nhwc2nchw=False):
    logger.info("Running MERA quantization ...")

    # Apply the quantization
    qtz_path = Path(output_dir)
    res_path = qtz_path / 'model.mera'
    with mera.Deployer(output_dir, overwrite=False) as deployer:
        suffix = Path(model_path).suffix
        if suffix == ".onnx":
            symbolic_shape = get_onnx_symbolic_shape_mapping(model_path, input_shape_dict)
            mera_model = mera.ModelLoader(deployer).from_onnx(model_path, shape_mapping=symbolic_shape)
        elif suffix == ".tflite":
            mera_model = mera.ModelLoader(deployer).from_tflite(model_path)
        elif suffix == ".pte":
            mera_model = mera.ModelLoader(deployer).from_executorch(model_path)
        else:
            raise RuntimeError(f"Failed to load the model {model_path}. This model may be unsupported model format.")

        # Load the calibration data into memory
        calib_data = load_calibration_data(mera_model, quant_config_dict, input_shape_dict, trans_nhwc2nchw)

        qtzer_cfg = mera.quantizer.QuantizerConfigPresets.MCU
        qtzer = mera.Quantizer(
            deployer, mera_model, quantizer_config=qtzer_cfg, mera_platform=platform
        )
        try:
            model_qtz = qtzer.calibrate(calib_data).quantize()
        except Exception as e:
            raise RuntimeError(f"Error: {e}\n\n"
                               f"Failed to quantize the model: {model_path}. "
                               "See the above error log for more details. "
                               "If that does not help, please contact Renesas.")
        model_qtz.save_to(res_path)

    return res_path


def _validate_extension(parser, ext_choices, filename):
    ext = Path(filename).suffix
    if ext not in ext_choices:
        parser.error(f"Unsupported file is input:{filename}. "
                     f"This tool supports the file that has the extension {ext_choices}.")
    return filename


def _validate_args(args):
    # Check framework compatible with input model
    ext = Path(args.input_model).suffix
    if args.framework == "pytorch" and ext != ".pt":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of pytorch, `input_model` extension must be '.pt'.")
    elif args.framework == "onnx" and ext != ".onnx":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of onnx, `input_model` extension must be '.onnx'.")
    elif args.framework == "tflite" and ext != ".tflite":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of tflite, `input_model` extension must be '.tflite'.")
    elif args.framework == "executorch" and ext != ".pte":
        raise ValueError("Specified `framework` and `input_model` extension do not match. "
                         "In the case of executorch, `input_model` extension must be '.pte'.")


def _validate_channel_num(list_to_be_valid, expect_channel_num, variable_name):
    if len(list_to_be_valid) != expect_channel_num:
        raise RuntimeError(f"Number of `{variable_name}` elements is invalid. "
                           f"Number of `{variable_name}` elements must be {expect_channel_num}. "
                           f"Check number of elements set for `{variable_name}` on the GUI. ")


################################################################################
# Main function
################################################################################
def main(args):
    """Main function
    """
    # Parse arguments
    args = parse_args(args)
    logger.info(f"Arguments: {args}")
    _validate_args(args)

    # Reconfigure the encoding
    sys.stdout.reconfigure(encoding='utf-8')
    # Reconfigure the error setting for numpy
    np.seterr(divide='raise')

    # Load json configurations
    input_shape_dict = None
    with open(args.shape_dict_json) as json_file:
        input_shape_dict = json.load(json_file)
    quant_config_dict = None
    with open(args.quant_config_json) as json_file:
        quant_config_dict = json.load(json_file)

    # Define NHWC or NCHW
    trans_nhwc2nchw = False if args.framework == "tflite" else True

    platform = Platform.MCU_ETHOS
    logger.info(f"Using Device Platform: {platform}")
    mera_path = quantize(
        model_path=args.input_model,
        output_dir=args.output_dir,
        input_shape_dict=input_shape_dict,
        quant_config_dict=quant_config_dict,
        platform=platform,
        trans_nhwc2nchw=trans_nhwc2nchw
    )
    logger.info(f"SUCCESS, saved at {mera_path}")


if __name__ == "__main__":
    main(sys.argv[1:])
