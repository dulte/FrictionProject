import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns


class Analyzer:
    def __init__(self,inputpath,outputpath):
        self.inputpath = inputpath
        self.outpath = outpath

        self.readParameters()

    def readParameters(self):
        with open(self.inputpath + "/parameters.txt") as f:
            for line in f.readline():
                words = line.split()

                if words[0] == "grooveHeight":
                    self.grooveHeight = words[1]

        with open(self.inputpath + "/snapshot/model.xyz") as f:
            f.readline()
            self.maxTime = f.readline.split()[1]

    
