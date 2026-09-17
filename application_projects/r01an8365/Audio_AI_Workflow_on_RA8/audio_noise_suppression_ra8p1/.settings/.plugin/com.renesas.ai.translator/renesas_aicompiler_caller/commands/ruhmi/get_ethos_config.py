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
import shutil

from renesas_aicompiler_caller.utils import LogFactory

logger = LogFactory().add_console_handler() \
                     .build(__name__)


def copy_ethos_system_config(output_dir: str):
    """Copy the ethos system configuration file to specified direcotry

    Args:
        output_dir: Output directory for Ethos system configuration file

    Returns:
        None
    """
    import mera
    lib_path = Path(mera.__file__).parent

    system_config_path = lib_path / "bin_utils" / "ccodegen" / "system_config.ini"
    if not system_config_path.exists():
        raise RuntimeError(f"Failed to find the Ethos config file from RUHMI: {system_config_path}. \n"
                           f"RUHMI was installed at {lib_path}")

    dst_path = Path(output_dir)
    if not dst_path.exists():
        logger.info(f"Create the output directory: {dst_path}")
        dst_path.mkdir(parents=True, exist_ok=True)

    logger.info(f"Copy `{system_config_path}` to `{dst_path}`")
    shutil.copy2(system_config_path, dst_path)


def main(args):
    """Main function
    """
    parser = argparse.ArgumentParser(prog="ruhmi get_ethos_config")

    parser.add_argument(
        '--output_dir',
        required=True,
        type=str,
        help='output directory of Ethos system configuration file'
    )

    args = parser.parse_args(args)

    copy_ethos_system_config(args.output_dir)


if __name__ == "__main__":
    main(sys.argv[1:])
