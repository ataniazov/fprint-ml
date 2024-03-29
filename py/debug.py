#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Module Docstring
"""

__author__ = "Ata Niyazov"
__version__ = "0.1.0"
__license__ = "MIT"

# import modules used here
import argparse

# Gather our code in a main() function


def main(args):
    """ Main entry point of the app """
    print args


# Standard boilerplate to call the main() function to begin
# the program.
if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser()

    # Required positional argument
    #parser.add_argument("args", help="Required positional argument")

    # Required dataset path
    parser.add_argument("data path", help="Required dataset path")

    # Dataset
    parser.add_argument('data size', metavar='D', type=int, help='dataset amount')    

    # Train
    parser.add_argument('train size', metavar='T', type=int, help='train amount')

    # Class file
    parser.add_argument("-c", "--class", action="store", help="class file")

    # Optional argument flag which defaults to False
    #parser.add_argument("-f", "--flag", action="store_true", default=False)

    # Optional argument which requires a parameter (eg. -d test)
    #parser.add_argument("-n", "--name", action="store", dest="name")

    # Optional verbosity counter (eg. -v, -vv, -vvv, etc.)
    # parser.add_argument(
    #     "-v",
    #     "--verbose",
    #     action="count",
    #     default=0,
    #     help="Verbosity (-v, -vv, etc)")

    # Specify output of "--version"
    parser.add_argument(
        "--version",
        action="version",
        version="%(prog)s (version {version})".format(version=__version__))

    args = parser.parse_args()
    main(args)
