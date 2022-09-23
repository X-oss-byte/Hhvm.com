# Copyright 2022-present Facebook. All Rights Reserved.

""" The main module you load in your .lldbinit. """


import lldb
import sys

import pretty
import stack


def __lldb_init_module(debugger, internal_dict):
    """ Load up the commands and pretty printers of each module.
    
    Arguments:
        debugger: Current debugger object
        internal_dict: Dict for current script session. For internal use by LLDB only.

    Returns:
        None
    """
    top = sys.modules[__name__].__name__
    pretty.__lldb_init_module(debugger, internal_dict, top)
    stack.__lldb_init_module(debugger, internal_dict, top)
