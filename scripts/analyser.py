#!/bin/python3
# -*- coding: utf-8 -*-

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
from mpl_toolkits.mplot3d import Axes3D


class DataReader:
    """ Reads and converts the raw data into a manageable form """

    def __init__(self, parameters, folder=os.getcwd()):
        "Read the file and do the necessary conversions"
        self.folder = os.path.abspath(folder)
        self.params = parameters

    def read(self, filename):
        return np.fromfile(os.path.join(self.folder, filename))

    """
    Original "self.params['bottomNodes'] - 1", removed '-1' to get rid of an error
    """

    def resize(self, array, everySecondElement=False, start=1):
        # if everySecondElement:
        #     array = np.copy(array[start::2])
        # return np.reshape(array, ((len(array) / (self.params["bottomNodes"])),
        #                          (self.params["bottomNodes"])))
        arrayList = []
        for i in range(int(len(array) / (self.params["bottomNodes"]))):
            arrayList.append(array[i*self.params['bottomNodes']:(i+1)*self.params['bottomNodes']])
        return np.array(arrayList)

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

    """
    Added input/ to the filename, due to the set up at abel
    """

    def readParameters(self, filename="input/parameters.txt",xyzFilename = "input/lattice.xyz"):#input/lattice.txt"):
        self.parameters = {}
        with open(os.path.join(self.folder, filename), 'r') as infile:
            for line in infile:
                if line[0] != "#" and line != "\n":
                    words = line.split()
                    # Convert the parameter to either int or float
                    # TODO: Just use eval?
                    try:
                        self.parameters[words[0]] = eval(words[1])
                    except:
                        self.parameters[words[0]] = str(words[1])

                    # try:
                    #     self.parameters[words[0]] = int(words[1])
                    # except ValueError as valErr:
                    #     self.parameters[words[0]] = float(words[1])

        with open(os.path.join(self.folder, xyzFilename), 'r') as infile:
            numBottomBlocks = 0
            for line in infile:
                if line[0] == 'B':
                    numBottomBlocks += 1

            self.parameters['bottomNodes'] = numBottomBlocks



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


    """
    Method for quickly getting a time array for a spesific type of data. freqName is the name found in the parameters.txt file corresponding to the given data
    """
    def getTimeArray(self,data,freqName):
        return np.arange(len(data))*self.parameters['step']*self.parameters[freqName]

    """
    If the number of springs attached is less then the cutoffPoint (percentage) times the total number of springs belonging to that node,
    the node is treated as dislodged, else it is treated as attached.
    """
    def getReducedSpringAttachments(self,cutoffPoint):
        return np.sum(np.where(self.readAndResize('node_springs_attached_interface.bin') <= self.parameters['ns']*cutoffPoint,0,1),axis = 1)

    """
    Plots the number of attached blocks. Show desides if the plot will be shown, or used in an other figure.
    """
    def plotReducedSpringAttachments(self, cutoffPoint = 0.1, show = True):
        data = self.getReducedSpringAttachments(cutoffPoint)
        time = getTimeArray(data,'freqNodeSpringsAttachedInterface')
        plt.plot(time,data)
        if show:
            plt.title("Number of Nodes Attached to the Ground")
            plt.xlabel("Time [s]")
            plt.ylabel("Number")
            plt.show()


    """
    Finds the front velocity of the rapture of the springs. 'h' is the time step used for integration, and is the between each measurment dt*freq.
    A symmetric differentiation is used to find the velocity.
    """
    def getFrontVelocities(self, cutoffPoint = 0.1):
        data = self.getReducedSpringAttachments(cutoffPoint)
        v = np.zeros(len(data) - 2)
        h = self.parameters["step"]*self.parameters['freqNodeSpringsAttachedInterface']
        for i in range(1,len(data) -1):
            v[i-1] = (data[i+1] - data[i-1])/(2.0*h)

        del data
        return np.abs(v)*self.parameters['d']

    """
    Plots the Front Velocities.
    """
    def plotFrontVelocities(self,cutoffPoint = 0.1):
        data = self.getFrontVelocities(cutoffPoint)
        time = np.arange(len(data))*self.parameters['step']*self.parameters['freqNodeSpringsAttachedInterface']
        plt.plot(time,data)
        plt.title("Front Velocity for size %g and height %g"%(self.getGrooveDim()[1],self.getGrooveDim()[0]))
        plt.xlabel("Time [s]")
        plt.ylabel("Velocity [m/s]")
        plt.show()


    """
    Reads the shear forces on the top rod. The array is then resized to a matrix with the shape [time,block]. If the parameter mean == True,
    mean forces is calculated, else the total force is calculated. The static friction coefficent is returned as the maximum value of the mean or total shear force
    on the rod divided by the normal force.

    TODO: Get the friction force from the flat case, and use that to normalize the static friction for grooves.
    """
    def getStaticFrictionCoefficient(self, mean = False):
        shearForce_on_rod = self.read('rodShearForce.bin')
        shearForce_on_rod = np.reshape(shearForce_on_rod, len(shearForce_on_rod)/self.parameters['nx'],self.parameters['nx'])
        if mean:
            shearForce_on_rod = np.mean(shearForce_on_rod,axis = 1)
        else:
            shearForce_on_rod = np.sum(shearForce_on_rod,axis = 1)

        staticCoefficiant = np.max(shearForce_on_rod)/np.float(self.parameters['fn'])
        del shearForce_on_rod
        return staticCoefficiant

    """
    Returns a time serier of the shear force on the rod. Much of the processing is the same as for the method above.
    """

    def getRodShearForceTimeSeries(self,mean = False):
        shearForce_on_rod = self.read('rodShearForce.bin')
        shearForce_on_rod = np.reshape(shearForce_on_rod, len(shearForce_on_rod)/self.parameters['nx'],self.parameters['nx'])
        if mean:
            shearForce_on_rod = np.mean(shearForce_on_rod,axis = 1)
        else:
            shearForce_on_rod = np.sum(shearForce_on_rod,axis = 1)

        return shearForce_on_rod/np.float(self.parameters['fn'])

    def plotRodShearForceTimeSeries(self,mean = False):
        data = self.getRodShearForceTimeSeries(mean)
        time = np.arange(data)*self.parameters['step']*self.parameters['freq...']
        plt.plot(time,data)
        plt.title("Shear force of rod for size %g and height %g"%(self.getGrooveDim()[1],self.getGrooveDim[0]))
        plt.xlabel("Time [s]")
        if mean:
            plt.ylabel(r"Mean $F_T/F_N$")
        else:
            plt.ylabel(r"Total $F_T/F_N$")
        plt.show()






    # def getStaticFrictionCoefficient(self):
    #     self.pusherForce = self.read('pusher_force.bin')
    #     staticCoefficiant = np.max(self.pusherForce)
    #     del self.pusherForce
    #     return staticCoefficiant

    def getGrooveDim(self):
        return self.parameters["grooveHeight"], self.parameters["grooveSize"]

    def makePlots(self):
        self.normalForce = self.readAndResize('normal_force.bin')
        self.plotNormalForce()
        del self.normalForce
        self.pusherForce = self.read('pusher_force.bin')
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

    def walk(self, pattern, className):
        namePattern = re.compile(pattern)
        instanceList = []
        for root, dirs, files in os.walk(self.path, topdown=False):
            outputDirs = (os.path.join(root, name) for name in dirs
                          if 'Output' in name)
            for path in outputDirs:
                match = re.search(namePattern, path)
                if match is not None:
                    savepath = os.path.join(self.savepath, match.group(0))
                    self.makePlotDir(savepath)
                    analyzer = className(path, savepath = savepath)#Analyzer(path, savepath=savepath)
                    instanceList.append(analyzer)

        return instanceList

    def makePlotDir(self, path):
        if not os.path.exists(path):
            print("Making plot directory")
            os.makedirs(path)
        else:
            print("Plot directory already exists. Skipping")



class Compare:
    def __init__(self,instanceList):
        self.instanceList = instanceList

    def makeStaticCoeffArray(self):
        self.staticCoefficiantArray = np.zeros([len(self.instanceList),3])
        for i in range(len(self.instanceList)):
            self.staticCoefficiantArray[i,0] = self.instanceList[i].getStaticFrictionCoefficient()
            self.staticCoefficiantArray[i,1] = self.instanceList[i].getGrooveDim()[0]
            self.staticCoefficiantArray[i,2] = self.instanceList[i].getGrooveDim()[1]

    def plotCoeffHeight(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()

        plt.plot(self.staticCoefficiantArray[:,1],self.staticCoefficiantArray[:,0])
        plt.show()

    def plotCoeffSize(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()

        plt.plot(self.staticCoefficiantArray[:,2],self.staticCoefficiantArray[:,0])
        plt.show()

    def plotCoeff3D(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        ax.scatter(self.staticCoefficiantArray[:,2],self.staticCoefficiantArray[:,1],self.staticCoefficiantArray[:,0])
        ax.set_xlabel("Size")
        ax.set_ylabel("Height")
        ax.set_zlabel(r"$\mu_s$")

        plt.show()

    def printStaticCoeffArray(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()
        print(self.staticCoefficiantArray)

    def plotPushers(self):

        for i in self.instanceList:
            plt.plot(i.read('pusher_force.bin'), label= "size: %g, height: %g"%(i.getGrooveDim()[1],i.getGrooveDim()[0]))

        plt.legend()
        plt.show()

    """
    Plots the different times series for the shear force on the rod. As of now, this does nothing. Change freq... to the right name for the freq of the shear force of the rod.
    """
    def plotRodShearForceAgainsRodVelocity(self):

        for i in self.instanceList:
            timeSeries = i.getRodShearForceTimeSeries
            time = np.arange(0,len(timeSeries))*i.parameters['dt']*i.parameters['freq...']
            plt.plot(timeSeries, label = "Rod Velocity: %g m/s"%i.parameters['driverVD'])


        plt.legend()
        plt.show()



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

    pattern = "grooveHeight\d+-grooveSize\d+"
    # instanceList = glob.walk(pattern, Analyzer)
    an = Analyzer()
    an.plotFrontVelocities()
    an.readAll()
    an.plotAttachedSprings(show = True, save = False)
    #comp = Compare(instanceList)
    #comp.makeStaticCoeffArray()
    #comp.printStaticCoeffArray()
    #comp.plotCoeff3D()
    #comp.plotPushers()
    #analyser = Analyzer(args.dir)
