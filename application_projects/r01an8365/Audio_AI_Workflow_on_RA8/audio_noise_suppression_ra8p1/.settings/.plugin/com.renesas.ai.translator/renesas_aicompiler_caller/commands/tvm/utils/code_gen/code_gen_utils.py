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
import json
import re
from enum import Enum
from renesas_aicompiler_caller.utils import LogFactory
from renesas_aicompiler_caller.commands.tvm.utils import rel2abspath

logger = LogFactory().add_console_handler() \
                     .build(__name__)


class Writer:
    """Writer class to define basic function.

    Args:
        filename: Path to output file.
    """
    def __init__(self, filename):
        self._tab = 4   # number of tab space
        self._encoding = "UTF-8"

        self._filename = os.path.abspath(filename)
        self._file = open(self._filename, "w", encoding=self._encoding)

    def __del__(self):
        if not self.closed:
            logger.warning(f"Found unclosed file: {self._filename}")
            self.close()

    def close(self):
        return self._file.close()

    def write(self, s, num_tab=0, end="\n"):
        file = self._file
        s = "{}{}{}".format(" " * num_tab * self._tab, s, end)
        rv = file.write(s)
        return rv

    def writelines(self, iterable, num_tab=0, end="\n"):
        file = self._file
        iterable_w_space = []
        for s in iterable:
            s = "{}{}{}".format(" " * num_tab * self._tab, s, end)
            iterable_w_space.append(s)
        assert len(iterable) == len(iterable_w_space)
        rv = file.writelines(iterable_w_space)
        return rv

    @property
    def closed(self):
        if self._file is None:
            return True
        return self._file.closed


class WriterState(Enum):
    WRITE = 0
    IGNORE = 1


class CodeGenMode(Enum):
    USE_PREPRO_RUNTIME = 0
    DONT_USE_PREPRO_RUNTIME = 1
    MIXED_USE_PREPRO_RUNTIME = 2


def get_code_gen_mode(preprocess_configs):
    """The value of preprocess config determines the type of sample code to output.
       Therefore, this funcion returns CodeGenMode depending on preprocess configs.

    Args:
        preprocess_configs: List containing preprocess configuration.

    Returns:
        code_gen_mode: Value for code gen mode to output sample code

    Notes:
        - When all use_prepro_runtime properties are True in preprocess configs, \
          this function returns USE_PREPRO_RUNTIME
        - When all use_prepro_runtime properties are False in preprocess configs, \
          this funciton returns DONT_USE_PREPRO_RUNTIME
        - Otherwise, this function returns MIXED_USE_PREPRO_RUNTIME
    """
    exists_true = False
    exists_false = False
    for preprocess_config in preprocess_configs:
        if preprocess_config["use_prepro_runtime"].lower() == "true":
            exists_true = True
        elif preprocess_config["use_prepro_runtime"].lower() == "false":
            exists_false = True

    if exists_true is True and exists_false is False:
        return CodeGenMode.USE_PREPRO_RUNTIME
    elif exists_true is False and exists_false is True:
        return CodeGenMode.DONT_USE_PREPRO_RUNTIME
    elif exists_true is True and exists_false is True:
        return CodeGenMode.MIXED_USE_PREPRO_RUNTIME
    else:
        raise NotImplementedError("[Internal error] Contact to Renesas with the following error message. \n"
                                  "Failed to get code gen mode. It may cause preprocess configs are not properly. \n"
                                  "In particular, the `use_prepro_runtime' properties are incorrect.")


def get_input_order_from_depjson(drpai_tvm_model_obj_dir, num_inputs):
    """Get input order from deploy json

    Args:
        drpai_tvm_model_obj_dir: drpai_tvm_model_obj_dir: Path to DRP-AI TVM Model Object directory.
        num_inputs: Number of inputs of model

    Returns:
         input_order: List containing the input node name. input_order[0] means a first input node.
    """

    drpai_tvm_model_obj_dir = rel2abspath(drpai_tvm_model_obj_dir)

    deploy_json = os.path.join(drpai_tvm_model_obj_dir, "deploy.json")
    # Load deploy.json as dict format
    with open(deploy_json) as json_file:
        deploy_dict = json.load(json_file)
    nodes = deploy_dict["nodes"]

    # Get input order
    input_order = []
    for i in range(num_inputs):
        input_order.append(nodes[i]["name"])

    return input_order


def reorder_preprocess_configs(right_input_order, preprocess_configs):
    """Reorder the preprocess settings by following `right_input_order`.

    Args:
        right_input_order: List containing input node names in order according to deploy.json
        preprocess_configs: List containing preprocess configs

    Returns:
        reordered_preprocess_configs: List containing reordered preprocess configs.
    """
    # Make a dictionary to index preprocess configs by input node name.
    preprocess_configs_map = {}
    for preprocess_conf in preprocess_configs:
        node_name = preprocess_conf["name"]
        preprocess_configs_map[node_name] = preprocess_conf

    # Reorder the preprocess configs
    reordered_preprocess_configs = []
    for input_node_name in right_input_order:
        if input_node_name not in preprocess_configs_map.keys():
            raise RuntimeError("[Internal error] Contact to Renesas with the following error message. \n"
                               f"Failed to parse preprocess config for `{input_node_name}`. "
                               f"The name property of preprocess configs must have `{input_node_name}`. "
                               f"But there are no exist in preprocess configs: {preprocess_configs_map.keys()}")
        reordered_preprocess_configs.append(preprocess_configs_map[input_node_name])

    logger.info("================ [Preprocess settings] ==================")
    logger.info(f"Before reordering: {preprocess_configs}")
    logger.info(f"After  reordering: {reordered_preprocess_configs}")

    if not len(reordered_preprocess_configs) == len(preprocess_configs):
        raise RuntimeError("[Internal error] Contact to Renesas with the following error message. \n"
                           "Failed to reorder the preprocess configs. "
                           f"DRP-AI TVM model has the following input node order. {right_input_order} "
                           "Please check `preprocess_config_json` settings")

    return reordered_preprocess_configs


def reorder_preprocess_obj_dirs(right_input_order, reordered_preprocess_configs, preprocess_obj_dirs):
    """Reorder the preprocess object directory by following `reordered_preprocess_configs`.

    Args:
        right_input_order: List containing input node names in order according to deploy.json
        reordered_preprocess_configs: List containing reoreded preprocess configs
        preprocess_obj_dirs: List containing path to preprocess object directory

    Returns:
        reordered_preprocess_obj_dirs: List containing reordered path to preprocess object directory.
    """

    # Reorder the preprocess_obj_dirs
    reordered_preprocess_obj_dirs = []
    tmp_preprocess_obj_dirs = []  # For error message to notify right order to user
    for preprocess_conf in reordered_preprocess_configs:
        output_dir = preprocess_conf["output_dir_path"]
        logger.info(f"Reorder for output_dir: {output_dir}")
        tmp_preprocess_obj_dirs.append(output_dir)
        # Assumption: If preprocess_obj_dirs has None, it means CodeGenMode.MIXED_USE_PREPRO_RUNTIME.
        match_regrex = False
        # Append preprocess_obj_dir if match output_dir and preprocess_obj_dir
        for preprocess_obj_dir in preprocess_obj_dirs:
            if preprocess_obj_dir is None:
                continue
            abs_dir_path = os.path.abspath(preprocess_obj_dir)
            if re.match(r'.*{}[/]*'.format(output_dir), abs_dir_path):
                match_regrex = True
                reordered_preprocess_obj_dirs.append(abs_dir_path)
        # When matching no one, append None
        if match_regrex is False:
            reordered_preprocess_obj_dirs.append(None)

    logger.info("================ [Preprocess directory] ==================")
    logger.info(f"Before reordering: {preprocess_obj_dirs}")
    logger.info(f"After  reordering: {reordered_preprocess_obj_dirs}")

    if not len(reordered_preprocess_obj_dirs) == len(preprocess_obj_dirs):
        raise RuntimeError("[Internal error] Contact to Renesas with the following error message. \n"
                           "Failed to reorder the preprocess object directory. "
                           f"DRP-AI TVM model has the following input node order. {right_input_order} "
                           "Please set `preprocess_obj_dirs` to the correct order "
                           "by following DRP-AI TVM model input order. "
                           f"e.g. {tmp_preprocess_obj_dirs}. ")

    return reordered_preprocess_obj_dirs


def reorder_pre_settings(right_input_order, preprocess_obj_dirs, preprocess_configs):
    """Reorder the preprocess settings by following `right_input_order`.

    Args:
        right_input_order: List containing input node names in order according to deploy.json
        preprocess_obj_dirs: List containing path to preprocess object directory
        preprocess_configs: List containing preprocess configs

    Returns:
        reordered_preprocess_obj_dirs: List containing reordered path to preprocess object directory.
        reordered_preprocess_configs: List containing reordered preprocess configs.
    """

    reordered_preprocess_configs = reorder_preprocess_configs(right_input_order,
                                                              preprocess_configs)

    reordered_preprocess_obj_dirs = reorder_preprocess_obj_dirs(right_input_order,
                                                                reordered_preprocess_configs,
                                                                preprocess_obj_dirs)

    return reordered_preprocess_obj_dirs, reordered_preprocess_configs
