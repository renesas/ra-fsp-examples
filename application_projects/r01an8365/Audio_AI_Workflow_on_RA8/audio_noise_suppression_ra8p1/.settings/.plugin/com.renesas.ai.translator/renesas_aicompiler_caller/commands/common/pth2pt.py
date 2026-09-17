# © 2023 Renesas Electronics Corporation
# RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
# This program must be used solely for the purpose for which
# it was furnished by Renesas Electronics Corporation. No part of this
# program may be reproduced or disclosed to others, in any
# form, without the prior written permission of Renesas Electronics
# Corporation.
import os
import sys
import torch
import argparse
from importlib import machinery


def is_list_or_tuple(data):
    if isinstance(data, list) or isinstance(data, tuple):
        return True
    else:
        return False


def create_dummy_data(data, dummy_data):
    if is_list_or_tuple(data) is False:
        return

    if len(data) < 1:
        return

    for elem in data:
        if is_list_or_tuple(elem):
            create_dummy_data(elem, dummy_data)
        else:
            dummy = torch.autograd.Variable(torch.randn(data))
            dummy_data.append(dummy)
            break


def main(args):

    parser = argparse.ArgumentParser()
    parser.add_argument("--input_pth")
    parser.add_argument("--input_python")
    parser.add_argument("--module_name")
    parser.add_argument("--input_shape")
    parser.add_argument("--gpu")

    args = parser.parse_args(args)
    pth_file = args.input_pth
    py_file = args.input_python
    output_pt_file = str.replace(args.input_pth, ".pth", ".pt")
    module_name = args.module_name
    if hasattr(args, 'gpu') and args.gpu is not None:
        gpu = int(args.gpu)
    else:
        gpu = -1

    dummy = []
    if hasattr(args, "input_shape") and args.input_shape is not None:
        shape = eval(args.input_shape)
        create_dummy_data(shape, dummy)

    if gpu >= 0:
        dev = "cuda:" + str(gpu)
    else:
        dev = "cpu"

    if not (os.path.exists(pth_file) and os.path.exists(py_file)):
        print('Error : .pth or .py file is not found')
        sys.exit(1)

    base, extension = os.path.splitext(pth_file)
    if extension == '.pth':
        loader = machinery.SourceFileLoader(module_name, py_file)
        module = loader.load_module()
        network = getattr(module, module_name)
        model = network()
        network = torch.load(pth_file, map_location=torch.device(dev))
        if 'net' in network:
            network = network['net']
        traced_script_module = torch.jit.trace(model, dummy)
        traced_script_module.save(output_pt_file)
        print("success convert to pt")


if __name__ == '__main__':
    main(sys.argv[1:])
    sys.exit(0)
