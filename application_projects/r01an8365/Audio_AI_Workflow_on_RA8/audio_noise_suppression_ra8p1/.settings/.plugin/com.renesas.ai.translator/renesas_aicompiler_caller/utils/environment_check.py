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
import sys
from pathlib import Path
import shutil
from typing import List, Dict
from distutils.util import strtobool
import hashlib
import platform
from renesas_aicompiler_caller.utils import LogFactory
from .variable_container import VariableContainer

logger = LogFactory().add_console_handler() \
                     .build(__name__)
vars = VariableContainer()

# The following values are calculated by using sha256sum with libtvm_runtime.so.
# These values are used to identify the DRP-AI TVM version.
SUPPORTED_DRPAI_TVM_VERSION = {
    "V2L": {
        "78650d4aef45e6669ae7b055e9e321c3a94fdf846bcfbd988790199ab0a8e806": "v1.1.1",
        "bee5e39a19b0fc92431e41b94b8e63cc7f2645ffda8606d08c72eb97bf35219b": "v2.3.0"
    },
    "V2H": {
        "9d1b5b6162313a30dbe48314b6019887bd705b057618ce9c9ade21919c2e2458": "v2.1.0",
        "6b6f4eec223399a7d6a3e5dd6158a213fc1b2b5a8c2fcaf97df4f694250ef958": "v2.2.1",
        "06e57df61bbb1831c3827f14e67696635bf585ea6a1fcae85984e8e402e4a851": "v2.3.0"
    },
    "V2N": {
        "98d1a5186ab160faafb150157885ef2fa88657f57e3113da42d2f8681c8272ed": "v2.5.1"
    }
}

SUPPORTED_MERA_VERSION = [
    {
        "supported_device": ["RA8P1"],  # Supported device
        "python_version": "3.10",
        "python_lib_dependencies": {
            "mera": "2.3",
            "ethos-u-vela": "4.2.0",
            "tflite": "2.18.0",
            "onnx": "1.17.0",
            "typing_extensions": "4.5.0"
        }
    }
]

# The following values record the paths to CMake-related files used when generating sample code.
# These values are used in code_gen/copy_files_factory.py
SRC_CMAKE_FILES = {
    "V2L": {  # Device
        "v1.1.1": [  # DRP-AI TVM version
            "template_apps/drpai_tvm_v111/rzv2l/CMakeLists.txt",
            "template_apps/drpai_tvm_v111/rzv2l/toolchain/runtime.cmake"
        ],
        "v2.3.0": [
            "template_apps/drpai_tvm_v230/rzv2l/CMakeLists.txt",
            "template_apps/drpai_tvm_v230/rzv2l/toolchain/runtime.cmake"
        ]
    },
    "V2H": {
        "v2.1.0": [
            "template_apps/drpai_tvm_v210/rzv2h/CMakeLists.txt",
            "template_apps/drpai_tvm_v210/rzv2h/toolchain/runtime.cmake"
        ],
        "v2.2.1": [
            "template_apps/drpai_tvm_v210/rzv2h/CMakeLists.txt",
            "template_apps/drpai_tvm_v210/rzv2h/toolchain/runtime.cmake"
        ],
        "v2.3.0": [
            "template_apps/drpai_tvm_v210/rzv2h/CMakeLists.txt",
            "template_apps/drpai_tvm_v210/rzv2h/toolchain/runtime.cmake"
        ]
    },
    "V2N": {
        "v2.5.1": [
            "template_apps/drpai_tvm_v210/rzv2h/CMakeLists.txt",
            "template_apps/drpai_tvm_v210/rzv2h/toolchain/runtime.cmake"
        ]
    }
}


def _check_tvm_version():
    """Check DRP-AI TVM version whether it is supported version.
    """
    # Create build runtime path
    build_runtime_path = "{}/obj/build_runtime/{}/libtvm_runtime.so".format(vars.get("TVM_ROOT"), vars.get("PRODUCT"))

    # Check if there is build runtime
    if not os.path.isfile(build_runtime_path):
        raise RuntimeError(f"No such file: {build_runtime_path}")

    # Find sha256sum for build_runtime
    with open(f"{build_runtime_path}", "rb") as f:
        build_runtime_data = f.read()
        hash_sha256 = hashlib.sha256(build_runtime_data).hexdigest()
        logger.info(f"hash_sha256 of {build_runtime_path}: {hash_sha256}")

        # Verify if DRP-AI TVM is supported
        exp_hash_sha256_dict = SUPPORTED_DRPAI_TVM_VERSION[vars.get("PRODUCT")]
        if hash_sha256 not in exp_hash_sha256_dict.keys():
            version_list = list(exp_hash_sha256_dict.values())
            raise RuntimeError("Unsupported DRP-AI TVM Version. "
                               f"This tool supports DRP-AI TVM ({version_list}) for {vars.get('PRODUCT')}")

        DRPAI_TVM_VERSION = exp_hash_sha256_dict[hash_sha256]
        vars.register("DRPAI_TVM_VERSION", exp_hash_sha256_dict[hash_sha256])

    logger.info(f"PRODUCT: {vars.get('PRODUCT')}")
    logger.info(f"DRP-AI TVM Version: {DRPAI_TVM_VERSION}")


def with_trailing_slash(path: Path) -> str:
    """Normalize the given path to end with "/".
    """
    path = path.resolve()
    s = str(path)
    return s if s.endswith('/') else s + '/'


def _check_python_version():
    """Check Python version whether it is supported version.
    """
    import sys
    # Check python version
    target_metadata = None
    python_version = f"{sys.version_info.major}.{sys.version_info.minor}"
    supported_python_version = []
    for metadata in SUPPORTED_MERA_VERSION:
        supported_python_version.append(metadata["python_version"])
        if python_version == metadata["python_version"]:
            target_metadata = metadata
            break

    if target_metadata is None:
        raise RuntimeError(f'This tool does not work with Python {sys.version}. '
                           f'Install the Python {supported_python_version}. '
                           'To check the default python version, run the following command.'
                           '> py --version \n'
                           'Python 3.10.X')
    return target_metadata


def exec_cmd(cmd: List[str]):
    import subprocess
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)

    # Output build log
    while proc.poll() is None:
        line = proc.stdout.readline().decode("utf-8")
        logger.info(line.removesuffix("\n"))

    return proc.returncode


def _check_python_package_and_version(package_name, package_version=None):
    import importlib
    import importlib.metadata
    from packaging import version

    installed_version = None
    try:
        installed_version = importlib.metadata.version(package_name)
    except Exception:
        raise RuntimeError(f"'{package_name}' is not installed. Install '{package_name}'. "
                           f"e.g. {sys.executable} -m pip install {package_name}")

    if installed_version is None:
        logger.info(f"'{package_name}' is installed, but version is unknown.")
        return

    logger.info(f"'{package_name}' version {installed_version} is installed.")
    if package_version and version.parse(installed_version) != version.parse(package_version):
        logger.warning(f"'{package_name}' version is {installed_version}. This tool was tested with {package_version}")


def _check_python_deps_for_windows(target_metadata: Dict):
    """Check python version and dependecies for Windows
    """
    import importlib.util
    if importlib.util.find_spec("mera") is None:
        raise RuntimeError("Install mera by using the following command:\n"
                           f"{sys.executable} -m pip install mera-*-.whl")

    import mera
    logger.info(f"{mera.get_mera_version()}")

    # Check python dependencies
    python_deps = target_metadata["python_lib_dependencies"]
    for package_name, package_version in python_deps.items():
        _check_python_package_and_version(package_name=package_name, package_version=None)


def _check_environment_variable_for_linux():
    """Check environment variable.
    """
    PRODUCT = os.getenv("PRODUCT", None)
    if PRODUCT is None:
        raise ValueError("No environment variable(PRODUCT). "
                         "Please set environment variable(PRODUCT) to your product name. "
                         "e.g. $export PRODUCT=V2L")

    if PRODUCT not in SUPPORTED_DRPAI_TVM_VERSION.keys():
        raise ValueError("Unsupported product is used. "
                         f"This tool supports {SUPPORTED_DRPAI_TVM_VERSION.keys()}.")
    vars.register("PRODUCT", PRODUCT)

    TVM_ROOT = os.getenv("TVM_ROOT", None)
    if TVM_ROOT is None:
        raise ValueError("No environment variable(TVM_ROOT). "
                         "Please set environment variable(TVM_ROOT) to your DRP-AI TVM root directory.")
    vars.register("TVM_ROOT", os.path.abspath(TVM_ROOT))

    # The following block is for sample code generation function.
    DBG_CODE_GEN_SNIPPET = strtobool(os.getenv("DBG_CODE_GEN_SNIPPET", "False"))
    vars.register("DBG_CODE_GEN_SNIPPET", DBG_CODE_GEN_SNIPPET)

    # If set `DEBUG_DRPAI_TVM_PLUG_ENG`, print debug information.
    DRPAI_TVM_PLUG_ENG_DEBUG = strtobool(os.getenv("DRPAI_TVM_PLUG_ENG_DEBUG", "False"))
    vars.register("DRPAI_TVM_PLUG_ENG_DEBUG", DRPAI_TVM_PLUG_ENG_DEBUG)

    # Set the compilation tool path to PATH
    if shutil.which("run_quantization_onnx.sh") is None:
        os.environ["PATH"] = str(Path("/drp-ai_tvm/tutorials")) + os.pathsep + os.environ["PATH"]

    SDK = os.getenv("SDK", None)
    if SDK is None:
        logger.info("SDK environment variable was not found. "
                    "SDK is parsed and obtained.")
        sdk_dirs = list(Path("/opt").rglob("sysroots"))
        assert len(sdk_dirs) == 1
        SDK = str(sdk_dirs[0].parent.resolve())
        os.environ["SDK"] = SDK
    vars.register("SDK", os.path.abspath(SDK))

    TRANSLATOR = os.getenv("TRANSLATOR", None)
    if TRANSLATOR is None:
        raise ValueError("No environment variable(TRANSLATOR). "
                         "Please set environment variable(TRANSLATOR) to your DRP-AI Translator root directory.")
    TRANSLATOR = with_trailing_slash(Path(TRANSLATOR))
    vars.register("TRANSLATOR", TRANSLATOR)

    QUANTIZER = os.getenv("QUANTIZER", None)
    if QUANTIZER is not None:
        QUANTIZER = with_trailing_slash(Path(QUANTIZER))
    vars.register("QUANTIZER", QUANTIZER)


def _check_environment_variable_for_windows():
    """Check environment variable for windows.
    """
    # Check whether vela command is enable
    if shutil.which("vela") is None:
        scripts_path = str(Path(sys.executable).parent / "Scripts")
        os.environ["PATH"] = scripts_path + os.pathsep + os.environ["PATH"]
        logger.info("The `vela` command was not found. Adding the following path to PATH environment variable: \n"
                    f"{scripts_path}")
        assert shutil.which("vela"), "Failed to find the vela command. " \
                                     "Set the vela command path to PATH environment variable"

    # Remove the environment variable to get AMD64 from platform.processor()
    PROCESSOR_IDENTIFIER = os.getenv("PROCESSOR_IDENTIFIER", None)
    if PROCESSOR_IDENTIFIER is not None:
        del os.environ["PROCESSOR_IDENTIFIER"]


def exec_environment_check():
    logger.info("Execute environment check ...")
    if platform.system() == "Windows":
        metadata = _check_python_version()
        _check_python_deps_for_windows(metadata)
        _check_environment_variable_for_windows()
    elif platform.system() == "Linux":
        _check_environment_variable_for_linux()
        _check_tvm_version()
    else:
        raise RuntimeError(f"Unsupported platform was detected: {platform.system()}"
                           "This tool only supports Windows/Linux")
    vars.show_container()
