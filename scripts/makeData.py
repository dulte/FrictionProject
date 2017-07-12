import numpy as np
from collections import *
import os,sys, subprocess


def getParameters(filename):
    try:
        with open(filename) as f:
            parameters = OrderedDict()
            for line in f:
                if line != "\n":
                    words = line.split()
                    if len(words) == 2:
                        parameters[words[0]] = words[1]
                    else:
                        parameters[words[0]] = ""

        return parameters
    except:
        print "Could not open parameters from " + filename
        exit(1)

def writeParametersToFile(parameters,num,hei,filename):
    with open(filename,"w") as f:
        for key in parameters:
            if parameters[key] != "":
                if key == "numberOfGrooves":
                    f.write(key + " " + str(num) + "\n")
                elif key == "grooveHeight":
                    f.write(key + " " + str(hei) + "\n")
                else:
                    f.write(key + " " + parameters[key] + "\n")
            elif parameters[key] == "":
                f.write("\n")
                f.write(key + "\n")


try:
    numStart = int(sys.argv[1])
    numStop = int(sys.argv[2])
    heightStart = int(sys.argv[3])
    heightStop = int(sys.argv[4])

    try:
        datafolder = str(sys.argv[5])
    except:
        folder = ""



except:
    print "The arguments are not correct: numStart, numStop, heightStart, heightStop"
    exit(1)


print getParameters("config.txt")

configName = "config.txt"

numbers = range(numStart,numStop+1)
heights = range(heightStart,heightStop+1)

for num in numbers:
    for hei in heights:
        parameters = getParameters(configName)
        writeParametersToFile(parameters,num,hei,configName)
        specificDataFolder = "data_n%s_h%s" %(num,hei)
        path = os.path.join(os.path.abspath(datafolder), specificDataFolder)
        try:
            os.makedirs(path)
        except:
            pass

        test = subprocess.Popen(["./FrictionProject", path],stdout=subprocess.PIPE)
        output = test.communicate()[0]
        print "Done with num = %s, height = %s" %(num,hei)

print "Done!"
