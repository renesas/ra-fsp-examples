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
import shutil
import json
from contextlib import contextmanager
import mera
from mera import Target, Platform
from renesas_aicompiler_caller.utils import LogFactory
from .utils import get_onnx_symbolic_shape_mapping

logger = LogFactory().add_console_handler() \
                     .build(__name__)


def parse_args(args):
    parser = argparse.ArgumentParser(prog="ruhmi deploy")
    parser.add_argument('--framework',
                        required=True,
                        type=str,
                        choices=["onnx", "tflite", "executorch", "quantized"],
                        help='Framework of input model file. This option can be set onnx or tflite.')
    parser.add_argument('--input_model',
                        required=True,
                        type=lambda x: _validate_extension(parser,
                                                           (".onnx",
                                                            ".tflite",
                                                            ".pte",
                                                            ".mera"), x),
                        help='Path to input model file.')
    parser.add_argument(
        '--output_dir',
        required=True,
        type=str,
        help='Path to output directory'
    )
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
    parser.add_argument('--optimization_mode',
                        type=int,
                        default=0,
                        choices=[0, 1],
                        help='Optimization mode at compile. "0" means "performance" which is the default and '
                             'compiles the model for maximum performance. '
                             'If "1" is set to this option, optimized for minimal RAM usage. '
                             'This means "size" options in vela.')
    parser.add_argument('--memory_mode',
                        type=int,
                        default=0,
                        choices=[0, 1],
                        help='Specify the inference behaviour. If "1" (Shared_Sram) was specified, '
                             'DMA transfer will be performed before executing inference. '
                             '"0" means Sram_Only mode for vela.')
    parser.add_argument('--use_only_cpu',
                        action='store_true',
                        help='Use only cpu. When specify this option, converted model will run on cpu, not Ethos.')
    parser.add_argument('--weight_loc',
                        type=int,
                        default=0,
                        choices=[0, 1],
                        help='Specify the ram / rom location for the weights. '
                             'This option only work on when specifying --use_only_cpu'
                             '"0" means that the weights are located in ROM. '
                             '"1" means that the weights are located in RAM.'
                        )
    parser.add_argument('--ethos_configuration_filepath',
                        type=str,
                        default=None,
                        help="Path to Ethos configuration file to be used during deployment")
    parser.add_argument('--mera_options',
                        type=str,
                        default="",
                        help='Additional options for Mera \n'
                             '--target [mcu/simulator] \n'
                             '--enable_ospi [true/false] \n'
                             '--enable_ref_data [true/false] \n'
                             '--suffix [string] \n'
                             '--use_x86 [true/false]')
    parser.add_argument('--vela_options',
                        type=str,
                        default="",
                        help='Additional options for vela \n'
                             '--sys_config [string like RA8P1] \n'
                             '--accel_config [string like ethos-u55-256] \n'
                             '--verbose_all [true/false]')
    args = parser.parse_args(args)

    return args


def parse_mera_options(mera_options: str):
    import shlex
    argv = mera_options
    # Split the string
    # e.g. "--device sakura2" => "--device" "sakura2"
    if mera_options:
        argv = shlex.split(mera_options)

    parser = argparse.ArgumentParser(description="Mera option parser")
    parser.add_argument('--target',
                        type=str,
                        default="mcu",
                        choices=["mcu", "simulator"],
                        help='Additional options for mera to run the model on x86 architecture')
    parser.add_argument('--enable_ospi',
                        default=False,
                        type=lambda x: x.lower() == "true",
                        help="Enable to use ospi memory")
    parser.add_argument('--enable_ref_data',
                        default=True,
                        type=lambda x: x.lower() == "true",
                        help="Enable to output the reference data")
    parser.add_argument('--suffix',
                        default='',
                        type=str,
                        help="Add the suffix to the output source code when converting for CPU")
    parser.add_argument('--use_x86',
                        default=False,
                        type=lambda x: x.lower() == "true",
                        help="If true, output x86 related source code (i.e. python bindings)")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="Unparsed arguments")
    args = parser.parse_args(argv)

    return args


def parse_vela_options(vela_options: str):
    import shlex
    argv = vela_options
    # Split the string
    # e.g. "--device sakura2" => "--device" "sakura2"
    if vela_options:
        argv = shlex.split(vela_options)

    parser = argparse.ArgumentParser(description="Vela option parser")
    parser.add_argument('--sys_config',
                        type=str,
                        default="RA8P1",
                        help='Selects the system configuration to use as specified in the Vela configuration file')
    parser.add_argument('--accel_config',
                        type=str,
                        default="ethos-u55-256",
                        help='Choose which hardware accelerator configuration to compile for.')
    parser.add_argument('--verbose_all',
                        default=False,
                        type=lambda x: x.lower() == "true",
                        help="Enable all --verbose-* options for Vela.")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="Unparsed arguments")
    args = parser.parse_args(argv)

    return args


def make_config(args, mera_options, vela_options):
    """Make mcu_config and vela_config to throw to MERA's API.

    Args:
        args: Result for main function parser
        mera_options: Result for mera options parser
        vela_options: Result for vela options parser

    Returns:
        vela_config: Dictionary containing the parameters for Vela
        mcu_config: Dictionary containing the parameters for CPU Mera
    """
    PARAM_MAPPING = {
        "memory_mode": ("Sram_Only", "Shared_Sram"),
        "optimise": ("Performance", "Size"),
        "weight_location": ("flash", "iram")
    }

    # ARM Vela options
    # only effective if Platform.MCU_ETHOS is selected, ignored otherwise
    vela_config = {}
    vela_config['enable_ospi'] = mera_options.enable_ospi
    vela_config['sys_config'] = vela_options.sys_config
    vela_config['memory_mode'] = PARAM_MAPPING["memory_mode"][args.memory_mode]
    vela_config['accel_config'] = vela_options.accel_config
    vela_config['optimise'] = PARAM_MAPPING["optimise"][args.optimization_mode]
    vela_config['verbose_all'] = vela_options.verbose_all

    # MCU C code generation options
    mcu_config = {}
    mcu_config['suffix'] = mera_options.suffix
    mcu_config['weight_location'] = PARAM_MAPPING["weight_location"][args.weight_loc]

    # On other scripts we could use True and it will not generate
    # x86 related source code (python bindings for example)
    mcu_config['use_x86'] = mera_options.use_x86

    return vela_config, mcu_config


def _validate_extension(parser, ext_choices, filename):
    ext = Path(filename).suffix
    if ext not in ext_choices:
        parser.error(f"Unsupported file is input:{filename}. "
                     f"This tool supports the file that has the extension {ext_choices}.")
    return filename


def _validate_args(args, mera_options, vela_options):
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

    # Check use_only_cpu option
    if args.use_only_cpu:
        if args.weight_loc is None:
            raise ValueError("Weight location is not specified. "
                             "Must specify `weight_loc` option when specifying `use_only_cpu` option.")
    else:
        # Check input model extension
        ext = Path(args.input_model).suffix
        if ext == ".onnx":
            raise ValueError("Invalid model format (.onnx) was specified. \n"
                             "Quantization is mandatory for ONNX models. Run quantization before converting.")
        elif ext == ".pte":
            raise ValueError("Invalid model format (.pte) was specified. \n"
                             "Quantization is mandatory for Executorch models. Run quantization before converting.")

    # Check enable_ref_data option
    ext = Path(args.input_model).suffix
    if mera_options.enable_ref_data and ext != ".tflite":
        logger.warning("enable_ref_data can only be specified when using .tflite models. "
                       "Therefore forcing enable_ref_data to False.")
        mera_options.enable_ref_data = False


@contextmanager
def temporarily_replace_file(target: Path, temp_file: Path = None):
    backup = target.with_suffix(target.suffix + ".bak")

    if temp_file is None:
        yield
        return

    try:
        # Back up the original file
        shutil.copy2(target, backup)
        shutil.copy2(temp_file, target)
        yield
    finally:
        # Replace with the original file
        if backup.exists():
            shutil.copy2(backup, target)
            backup.unlink()


def compile_mera(model_path, out_dir, platform, target,
                 input_shape_dict, vela_config, mcu_config, enable_ref_data=True):
    # ----- setup params
    host_arch = "x86"

    # ----- instantiate the model and save
    logger.info("\nDeploying MERA model ...")
    with mera.Deployer(out_dir, overwrite=True) as deployer:
        suffix = Path(model_path).suffix
        if suffix == ".onnx":
            symbolic_shape = get_onnx_symbolic_shape_mapping(model_path, input_shape_dict)
            model = mera.ModelLoader(deployer).from_onnx(model_path, shape_mapping=symbolic_shape)
        elif suffix == ".tflite":
            model = mera.ModelLoader(deployer).from_tflite(model_path)
        elif suffix == ".pte":
            model = mera.ModelLoader(deployer).from_executorch(model_path)
        elif suffix == ".mera":
            model = mera.ModelLoader(deployer).from_quantized_mera(model_path)
        else:
            raise RuntimeError(f"Unsupported model format was input: {suffix}")

        try:
            deployer.deploy(
                model,
                mera_platform=platform,
                target=target,
                host_arch=host_arch,
                vela_config=vela_config,
                mcu_config=mcu_config,
                enable_ref_data=enable_ref_data,
            )
        except Exception as e:
            raise RuntimeError(f"Error: {e}\n\n"
                               f"Failed to deploy the model: {model_path}. "
                               "See the above error log for more details. "
                               "If that does not help, please contact Renesas.")

    return out_dir


def main(args):
    """Main function
    """
    # Parse arguments
    args = parse_args(args)
    mera_options = parse_mera_options(args.mera_options)
    vela_options = parse_vela_options(args.vela_options)
    logger.info(f"Arguments: {args}")
    logger.info(f"Mera options: {mera_options}")
    logger.info(f"Vela options: {vela_options}")
    _validate_args(args, mera_options, vela_options)
    vela_config, mcu_config = make_config(args, mera_options, vela_options)

    # Reconfigure the encoding
    sys.stdout.reconfigure(encoding='utf-8')

    # Load json configurations
    input_shape_dict = None
    with open(args.shape_dict_json) as json_file:
        input_shape_dict = json.load(json_file)

    # Compile model
    platform = Platform.MCU_CPU if args.use_only_cpu else Platform.MCU_ETHOS
    target = None
    if mera_options.target == "mcu":
        target = Target.MCU
    elif mera_options.target == "simulator":
        target = Target.MERAInterpreter
    else:
        raise RuntimeError(f"Unsupported target {mera_options.target} was specified. "
                           "This script only supports mcu or simulator.")
    model_path = args.input_model
    output_dir = args.output_dir
    logger.info(f"Targeting {target} on {platform} ")

    # Get the system config file
    lib_path = Path(mera.__file__).parent
    system_config_path = lib_path / "bin_utils" / "ccodegen" / "system_config.ini"

    with temporarily_replace_file(system_config_path, args.ethos_configuration_filepath):
        mera_path = compile_mera(
            model_path,
            out_dir=output_dir,
            platform=platform,
            target=target,
            input_shape_dict=input_shape_dict,
            vela_config=vela_config,
            mcu_config=mcu_config,
            enable_ref_data=mera_options.enable_ref_data
        )
        logger.info(f"SUCCESS, saved at {mera_path}")


if __name__ == "__main__":
    main(sys.argv[1:])
