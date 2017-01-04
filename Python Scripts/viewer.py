import numpy as np
import matplotlib.pyplot as plt

folder = "/home/daniel/Dokumenter/Skole/FrictionProject/build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/"

normalData = np.loadtxt(folder + "normalforces.txt")
pusherData = np.fromfile(folder + "pusherforces.bin")
posData = np.fromfile(folder + "Output/node_position_interface.bin")

print "pos ", len(posData)
posData = (posData[::2]).copy()
posData.resize(len(posData)/(57.0),57)

springAttachedData = np.fromfile(folder + "Output/node_springs_attached_interface.bin")
print "spring ", len(springAttachedData)

springAttachedData2 = (springAttachedData[1::2]).copy()
springAttachedData = (springAttachedData[::2]).copy()


springAttachedData /= springAttachedData[0]
springLen = len(springAttachedData)
springAttachedData.resize(len(springAttachedData)/(57.0),56)


springAttachedData2 /= springAttachedData2[0]
# springLen = len(springAttachedData)
springAttachedData2.resize(springLen/(57.0),56)

print np.sum(springAttachedData==springAttachedData2)/float((springAttachedData.size))

springPlot = (springAttachedData+springAttachedData2)/2.0



# print normalData.shape
# print posData.shape
# print posData
# plt.pcolor(normalData)
# plt.show()
# plt.pcolor(posData-posData[0,:])
# plt.show()
# plt.plot(pusherData)
# plt.show()
# plt.pcolor(springAttachedData2)
plt.pcolor(springPlot)
plt.show()
