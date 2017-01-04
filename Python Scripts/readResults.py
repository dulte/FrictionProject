# Read binary file
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button

class FrictionAnalyser:
    def __init__(self, parameterFileName):
        self.parameterFileName = parameterFileName
        self.readParameters()

    def sliderRepresentation(self, spaceTimeFile):
        self.spaceTimeData = np.loadtxt(spaceTimeFile)
        self.nt = len(self.spaceTimeData)/self.nx;
        # self.spaceTimeData.resize(self.nt, self.nx)
        self.figure = plt.figure()
        self.figaxes = plt.axes([0.30, 0.15, 0.65, 0.85])
        plt.plot(self.spaceTimeData[0,:])

        self.axes = plt.axes([0.25, 0.1, 0.65, 0.03])
        self.slider_object = Slider(self.axes, 'tid', 0, 400, 1)
        self.slider_object.on_changed(self.on_changed_action)
        plt.show()

    def on_changed_action(self, val):
        ydata = self.spaceTimeData[int(val), :]
        lines = self.figaxes.get_lines()
        self.figaxes.lines[0].set_ydata(ydata)
        self.figaxes.set_ylim([-0.01, 100])
        #self.figaxes.autoscale_view(True, True, True)
        self.figure.canvas.draw_idle()

    def readParameters(self):
        ifile = open(self.parameterFileName, "r")
        fileContents = ifile.readlines()
        for line in fileContents:
            words = line.split()
            print words
            if words[0] == "nx":
                self.nx = float(words[1])
            if words[0] == "dt":
                self.dt = float(words[1])
            # TODO: get more parameters from parameter file

    def pcolorplot(self, spaceTimeFile):
        spaceTimeData = np.fromfile(spaceTimeFile)
        nt = len(spaceTimeData)/self.nx;
        spaceTimeData.resize(nt, self.nx)
        plt.pcolormesh(spaceTimeData-spaceTimeData[0])
        plt.xlabel('Block index')
        plt.ylabel('Time step')
        plt.colorbar()
        plt.show()

if __name__=="__main__":
    folder = "/home/daniel/Dokumenter/Skole/FrictionProject/build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/"
    normalforces = (folder + "normalforces.txt")
    filenameParameters = "parameters.txt"
    filenamePositions = "output/friction.bin"
    analyser = FrictionAnalyser(filenameParameters)
    analyser.sliderRepresentation(normalforces)
