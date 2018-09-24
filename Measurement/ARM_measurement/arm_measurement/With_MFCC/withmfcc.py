import matplotlib.pyplot as plt
import csv
import numpy as np

x1 = []
y = []
totoal_cur = []
mfcc = []
f1 = []
b1 = []
b2 =[]
b3 =[]
b4 =[]
L1 =[]
header = 0
#with open('All_period_MFCC_BNNs_32000_points.csv','r') as csvfile:
with open('first_period_with_mfcc.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    #plots.next()
    for row in plots:
    	if header == 0:
    		print('continue')
    		header = 1
        else:
            x1.append(float(row[0])+3.84)
            y.append(float(row[1])*1000) #current
            #if float(row[0]) < -2.031 and float(row[0]) > -3.86:           
            #	totoal_cur.append(float(row[1])*1000)            
            if float(row[0]) < 3.888 and float(row[0]) > -3.86:           
            	totoal_cur.append(float(row[1])*1000)
            if float(row[0]) < -3.7522 and float(row[0]) > -3.86:
            	mfcc.append(float(row[1])*1000)
            if float(row[0]) < -3.536 and float(row[0]) > -3.752:
            	f1.append(float(row[1])*1000)
            if float(row[0]) < -3.369 and float(row[0]) > -3.536:
            	b1.append(float(row[1])*1000)
            if float(row[0]) < -2.742 and float(row[0]) > -3.369:
            	b2.append(float(row[1])*1000)
            if float(row[0]) < -2.470 and float(row[0]) > -2.742:
            	b3.append(float(row[1])*1000)
            if float(row[0]) < -2.176 and float(row[0]) > -2.470:
            	b4.append(float(row[1])*1000)
            if float(row[0]) < -2.031 and float(row[0]) > -2.176:
            	L1.append(float(row[1])*1000)

            if float(row[0]) < -1.913 and float(row[0]) > -2.031:
            	mfcc.append(float(row[1])*1000)
            if float(row[0]) < -1.682 and float(row[0]) > -1.913:
            	f1.append(float(row[1])*1000)
            if float(row[0]) < -1.502 and float(row[0]) > -1.682:
            	b1.append(float(row[1])*1000)
            if float(row[0]) < -0.805 and float(row[0]) > -1.502:
            	b2.append(float(row[1])*1000)
            if float(row[0]) < -0.503 and float(row[0]) > -0.805:
            	b3.append(float(row[1])*1000)
            if float(row[0]) < -0.166 and float(row[0]) > -0.503:
            	b4.append(float(row[1])*1000)
            if float(row[0]) < -0.0085 and float(row[0]) > -0.166:
            	L1.append(float(row[1])*1000)

            if float(row[0]) < 0.097 and float(row[0]) > -0.0085:
            	mfcc.append(float(row[1])*1000)
            if float(row[0]) < 0.332 and float(row[0]) > 0.097:
            	f1.append(float(row[1])*1000)
            if float(row[0]) < 0.510 and float(row[0]) > 0.332:
            	b1.append(float(row[1])*1000)
            if float(row[0]) < 1.208 and float(row[0]) > 0.510:
            	b2.append(float(row[1])*1000)
            if float(row[0]) < 1.508 and float(row[0]) > 1.208:
            	b3.append(float(row[1])*1000)
            if float(row[0]) < 1.854 and float(row[0]) > 1.508:
            	b4.append(float(row[1])*1000)
            if float(row[0]) < 2.003 and float(row[0]) > 1.854:
            	L1.append(float(row[1])*1000)

            if float(row[0]) < 2.105 and float(row[0]) > 2.003:
            	mfcc.append(float(row[1])*1000)
            if float(row[0]) < 2.320 and float(row[0]) > 2.105:
            	f1.append(float(row[1])*1000)
            if float(row[0]) < 2.492 and float(row[0]) > 2.320:
            	b1.append(float(row[1])*1000)
            if float(row[0]) < 3.140 and float(row[0]) > 2.492:
            	b2.append(float(row[1])*1000)
            if float(row[0]) < 3.420 and float(row[0]) > 3.140:
            	b3.append(float(row[1])*1000)
            if float(row[0]) < 3.734 and float(row[0]) > 3.420:
            	b4.append(float(row[1])*1000)
            if float(row[0]) < 3.881 and float(row[0]) > 3.734:
            	L1.append(float(row[1])*1000)
total_time = 3.881 + 3.86
mfcc_time = -3.7522 + 3.86 + (-1.913 - (-2.031)) + (0.097 - (-0.0085)) + (2.105 - 2.003)
f1_time =-3.536 + 3.752 + (-1.682 - (-1.913)) + (0.332 - 0.097) + (2.320 - 2.105)
b1_time =-3.369 + 3.536 + (-1.502 - (-1.682)) + (0.510 - 0.332) + (2.492 - 2.320)
b2_time =-2.742 + 3.369 + (-0.805 - (-1.502)) + (1.208 - 0.510) + (3.140 - 2.492)
b3_time =-2.470 + 2.742 + (-0.503 - (-0.805)) + (1.508 - 1.208) + (3.420 - 3.140)
b4_time =-2.176 + 2.470 + (-0.166 - (-0.503)) + (1.854 - 1.508) + (3.734 - 3.420)
L1_time =-2.031 + 2.176 + (-0.0085 -(-0.166)) + (2.003 - 1.854) + (3.881 - 3.734)

current = np.mean(totoal_cur) #first tile
mfcc_current = np.mean(mfcc)
f1_current = np.mean(f1)
b1_current = np.mean(b1)
b2_current = np.mean(b2)
b3_current = np.mean(b3)
b4_current = np.mean(b4)
L1_current = np.mean(L1)
#current = sum(y)/25209 #whole
print("total       I * V * s = " + str(current) + " * " + str(3.3) + " * " + str(total_time) + " = " + str(current*3.3*total_time))
print("mfcc        I * V * s = " + str(mfcc_current) + " * " + str(3.3) + " * " + str(mfcc_time) + " = " + str(mfcc_current*3.3*mfcc_time))
print("first Layer I * V * s = " + str(f1_current) + " * " + str(3.3) + " * " + str(f1_time) + " = " + str(f1_current*3.3*f1_time))
print("Binary 1,   I * V * s = " + str(b1_current) + " * " + str(3.3) + " * " + str(b1_time) + " = " + str(b1_current*3.3*b1_time))
print("Binary 2,   I * V * s = " + str(b2_current) + " * " + str(3.3) + " * " + str(b2_time) + " = " + str(b2_current*3.3*b2_time))
print("Binary 3,   I * V * s = " + str(b3_current) + " * " + str(3.3) + " * " + str(b3_time) + " = " + str(b3_current*3.3*b3_time))
print("Binary 4,   I * V * s = " + str(b4_current) + " * " + str(3.3) + " * " + str(b4_time) + " = " + str(b4_current*3.3*b4_time))
print("Last Layer, I * V * s = " + str(L1_current) + " * " + str(3.3) + " * " + str(L1_time) + " = " + str(L1_current*3.3*L1_time))
plt.plot(x1,y, label='')
plt.xlim(-.2, 1.9)
plt.ylim(50, 110)
plt.xticks(fontsize=28)
plt.yticks(fontsize=28)
plt.xlabel('time (s)',fontsize=28)
p = plt.axvspan(-3.86+3.86, -3.767+3.86, facecolor='green', alpha=0.5)
p = plt.axvspan(-3.767+3.86, -3.561+3.86, facecolor='gray', alpha=0.5)
p = plt.axvspan(-3.561+3.86, -3.384+3.86, facecolor='gray', alpha=0.5)
p = plt.axvspan(-3.384+3.86, -2.760+3.86, facecolor='gray', alpha=0.5)
p = plt.axvspan(-2.760+3.86, -2.490+3.86, facecolor='gray', alpha=0.5)
p = plt.axvspan(-2.490+3.86, -2.190+3.86, facecolor='gray', alpha=0.5)
p = plt.axvspan(-2.190+3.86, -2.046+3.86, facecolor='gray', alpha=0.5)
plt.ylabel('Current (mA)',fontsize=28)
#plt.title('Current-Time')
#plt.legend()
plt.show()