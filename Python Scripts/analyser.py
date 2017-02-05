import numpy as np
import matplotlib.pyplot as plt
import argparse
import os
import sys

class DataReader:
    """ Reads and converts the raw data into a manageable form """

    def __init__(self, parameters, folder=os.getcwd()):
        "Read the file and do the necessary conversions"
        self.folder = os.path.abspath(folder)
        self.params = parameters

    def read(self, filename):
        np.fromfile(os.path.join(self.folder, filename))

    def resize(self, array, everySecondElement=False, start=0):
        if everySecondElement:
            array = array[start::2]
        array.resize(len(array)/(self.params["bottomNodes"])-1,
                     self.params["bottomNodes"])-1

class Analyzer:
    def __init__(self, folder=os.getcwd()):
        "Read all available files in the folder"
        print(folder)
        self.folder = os.path.abspath(folder)
        print(self.folder)
        sys.stdout.write("Reading parameters...")
        self.readParameters()
        sys.stdout.write("done.\n")
        self.datareader = DataReader(self.parameters, self.folder)

        self.readAll()

    def readAll(self):
        self.normalForce = self.readAndResize('normal_force.bin')
        self.pusherForce = self.readAndResize('pusher_force.bin')
        self.positionInterface = self.readAndResize('node_position_interface.bin')
        self.attachedSprings = self.readAndResize('node_springs_attached_interface.bin')
        self.shearForce = self.readAndResize('shear_force.bin')

    def readAndResize(self, filename, start=0):
        data = self.read(filename)
        return self.datareader(data, everySecondElement=True, start=start)

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
                    if float(words[1]) - int(words[1]) == 0:
                        self.parameters[words[0]] = int(words[1])
                    else:
                        self.parameters[words[0]] = float(words[1])
                except ValueError as valErr:
                    print(valErr)

    def plotNormalForce(self):
        plt.pcolor(self.normalForce)
        plt.title("Normal force")
        plt.show()

    def plotPusherForce(self):
        plt.pcolor(self.pusherForce)
        plt.title("Force on pusher")
        plt.show()

    def plotPositionInterface(self):
        pos = self.positionInterface - self.positionInterface[0, :]
        plt.pcolor(pos)
        plt.title("Relative position of the interface")
        plt.show()

    def plotAttachedSprings(self):
        plt.pcolor(self.attachedSprings)
        plt.title("Percent of attached springs")
        plt.xlabel("Block")
        plt.ylabel("Time")
        plt.show()

    def plotShearForce(self):
        plt.pcolor(self.shearForce)
        plt.title("Shear force")
        plt.show()

if __name__ == '__main__':
    analyser = Analyzer('../Output')
    analyser.plotNormalForce()
    analyser.plotPusherforce()
    analyser.plotPositionInterface()
    analyser.AttachedSprings()
