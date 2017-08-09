#!/bin/python3
# -*- coding: utf-8 -*-
import sys
import argparse
import os
from operator import attrgetter
from friction import FrictionManager, GroupAnalyzer
from graphicalanalyzer import GraphicalAnalyzer
from PyQt5.QtWidgets import QApplication


class SortingHelpFormatter(argparse.RawTextHelpFormatter):
    """ Custom formatter for argparse help """

    def add_arguments(self, actions):
        actions = sorted(actions, key=attrgetter('option_strings'))
        super(SortingHelpFormatter, self).add_arguments(actions)

######################
#  GLOBAL FUNCTIONS  #
######################


def getArgs():
    parser = argparse.ArgumentParser(
        description=("Creates plots from data\n\n"
                     "A directory path (search_path) is specified (defaults to current) where its\n"
                     "directories are matched to a given pattern (--pattern, defaults to\n"
                     "any directory). Within these matched directories, input directory\n"
                     "(--inputpath) and output directory (--output) paths are sent to an analyzer\n"
                     "which subsequently creates the plots.\n"),
        formatter_class=SortingHelpFormatter)
    parser.add_argument('search_path', type=os.path.abspath,
                        help="Directory to search for output files to analyze.")
    parser.add_argument('--plotpath', default=os.path.abspath('./plots'),
                        type=os.path.abspath,
                        help="Path to place the plots.")
    parser.add_argument('--paramname', default='parameters.txt',
                        help="Name of the parameter file.")
    parser.add_argument('--input', default='input',
                        help='Directory to search for input files.')
    parser.add_argument('--pattern', default="*",
                        help="Pattern of directories to match.")
    parser.add_argument('--output', default='output',
                        help="Directory to search for output files.")
    args = parser.parse_args()
    return args


if __name__ == '__main__':
    args = getArgs()
    manager = FrictionManager()
    manager.setUp(args)

    app = QApplication(sys.argv)
    analyzer = GraphicalAnalyzer(manager)
    analyzer.show()
    app.exec_()
