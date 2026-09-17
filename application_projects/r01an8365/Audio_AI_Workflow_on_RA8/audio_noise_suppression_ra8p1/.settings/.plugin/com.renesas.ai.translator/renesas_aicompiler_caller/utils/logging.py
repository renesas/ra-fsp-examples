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
from __future__ import annotations
import datetime
from typing import Optional
from logging import getLogger, handlers, Handler, StreamHandler, Formatter, DEBUG


class LogFactory:
    """Log factory to create logger

    Note:
        You can create a logger with
        >>> logger = LogFactory().build(__name__)
        To capture the log data and pass it to handlers for console, you can also create a logger with
        >>> logger = LogFactory().add_console_handler() \
                                 .add_file_handler() \
                                 .build(__name__)
    """
    DEFAULT_FORMATTER = Formatter('%(asctime)s [%(levelname)s] (%(filename)s #%(lineno)d, %(funcName)s): %(message)s')

    def __init__(self, root_level: int = DEBUG):
        self._root = getLogger(None)
        self._root.setLevel(root_level)
        self._root.propagate = False

    def _create_console_handler(self, level: int = DEBUG) -> StreamHandler:
        handler = StreamHandler()
        handler.setLevel(level)
        handler.setFormatter(self.DEFAULT_FORMATTER)
        return handler

    def _create_file_handler(self, filename: str, level: int = DEBUG) -> handlers.RotatingFileHandler:
        filename = filename if filename else f'{datetime.now().strftime("%Y-%d-%m %H.%M.%S")}.log'
        handler = handlers.RotatingFileHandler(filename, maxBytes=100000, backupCount=1)
        handler.setLevel(level)
        handler.setFormatter(self.DEFAULT_FORMATTER)
        return handler

    def _has_handler(self, handler_type: Handler):
        if self._root.hasHandlers():
            for registered_handler in self._root.handlers:
                if isinstance(registered_handler, handler_type):
                    return True
        return False

    def add_console_handler(self, level: int = DEBUG) -> LogFactory:
        if self._has_handler(StreamHandler):
            return self
        self._root.addHandler(self._create_console_handler(level))
        return self

    def add_file_handler(self, filename: Optional[str] = None, level: int = DEBUG) -> LogFactory:
        if self._has_handler(handlers.RotatingFileHandler):
            return self
        self._root.addHandler(self._create_file_handler(filename, level))
        return self

    def build(self, name: str):
        logger = getLogger(name)
        return logger
