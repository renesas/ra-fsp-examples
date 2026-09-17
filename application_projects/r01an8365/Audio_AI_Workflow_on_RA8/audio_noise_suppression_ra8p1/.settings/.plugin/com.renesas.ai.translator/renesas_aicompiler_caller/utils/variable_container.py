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
from renesas_aicompiler_caller.utils import LogFactory

logger = LogFactory().add_console_handler() \
                     .build(__name__)


class VariableContainer(object):
    """Container for storing variable that is used for DRP-AI TVM

    """

    def __new__(cls, *args, **kargs):
        """This class is designed by Singleton pattern to keep a variable consistency.

        Note:
            This class is not thread safe. Don't use this class in multi-thread context.
        """
        if not hasattr(cls, "_instance"):
            cls._instance = super(VariableContainer, cls).__new__(cls)
        return cls._instance

    def __init__(self):
        if getattr(self, '_initialized', False):
            return
        self._initialized = True
        self.variables = {}

    def register(self, name, value):
        """Add a variable to the collection of variables

        Args:
            name: Variable name to be added into the container.
            value: Variable value to be added into the container.

        Returns:
            None
        """
        if name in self.variables.keys():
            logger.warn("The same name was already registered. But it was overwriiten.\n"
                        f"Previous value  : {self.variables[name]}\n"
                        f"Rewritten value : {value}\n")
        self.variables[name] = value

    def unregister(self, name):
        """Remove a variable from the container.

        Args:
            name: Variable name to be deleted

        Returns:
            None
        """
        if name not in self.variables:
            logger.warn(f"Failed to find a `{name}` variable")
            return
        del self.variables[name]

    def get(self, name):
        """Get a variable value from the container

        Args:
            name: Variable name to be got

        Returns:
            value: If None, failed to get variable value because name key is not found in the container.
                   Otherwise, returns a variable value which is stored in the container.
        """
        if name not in self.variables:
            raise RuntimeError(f"Failed to find a `{name}` variable. "
                               "The variable container doesn't have `{name}` variable.")
        return self.variables[name]

    def show_container(self):
        # Check that whether self.variables is empty
        if len(self.variables) == 0:
            print("Variable container does not have anything...")
            return

        # Count number of variable name's character.
        variable_names = self.variables.keys()
        max_len = len(max(variable_names, key=lambda name: len(name)))

        # Print
        print("="*(max_len+1))
        for variable_name, value in self.variables.items():
            print(f'{variable_name:{max_len+1}s} | {value} ')
