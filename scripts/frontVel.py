import numpy as np
import matplotlib.pyplot as plt
from collections import *
import os,sys, subprocess
from scipy import interpolate


class viewer:
    def __init__(self, parametersName,metaFolder):

        self.metaFolder = metaFolder
        dirs = []
        for dirpath, dirnames, filenames in os.walk(metaFolder):
            dirs.append(dirnames)

        self.outputFolders = dirs[0]

        #self.parameters = getMultipleParameters(parametersName)
        self.parameters = self.readParameters(metaFolder + parametersName)
        print self.parameters



    def getMultipleParameters(self,filename):
        parameters = {}
        for f in outputFolders:
            parameters[f] = (self.readParameters(filename))
        return parameters


    def readParameters(self,filename):
        parameters = {}

        with open(filename) as f:
            for line in f:
                words = line.split()
                parameters[str(words[0])] = float(words[1])

        return parameters

    def resizeData(self,data, everySecondElement = False, start = 0):
        if everySecondElement:
            data = data[start::2].copy()
        data2 = data.copy()
        length = len(data)
        data2.resize((length)/(float(self.parameters["bottomNodes"])-1),int(self.parameters["bottomNodes"]) - 1)
        return data2


    def findFrontVelocity(self,plot=True):

        data = np.fromfile(self.metaFolder + "node_springs_attached_interface.bin")
        data = self.resizeData(data,True)
        reducedData = data



        fronts = np.zeros((reducedData.shape[0]))

        length = (reducedData.shape[1])

        print reducedData

        maxElement = np.max(reducedData)

        smoothingSize = 20
        smoothingLimit = 0.5
        smoothOutliarSize = 1.3

        smoothRep = 0

        for i in range(int(reducedData.shape[0])):
            for j in range(1,length-1):

                if (reducedData[i,(length-1)-j] >= maxElement*0.3 and reducedData[i,(length-1) - j-1] < maxElement*0.3):
                    fronts[i] = (length-1) - j
                    break
                elif reducedData[i,-1] < maxElement*0.4:
                    fronts[i] = length


        for _ in range(smoothRep):
            for j in range(smoothingSize):
                for i in range(j,int(len(fronts))-j):
                    if (fronts[i-j] != 0 and fronts[i] < fronts[i+j]*(1-smoothingLimit) and fronts[i+j] != 0) and fronts[i-j] <= fronts[i+j]:
                        fronts[i] = (fronts[i-j] + fronts[i+j])/2.0
            for j in range(smoothingSize):
                for i in range(j,int(len(fronts))-j):
                    if (fronts[i-j] != 0 and fronts[i] > smoothOutliarSize*(fronts[i+j] + fronts[i-j])/2.0 and fronts[i+j] != 0) and fronts[i-j] <= fronts[i+j]:
                        fronts[i] = (fronts[i-j] + fronts[i+j])/2.0


        print fronts
        frontVel = np.zeros(int(len(fronts)))


        # plt.pcolor(data.T)
        # plt.plot(fronts, "y")
        # plt.show()


        # time = np.linspace(0,int(len(fronts)),int(len(fronts)))
        #
        #
        #
        # tck = interpolate.splrep(time,fronts,s = 3)
        # func = interpolate.splev(time,tck)
        # plt.plot(time,func)
        # plt.show()

        for i in range(1,int(len(fronts))):
            frontVel[i-1] = (fronts[i] - fronts[i-1])/(float(self.parameters["step"])*100)*0.005

        # slippedNodes = np.sum(reducedData,axis = 1)
        #
        # frontVel = np.zeros(int(len(slippedNodes))-1)
        #
        # for i in range(1,int(len(slippedNodes))):
        #     frontVel[i-1] = (slippedNodes[i] - slippedNodes[i-1])/float(self.parameters["step"])
        #
        # frontVel = np.where(frontVel < 0, 0, frontVel)


        if plot:
            plt.semilogy(frontVel, "o")

            fig, ax1 = plt.subplots()
            ax2 = ax1.twinx()
            ax1.plot(fronts, "r")
            ax2.plot(np.where(frontVel>0,frontVel,0))

            # plt.plot(np.where(frontVel>0,frontVel,0))
            plt.show()
        else:
            return frontVel





#folder = "build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/OutputTest"


# dirs = []
# for dirpath, dirnames, filenames in os.walk(folder):
#     dirs.append(dirnames)
#
# outputFolders = dirs[0]

if __name__ == "__main__":
    folder = "/home/daniel/Dokumenter/Skole/FrictionProject/build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/Output/"
    a = viewer("parameters.txt",folder)
    a.findFrontVelocity()

# for f in outputFolders:
#     data = np.fromfile(folder + "/" + f + "/" + "node_position_interface.bin")
#     plt.plot(data)
#     plt.show()
