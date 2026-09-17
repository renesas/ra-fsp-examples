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
import re
from typing import List
import renesas_aicompiler_caller
from renesas_aicompiler_caller.utils import LogFactory
from renesas_aicompiler_caller.commands.tvm.utils import rel2abspath
from .code_gen_utils import Writer, WriterState, CodeGenMode
from ._registry import register_code_generator

__all__ = []

logger = LogFactory().add_console_handler() \
                     .build(__name__)

mod_path = os.path.dirname(os.path.abspath(renesas_aicompiler_caller.__file__))

if os.getenv("DBG_CODE_GEN_SNIPPET", None) is None:
    DBG_CODE_GEN_SNIPPET = False
else:
    DBG_CODE_GEN_SNIPPET = True

BYTE_ALIGN = 64


@register_code_generator
class V2LSampleAppCppWriterForV111(Writer):
    """Writer to output sample code depending on DRP-AI TVM Model Object.

    Args:
        filename: Path to output file.
        drpai_tvm_model_obj_dir: Path to DRP-AI TVM Model Object directory.
        preprocess_obj_dirs: List containing path to preprocess object directory.
        preprocess_configs: List containing preprocess configuration.
        code_gen_mode: Value for code gen mode to output sample code
        template_file: Path to template file that is written sample application. e.g. /path/to/sample_app.cpp
        drpai_tvm_version: DRP-AI TVM version

    Notes1:
        After instantiation of this class, please call the following function.
        ```
        a = SampleAppCppWriter(xxx)
        a.write_code()
        ```

    Note2:
        This class assumes preprocess_obj_dirs and preprocess_configs are aligned in input order defined in deploy.json
    """
    def __init__(self,
                 filename: str,
                 drpai_tvm_model_obj_dir: str,
                 preprocess_obj_dirs: List[str],
                 preprocess_configs: List[str],
                 code_gen_mode: CodeGenMode,
                 template_file: str = None,
                 drpai_tvm_version: str = "v1.1.1"):
        super().__init__(filename)
        default_template_file = f"{mod_path}/template_apps/drpai_tvm_v111/rzv2l/sample_app.cpp"
        self._templates = default_template_file if template_file is None else template_file
        self._drpai_tvm_model_obj_dir = rel2abspath(drpai_tvm_model_obj_dir)
        self._preprocess_obj_dirs = rel2abspath(preprocess_obj_dirs)
        self._preprocess_configs = preprocess_configs
        self._code_gen_mode = code_gen_mode
        self._version = drpai_tvm_version

        # Import template file
        self._contents = self._import_template_file(self._templates)

        self._state = WriterState.WRITE

        if self._code_gen_mode == CodeGenMode.USE_PREPRO_RUNTIME or \
           self._code_gen_mode == CodeGenMode.MIXED_USE_PREPRO_RUNTIME:
            # Parse the preprocess object size
            _preprocess_obj_sizes = self._parse_preprocess_obj_size(self._preprocess_obj_dirs)
            self._preprocess_obj_offsets = self._calc_preprocess_obj_offset(_preprocess_obj_sizes)
            self._drpai_tvm_model_obj_offset = self._calc_drpai_tvm_model_obj_offset(_preprocess_obj_sizes)
        elif self._code_gen_mode == CodeGenMode.DONT_USE_PREPRO_RUNTIME:
            self._drpai_tvm_model_obj_offset = self._calc_drpai_tvm_model_obj_offset((0,))

    def _read_addrmap_intm(self, filename: str):
        """Read addrmap_intm.txt

        Args:
            filename: Path to addrmap_intm.txt

        Returns:
            addrmap: Dictionary containing start address, size and element name.
                     Key is start address. Value are size and element name.
                     i.e. Start address -> [size, element name]
        """
        addrmap = {}
        with open(filename, "r") as f:
            for line in f:
                line = line.strip()
                element, addr, size = line.split()
                addr = int(addr, 16)
                size = int(size, 16)
                addrmap[addr] = [size, element]
        return addrmap

    def _parse_preprocess_obj_size(self, preprocess_obj_dirs: List[str]):
        """Parse preprocess object size from pp_addrmap_intm.txt

        Args:
            preprocess_obj_dirs: List containing path to preprocess object directory.

        Returns:
            preprocess_obj_size: List containing the preprocess object size
        """

        preprocess_obj_size = []
        for preprocess_obj_dir in preprocess_obj_dirs:
            if preprocess_obj_dir is None:
                preprocess_obj_size.append(0)
                continue
            abs_preprocess_obj_dir = os.path.abspath(preprocess_obj_dir)
            file = os.path.join(abs_preprocess_obj_dir, "pp_addrmap_intm.txt")

            addrmap = self._read_addrmap_intm(file)
            max_addr = max(list(addrmap.keys()))
            size = addrmap[max_addr][0]
            total_size = max_addr + size

            preprocess_obj_size.append(total_size)
        return preprocess_obj_size

    def _calc_preprocess_obj_offset(self, preprocess_obj_sizes: List[int]):
        """Calculate preprocess object offset address

        Args:
            preprocess_obj_sizes: Lists containing the preprocess object size

        Returns:
            offset_list: List containing the start address to deploy preprocess object.
        """
        offset_list = [0]
        offset_addr = 0
        for preprocess_obj_size in preprocess_obj_sizes[:-1]:
            if preprocess_obj_size == 0:
                offset_list.append(0)
                continue
            offset_addr += preprocess_obj_size

            # Align by 64 byte
            offset = offset_addr % BYTE_ALIGN
            offset_addr = (offset_addr - offset) + BYTE_ALIGN

            offset_list.append(offset_addr)

        for i, offset in enumerate(offset_list):
            logger.info(f"offset_addr for preprocess {i}: 0x{offset:x}")
        return offset_list

    def _calc_drpai_tvm_model_obj_offset(self, preprocess_obj_sizes: List[int]):
        """Calculate drpai tvm model object offset address

        Args:
            preprocess_obj_sizes: Lists containing the preprocess object size

        Returns:
            offset_addr: Address to deploy drpai tvm model object.
        """
        offset_addr = 0
        for preprocess_obj_size in preprocess_obj_sizes:
            offset_addr += preprocess_obj_size

            # Align by 64 byte
            offset = offset_addr % BYTE_ALIGN
            offset_addr = (offset_addr - offset) + BYTE_ALIGN

        logger.info(f"offset_addr for DRP-AI TVM Model Obj: 0x{offset_addr:x}")
        return offset_addr

    def _import_template_file(self, template_file: str):
        """Import template file to this class

        Args:
            template_file: Path to import template file. e.g. /path/to/sample_app.cpp

        Returns:
            contents: List containing template file strings.
        """
        try:
            logger.info(f"Read {template_file}...")
            with open(template_file, "r") as f:
                contents = f.read()
                return contents.splitlines()
        except Exception as e:
            raise RuntimeError(f"Failed to open {template_file}"
                               f"Error details: {e}")

    def _write_macro(self):
        """Write macro into sample_app.cpp

        Args:
            None

        Returns:
            None
        """

        model_in_num = len(self._preprocess_configs)

        # Make macro
        snippet = []
        snippet.append("/*Input image info*/")
        for i, config in enumerate(self._preprocess_configs):
            if config["use_prepro_runtime"].lower() == "true":
                _, H, W, C = config["input_data_format"]["shape_in"]
            else:
                if config["output_data_format"]["order_out"] == "HWC":
                    _, H, W, C = config["output_data_format"]["shape_out"]
                elif config["output_data_format"]["order_out"] == "CHW":
                    _, C, H, W = config["output_data_format"]["shape_out"]
            snippet.append(f"#define INPUT_IMAGE{i}_H    ({H})")
            snippet.append(f"#define INPUT_IMAGE{i}_W    ({W})")
            snippet.append(f"#define INPUT_IMAGE{i}_C    ({C})")

        snippet.append("/*Model input info*/")
        snippet.append(f"#define MODEL_IN_NUM    ({model_in_num})")
        for i, config in enumerate(self._preprocess_configs):
            if config["output_data_format"]["order_out"] == "HWC":
                _, H, W, C = config["output_data_format"]["shape_out"]
            elif config["output_data_format"]["order_out"] == "CHW":
                _, C, H, W = config["output_data_format"]["shape_out"]
            else:
                raise NotImplementedError()
            snippet.append(f"#define MODEL_IN{i}_H    ({H})")
            snippet.append(f"#define MODEL_IN{i}_W    ({W})")
            snippet.append(f"#define MODEL_IN{i}_C    ({C})")

        snippet.append("#define TVM_MODEL_OBJ_OFFSET "
                       f"(0x{self._drpai_tvm_model_obj_offset:x})")

        # Write macro codes
        self.writelines(snippet)

    def _write_global_var(self):
        """Write global variables

        Args:
            None

        Returns:
            None
        """
        if self._code_gen_mode == CodeGenMode.USE_PREPRO_RUNTIME:
            self._write_global_var_for_use_prepro_runtime()
        elif self._code_gen_mode == CodeGenMode.DONT_USE_PREPRO_RUNTIME:
            self._write_global_var_for_dont_use_prepro_runtime()
        elif self._code_gen_mode == CodeGenMode.MIXED_USE_PREPRO_RUNTIME:
            self._write_global_var_for_mixed_use_prepro_runtime()
        else:
            raise NotImplementedError("[Internal error] Contact to Renesas with the following error message. \n"
                                      "Failed to write the global variable. \n"
                                      "Specified code gen mode is invalid: \n"
                                      f"self._code_gen_mode = {self._code_gen_mode}")

    def _write_global_var_for_use_prepro_runtime(self):
        # Make macro
        snippet = []

        snippet.append("/* Model Binary */")
        abs_drpai_tvm_model_obj_dir = os.path.abspath(self._drpai_tvm_model_obj_dir)
        drpai_tvm_model_obj_dirname = os.path.basename(abs_drpai_tvm_model_obj_dir)
        snippet.append(f'const static std::string model_dir = "{drpai_tvm_model_obj_dirname}";')

        snippet.append("/* Pre-processing Runtime Object */")
        pre_dir = ""
        for preprocess_obj_dir in self._preprocess_obj_dirs:
            abs_preprocess_obj_dir = os.path.abspath(preprocess_obj_dir)
            preprocess_obj_dir_dirname = os.path.basename(abs_preprocess_obj_dir)
            pre_dir += f'model_dir + "/{preprocess_obj_dir_dirname}", '

        snippet.append('const static std::vector<std::string> pre_dir = { ' +
                       pre_dir + ' };')

        pre_addr_start = ""
        for offset in self._preprocess_obj_offsets:
            pre_addr_start += f"0x{offset:x}, "
        snippet.append('const static std::vector<uint32_t> pre_start_addr = { ' +
                       pre_addr_start + ' };')

        snippet.append('/* Image buffer */')
        snippet.append('static unsigned char * img_buffer[MODEL_IN_NUM];')
        model_in_num = len(self._preprocess_configs)
        img_sizes = ""
        for i in range(model_in_num):
            img_sizes += f"INPUT_IMAGE{i}_H * INPUT_IMAGE{i}_W * INPUT_IMAGE{i}_C, "

        snippet.append(
"""
const static std::vector<uint32_t> img_sizes = {{
    {}
}};""".format(img_sizes))  # noqa

        # Define in_param
        in_param = ""
        for i, config in enumerate(self._preprocess_configs):
            format_in = "FORMAT_{}".format(config["input_data_format"]["format_in"])
            format_out = "FORMAT_{}".format(config["output_data_format"]["format_out"])
            in_param += \
"""
    {{
        .pre_in_shape_w = INPUT_IMAGE{}_W,
        .pre_in_shape_h = INPUT_IMAGE{}_H,
        .pre_in_format  = {},
        .pre_out_format = {},
    }},""".format(i, i, format_in, format_out)  # noqa

        snippet.append(
"""
static std::vector<s_preproc_param_t> in_param = {{
    {}
}};""".format(in_param))  # noqa

        # Write global variables
        self.writelines(snippet)

    def _write_global_var_for_dont_use_prepro_runtime(self):
        # Make macro
        snippet = []

        snippet.append("/* Model Binary */")
        abs_drpai_tvm_model_obj_dir = os.path.abspath(self._drpai_tvm_model_obj_dir)
        drpai_tvm_model_obj_dirname = os.path.basename(abs_drpai_tvm_model_obj_dir)
        snippet.append(f'const static std::string model_dir = "{drpai_tvm_model_obj_dirname}";')

        snippet.append('/* Image buffer */')
        snippet.append('static unsigned char * img_buffer[MODEL_IN_NUM];')
        model_in_num = len(self._preprocess_configs)
        img_sizes = ""
        for i in range(model_in_num):
            img_sizes += f"INPUT_IMAGE{i}_H * INPUT_IMAGE{i}_W * INPUT_IMAGE{i}_C, "

        snippet.append(
"""
const static std::vector<uint32_t> img_sizes = {{
    {}
}};""".format(img_sizes))  # noqa

        # Write global variables
        self.writelines(snippet)

    def _write_global_var_for_mixed_use_prepro_runtime(self):
        # Make macro
        snippet = []

        snippet.append("/* Model Binary */")
        abs_drpai_tvm_model_obj_dir = os.path.abspath(self._drpai_tvm_model_obj_dir)
        drpai_tvm_model_obj_dirname = os.path.basename(abs_drpai_tvm_model_obj_dir)
        snippet.append(f'const static std::string model_dir = "{drpai_tvm_model_obj_dirname}";')

        snippet.append("/* Pre-processing Runtime Object */")
        pre_dir = ""
        for preprocess_obj_dir in self._preprocess_obj_dirs:
            if preprocess_obj_dir is None:
                pre_dir += '"", '
            else:
                abs_preprocess_obj_dir = os.path.abspath(preprocess_obj_dir)
                preprocess_obj_dir_dirname = os.path.basename(abs_preprocess_obj_dir)
                pre_dir += f'model_dir + "/{preprocess_obj_dir_dirname}", '

        snippet.append('const static std::vector<std::string> pre_dir = { ' +
                       pre_dir + ' };')

        pre_addr_start = ""
        for offset in self._preprocess_obj_offsets:
            pre_addr_start += f"0x{offset:x}, "
        snippet.append('const static std::vector<uint32_t> pre_start_addr = { ' +
                       pre_addr_start + ' };')

        snippet.append('/* Image buffer */')
        snippet.append('static unsigned char * img_buffer[MODEL_IN_NUM];')
        model_in_num = len(self._preprocess_configs)
        img_sizes = ""
        for i in range(model_in_num):
            img_sizes += f"INPUT_IMAGE{i}_H * INPUT_IMAGE{i}_W * INPUT_IMAGE{i}_C, "

        snippet.append(
"""
const static std::vector<uint32_t> img_sizes = {{
    {}
}};""".format(img_sizes))  # noqa

        # Define in_param
        in_param = ""
        for i, config in enumerate(self._preprocess_configs):
            if config["use_prepro_runtime"].lower() == "false":
                in_param += \
"""
    {
        .pre_in_shape_w = NULL,
        .pre_in_shape_h = NULL,
        .pre_in_format  = NULL,
        .pre_out_format = NULL,
    },"""  # noqa

            else:
                format_in = "FORMAT_{}".format(config["input_data_format"]["format_in"])
                format_out = "FORMAT_{}".format(config["output_data_format"]["format_out"])
                in_param += \
"""
    {{
        .pre_in_shape_w = INPUT_IMAGE{}_W,
        .pre_in_shape_h = INPUT_IMAGE{}_H,
        .pre_in_format  = {},
        .pre_out_format = {},
    }},""".format(i, i, format_in, format_out)  # noqa

        snippet.append(
"""
static std::vector<s_preproc_param_t> in_param = {{
    {}
}};""".format(in_param))  # noqa

        # Write global variables
        self.writelines(snippet)

    def _write_debug_code(self):
        logger.info("Write code with enabling `DBG_CODE_GEN_SNIPPET`...")
        for line in self._contents:
            self.write(line)

    def _is_ifdef_line_with_genmode(self, raw_str: str):
        """Check whether input string is ifdef statement that has CODE_GEN_MODE_*

        Args:
            raw_str: String that determines if it should be wriiten in the code.

        Returns:
            bool: When True, input string is ifdef statement that has CODE_GEN_MODE_*.
        """

        if self._code_gen_mode == CodeGenMode.USE_PREPRO_RUNTIME:
            if re.fullmatch(r".*defined\(CODE_GEN_MODE_USE_PREPRO_RUNTIME\).*", raw_str):
                self._state = WriterState.WRITE
                return True
            elif re.fullmatch(r".*defined\(CODE_GEN_MODE_DONT_USE_PREPRO_RUNTIME\).*"
                              r"|.*defined\(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME\).*", raw_str):
                self._state = WriterState.IGNORE
                return True
        elif self._code_gen_mode == CodeGenMode.DONT_USE_PREPRO_RUNTIME:
            if re.fullmatch(r".*defined\(CODE_GEN_MODE_DONT_USE_PREPRO_RUNTIME\).*", raw_str):
                self._state = WriterState.WRITE
                return True
            elif re.fullmatch(r".*defined\(CODE_GEN_MODE_USE_PREPRO_RUNTIME\).*"
                              r"|.*defined\(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME\).*", raw_str):
                self._state = WriterState.IGNORE
                return True
        elif self._code_gen_mode == CodeGenMode.MIXED_USE_PREPRO_RUNTIME:
            if re.fullmatch(r".*defined\(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME\).*", raw_str):
                self._state = WriterState.WRITE
                return True
            elif re.fullmatch(r".*defined\(CODE_GEN_MODE_USE_PREPRO_RUNTIME\).*"
                              r"|.*defined\(CODE_GEN_MODE_DONT_USE_PREPRO_RUNTIME\).*", raw_str):
                self._state = WriterState.IGNORE
                return True

        return False

    def _should_write_string(self, raw_str: str):
        """Check whether input string should be written to code.

        Args:
            raw_str: String that determines if it should be wriiten in the code.

        Returns:
            bool: When True, input string should be written to code.
        """

        # When #ifdef statements are found, ignore this block of code.
        # Never output #ifdef statements, therefore this function return false if found it.
        if re.fullmatch(r"#define DBG_CODE_GEN_SNIPPET", raw_str):
            return False
        elif re.fullmatch(r"#ifdef DBG_CODE_GEN_SNIPPET", raw_str):
            self._state = WriterState.IGNORE
            return False
        elif re.fullmatch(r"#else", raw_str):
            self._state = WriterState.WRITE
            return False
        elif re.fullmatch(r"#endif \/.*", raw_str):
            self._state = WriterState.WRITE
            return False

        if self._is_ifdef_line_with_genmode(raw_str):
            return False

        # Skip writing code when _state is WriterState.IGNORE.
        if self._state == WriterState.IGNORE:
            return False

        return True

    def write_code(self):
        """Write sample code

        Args:
            None

        Returns:
            None
        """
        if DBG_CODE_GEN_SNIPPET:
            self._write_debug_code()
            return

        for line in self._contents:
            # Remove return code(\n)
            raw_str = line.strip()
            if not self._should_write_string(raw_str):
                continue

            # Write the sample code
            if re.fullmatch(r"//<\\WRITE_VERSION/>", raw_str):
                snippet = f"* Version      : {self._version}"
                self.write(snippet)
            elif re.fullmatch(r"//<\\WRITE_MACRO/>", raw_str):
                self._write_macro()
            elif re.fullmatch(r"//<\\WRITE_GLOBAL_VAR/>", raw_str):
                self._write_global_var()
            else:
                self.write(line)


@register_code_generator
class V2LSampleAppCppWriterForV230(Writer):
    """Writer to output sample code depending on DRP-AI TVM Model Object.

    Args:
        filename: Path to output file.
        drpai_tvm_model_obj_dir: Path to DRP-AI TVM Model Object directory.
        preprocess_obj_dirs: List containing path to preprocess object directory.
        preprocess_configs: List containing preprocess configuration.
        code_gen_mode: Value for code gen mode to output sample code
        template_file: Path to template file that is written sample application. e.g. /path/to/sample_app.cpp
        drpai_tvm_version: DRP-AI TVM version

    Notes1:
        After instantiation of this class, please call the following function.
        ```
        a = SampleAppCppWriter(xxx)
        a.write_code()
        ```

    Note2:
        This class assumes preprocess_obj_dirs and preprocess_configs are aligned in input order defined in deploy.json
    """
    def __init__(self,
                 filename: str,
                 drpai_tvm_model_obj_dir: str,
                 preprocess_obj_dirs: List[str],
                 preprocess_configs: List[str],
                 code_gen_mode: CodeGenMode,
                 template_file: str = None,
                 drpai_tvm_version: str = "v2.3.0"):
        default_template_file = f"{mod_path}/template_apps/drpai_tvm_v230/rzv2l/sample_app.cpp"
        template_file = default_template_file if template_file is None else template_file
        self._writer = V2LSampleAppCppWriterForV111(filename,
                                                    drpai_tvm_model_obj_dir,
                                                    preprocess_obj_dirs,
                                                    preprocess_configs,
                                                    code_gen_mode,
                                                    template_file=template_file,
                                                    drpai_tvm_version=drpai_tvm_version)

    def write_code(self):
        self._writer.write_code()

    def close(self):
        self._writer.close()
