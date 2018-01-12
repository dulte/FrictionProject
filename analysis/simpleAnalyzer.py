import matplotlib.pyplot as plt
from matplotlib import cm,colors
import numpy as np
import seaborn as sns
import sys
import argparse
import os
from operator import attrgetter
from friction import FrictionManager

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
    parser.add_argument('--paramname', default='parameters',
                        help="Name of the parameter file.")
    parser.add_argument('--input', default='input',
                        help='Directory to search for input files.')
    parser.add_argument('--pattern', default="*",
                        help="Pattern of directories to match.")
    parser.add_argument('--output', default='output',
                        help="Directory to search for output files.")
    args = parser.parse_args()
    return args


class Analyzer:
    def __init__(self,manager):
        self.manager = manager

    def getNormalForceForHeight(self,height):
        lattices = self.manager.lattice

        for l in lattices:
            if l.grooveHeight == grooveHeight:
                pass


    def plotAllForce(self,data,forceType,height=None,size=None):
        lattices = self.manager.lattice

        heights = [l.height for l in lattices]
        sizes = [l.size for l in lattices]

        if size == None and height != None:
            parameters = sizes[:]
            cmLabel = "Size"
            titleText = forceType + " Force at Max for Height = {}".format(height)
        elif size != None and height == None:
            parameters = heights[:]
            cmLabel = "Height"
            titleText = forceType + " Force at Max for Size = {}".format(size)
        if size == None and height == None:
            print("Please Give Either a Size or a Height")
            return

        norm = colors.Normalize(
                            vmin=np.min(parameters),
                            vmax=np.max(parameters))

        c_m = cm.coolwarm
        s_m = cm.ScalarMappable(cmap=c_m, norm=norm)
        s_m.set_array([])


        for i in range(len(heights)):
            if size == None and height != None:
                if heights[i] == height:
                    plt.plot(data[i], label=height,color=s_m.to_rgba(parameters[i]))
            elif size != None and height == None:
                if sizes[i] == size:
                    plt.plot(data[i], label=size,color=s_m.to_rgba(parameters[i]))
            elif height != None and size != None:
                if sizes[i] == size and heights[i] == height:
                    plt.plot(data[i], label=(size,height))






        if size == None or height == None:
            cbar = plt.colorbar(s_m)
            cbar.set_label(cmLabel, fontsize=18)

        plt.title(titleText, fontsize=18)



    def plotAllNormalForce(self,height=None,size=None):
        self.plotAllForce(self.manager.getNormalForceAtMax(),"Normal",height,size)

        plt.xlabel("Block", fontsize=18)
        plt.ylabel("Normal Force [N]", fontsize=18)
        plt.show()

    def plotAllShearForce(self,height=None,size=None):
        self.plotAllForce(self.manager.getShearForceAtMax(),"Shear",height,size)

        plt.xlabel("Block", fontsize=18)
        plt.ylabel("Normal Force [N]", fontsize=18)
        plt.show()


if __name__ == '__main__':
    args = getArgs()
    manager = FrictionManager()
    manager.setUp(args)

    a = Analyzer(manager)
    a.plotAllNormalForce(height=4)
    a.plotAllNormalForce(height=7)
    a.plotAllNormalForce(size=11)
    a.plotAllNormalForce(size=14)
