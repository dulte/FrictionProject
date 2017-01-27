import numpy as np
import matplotlib.pyplot as plt
# import seaborn


def readParameters(filename):
    parameters = {}

    with open(filename) as f:
        for line in f:
            words = line.split()
            parameters[str(words[0])] = float(words[1])

    return parameters

def resizeData(data, everySecondElement = False, start = 0):
    if everySecondElement:
        data = data[start::2].copy()
    data2 = data.copy()
    length = len(data)
    data2.resize((length)/(float(parameters["bottomNodes"])-1),int(parameters["bottomNodes"]) - 1)
    return data2






folder = "/home/daniel/Dokumenter/Skole/FrictionProject/build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/Output/"
folderParameters = "/home/daniel/Dokumenter/Skole/FrictionProject/build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/Output/"

parameters = readParameters(folderParameters + "parameters.txt")

print parameters


normalData = np.fromfile(folder + "normal_force.bin")
pusherData = np.fromfile(folder + "pusher_force.bin")
posData = np.fromfile(folder + "node_position_interface.bin")

shearData = np.fromfile(folder + "shear_force.bin")
shearData = resizeData(shearData,True)

time = np.linspace(0,parameters["step"]*1000*len(pusherData),len(pusherData))


print "pos ", len(posData)
posData = (posData[1::2]).copy()
posData.resize(len(posData)/(57.0),57)

#posData = resizeData(posData, everySecondElement = False,start = 0)

springAttachedData = np.fromfile(folder + "node_springs_attached_interface.bin")

print springAttachedData


normalData = resizeData(normalData, everySecondElement = True,start=0)
print normalData

springAttachedData = resizeData(springAttachedData,everySecondElement = True)
springPlot = springAttachedData
print springPlot
# print "spring ", len(springAttachedData)
#
# springAttachedData2 = (springAttachedData[1::2]).copy()
# springAttachedData = (springAttachedData[::2]).copy()
#
#
# springAttachedData /= springAttachedData[0]
# springLen = len(springAttachedData)
# springAttachedData.resize(len(springAttachedData)/(57.0),56)
#
#
# springAttachedData2 /= springAttachedData2[0]
# # springLen = len(springAttachedData)
# springAttachedData2.resize(springLen/(57.0),56)
#
# print np.sum(springAttachedData==springAttachedData2)/float((springAttachedData.size))
#
# springPlot = (springAttachedData+springAttachedData2)/2.0

# plt.pcolor(shearData)
# plt.show()
#
# # print normalData.shape
# # print posData.shape
# # print posData
# plt.pcolor(normalData)
# plt.show()
# plt.pcolor(posData-posData[0,:])
# plt.show()

plt.title("Force on Pusher")
plt.xlabel("Time [s]")
plt.ylabel("Force/Fn")
plt.plot(time,pusherData/1920)
plt.show()
# plt.pcolor(springAttachedData2)
plt.title("Percent of Springs Attached")
plt.xlabel("Block")
plt.ylabel("Time")
plt.pcolormesh(springPlot)
plt.show()
