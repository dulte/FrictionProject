from __future__ import division, absolute_import
from __future__ import print_function, unicode_literals
import numpy as np
import matplotlib.pyplot as plt
import argparse
import os
import sys
import re
from string import Formatter
from operator import attrgetter


class DataReader:
    """ Reads and converts the raw data into a manageable form """

    def __init__(self, parameters, folder=os.getcwd()):
        "Read the file and do the necessary conversions"
        self.folder = os.path.abspath(folder)
        self.params = parameters

    def read(self, filename):
        return np.fromfile(os.path.join(self.folder, filename))

    def resize(self, array, everySecondElement=False, start=0):
        if everySecondElement:
            array = array[start::2]
        return np.resize(array, (len(array) / (self.params["bottomNodes"] - 1),
                                 self.params["bottomNodes"] - 1))


class Analyzer:

    def __init__(self, folder=os.getcwd(), savepath='plots'):
        "Read all available files in the folder"
        self.folder = os.path.abspath(folder)
        sys.stdout.write("Reading parameters...")
        self.readParameters()
        sys.stdout.write("done.\n")
        self.datareader = DataReader(self.parameters, self.folder)
        self.savepath = savepath

    def readAll(self):
        self.normalForce = self.readAndResize('normal_force.bin')
        self.pusherForce = self.readAndResize('pusher_force.bin')
        self.positionInterface = self.readAndResize(
            'node_position_interface.bin')
        self.attachedSprings = self.readAndResize(
            'node_springs_attached_interface.bin')
        self.shearForce = self.readAndResize('shear_force.bin')

    def readAndResize(self, filename, start=0):
        data = self.read(filename)
        return self.datareader.resize(data, everySecondElement=True, start=start)

    def read(self, filename):
        sys.stdout.write("Reading {}...".format(filename))
        data = self.datareader.read(filename)
        sys.stdout.write("done.\n")
        return data

    def readParameters(self, filename="parameters.txt"):
        self.parameters = {}
        with open(os.path.join(self.folder, filename), 'r') as infile:
            for line in infile:
                words = line.split()
                # Convert the parameter to either int or float
                try:
                    self.parameters[words[0]] = int(words[1])
                except ValueError as valErr:
                    self.parameters[words[0]] = float(words[1])

    def plotNormalForce(self, show=False, save=True):
        plt.pcolormesh(self.normalForce)
        plt.title("Normal force")
        if show:
            plt.show()
        if save:
            plt.savefig(os.path.join(self.savepath, 'plotNormalForce.png'), dpi=1200)

    def plotPusherForce(self, show=False, save=True):
        plt.plot(self.pusherForce)
        plt.title("Force on pusher")
        if show:
            plt.show()
        if save:
            plt.savefig(os.path.join(self.savepath, 'plotPusherForce.png'), dpi=1200)

    def plotPositionInterface(self, show=False, save=True):
        pos = self.positionInterface - self.positionInterface[0, :]
        plt.pcolormesh(pos)
        plt.title("Relative position of the interface")
        if show:
            plt.show()
        if save:
            plt.savefig(os.path.join(self.savepath, 'plotPositionInterface.png'), dpi=1200)

    def plotAttachedSprings(self, show=False, save=True):
        plt.pcolormesh(self.attachedSprings)
        plt.title("Percent of attached springs")
        plt.xlabel("Block")
        plt.ylabel("Time")
        if show:
            plt.show()
        if save:
            plt.savefig(os.path.join(self.savepath, 'plotAttachedSprings.png'), dpi=1200)

    def plotShearForce(self, show=False, save=True):
        plt.pcolormesh(self.shearForce)
        plt.title("Shear force")
        if show:
            plt.show()
        if save:
            plt.savefig(os.path.join(self.savepath, 'plotShearForce.png'), dpi=1200)

    def getReducedSpringAttachments(self):
        return np.sum(self.readAndResize('node_springs_attached_interface.bin'))

    def plotFrontVelocities(self):
        data = self.getReducedSpringAttachments()
        v = np.zeros(len(data) - 2)
        h = self.parameters["dt"]*100.
        for i in range(1,len(data) -1):
            v[i-1] = (data[i+1] - data[i-1])/(2.0*h)

        del data
        return np.abs(v)*0.005*50

    def makePlots(self):
        self.normalForce = self.readAndResize('normal_force.bin')
        self.plotNormalForce()
        del self.normalForce
        self.pusherForce = self.readAndResize('pusher_force.bin')
        self.plotPusherForce()
        del self.pusherForce
        self.positionInterface = self.readAndResize(
            'node_position_interface.bin')
        self.plotPositionInterface()
        del self.positionInterface
        self.attachedSprings = self.readAndResize(
            'node_springs_attached_interface.bin')
        self.plotAttachedSprings()
        del self.attachedSprings
        self.shearForce = self.readAndResize('shear_force.bin')
        self.plotShearForce()
        del self.shearForce


class SortingHelpFormatter(argparse.RawTextHelpFormatter):
    """ Custom formatter for argparse help """

    def add_arguments(self, actions):
        actions = sorted(actions, key=attrgetter('option_strings'))
        super(SortingHelpFormatter, self).add_arguments(actions)


class Globbler:
    def __init__(self, path):
        "docstring"
        self.path = path
        self.cwd = os.getcwd()
        self.savepath = os.path.join(self.cwd, 'plots')

    def walk(self):
        namePattern = re.compile("grooveHeight\d+-grooveSize\d+")
        for root, dirs, files in os.walk(self.path, topdown=False):
            outputDirs = (os.path.join(root, name) for name in dirs
                          if 'Output' in name)
            for path in outputDirs:
                match = re.search(namePattern, path)
                if match is not None:
                    savepath = os.path.join(self.savepath, match.group(0))
                    self.makePlotDir(savepath)
                    analyzer = Analyzer(path, savepath=savepath)
                    analyzer.makePlots()

    def makePlotDir(self, path):
        if not os.path.exists(path):
            print("Making plot directory")
            os.makedirs(path)
        else:
            print("Plot directory already exists. Skipping")


######################
#  GLOBAL FUNCTIONS  #
######################


def getArgs():
    parser = argparse.ArgumentParser(description=("Creates plots from data "
                                                  "created for FrictionProject"),
                                     formatter_class=SortingHelpFormatter)
    parser.add_argument('dir', nargs='?', default="Output")
    args = parser.parse_args()
    return args


if __name__ == '__main__':
    args = getArgs()
    glob = Globbler(args.dir)
    glob.walk()
    #analyser = Analyzer(args.dir)
