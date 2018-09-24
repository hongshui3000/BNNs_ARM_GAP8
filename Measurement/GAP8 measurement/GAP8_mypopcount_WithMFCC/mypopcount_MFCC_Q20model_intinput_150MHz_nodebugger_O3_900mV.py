import matplotlib.pyplot as plt
import csv
import numpy as np
plt.figure(figsize=(6,3))
plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.98)
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
starttime = -0.0126
endtime = 0.9866
#with open('All_period_MFCC_BNNs_32000_points.csv','r') as csvfile:
with open('mypopcount_MFCC_Q20model_intinput_150MHz_nodebugger_O3_900mV.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    #plots.next()
    for row in plots:
        if header == 0:
            print('continue')
            header = 1
        else:
            x1.append(float(row[0])-starttime)
            y.append(float(row[1])) #current
            #if float(row[0]) < -2.031 and float(row[0]) > -3.86:           
            #	totoal_cur.append(float(row[1]))            
            if float(row[0]) < endtime and float(row[0]) > starttime:    #line 17206 - 2077 +1       
            	totoal_cur.append(float(row[1]))
            #if float(row[0]) < starttime+0.005469 and float(row[0]) > starttime:
            #	mfcc.append(float(row[1]))
            #if float(row[0]) < -3.536 and float(row[0]) > -3.752:
            #	f1.append(float(row[1]))
            #if float(row[0]) < -3.369 and float(row[0]) > -3.536:
            #	b1.append(float(row[1]))
            #if float(row[0]) < -2.742 and float(row[0]) > -3.369:
            #	b2.append(float(row[1]))
            #if float(row[0]) < -2.470 and float(row[0]) > -2.742:
            #	b3.append(float(row[1]))
            #if float(row[0]) < -2.176 and float(row[0]) > -2.470:
            #	b4.append(float(row[1]))
            #if float(row[0]) < -2.031 and float(row[0]) > -2.176:
            #	L1.append(float(row[1]))

total_time = endtime - starttime
#mfcc_time = -3.7522 + 3.86 + (-1.913 - (-2.031)) + (0.097 - (-0.0085)) + (2.105 - 2.003)
#f1_time =-3.536 + 3.752 + (-1.682 - (-1.913)) + (0.332 - 0.097) + (2.320 - 2.105)
#b1_time =-3.369 + 3.536 + (-1.502 - (-1.682)) + (0.510 - 0.332) + (2.492 - 2.320)
#b2_time =-2.742 + 3.369 + (-0.805 - (-1.502)) + (1.208 - 0.510) + (3.140 - 2.492)
#b3_time =-2.470 + 2.742 + (-0.503 - (-0.805)) + (1.508 - 1.208) + (3.420 - 3.140)
#b4_time =-2.176 + 2.470 + (-0.166 - (-0.503)) + (1.854 - 1.508) + (3.734 - 3.420)
#L1_time =-2.031 + 2.176 + (-0.0085 -(-0.166)) + (2.003 - 1.854) + (3.881 - 3.734)

current = np.mean(totoal_cur) #first tile
#mfcc_current = np.mean(mfcc)
#f1_current = np.mean(f1)
#b1_current = np.mean(b1)
#b2_current = np.mean(b2)
#b3_current = np.mean(b3)
#b4_current = np.mean(b4)
#L1_current = np.mean(L1)
  #current = sum(y)/25209 #whole
print("total       I * V * s = " + str(current) + " * " + str(2.8) + " * " + str(total_time) + " = " + str(current*2.8*total_time))
#print("mfcc        I * V * s = " + str(mfcc_current) + " * " + str(2.8) + " * " + str(mfcc_time) + " = " + str(mfcc_current*2.8*mfcc_time))
#print("first Layer I * V * s = " + str(f1_current) + " * " + str(2.8) + " * " + str(f1_time) + " = " + str(f1_current*2.8*f1_time))
#print("Binary 1,   I * V * s = " + str(b1_current) + " * " + str(2.8) + " * " + str(b1_time) + " = " + str(b1_current*2.8*b1_time))
#print("Binary 2,   I * V * s = " + str(b2_current) + " * " + str(2.8) + " * " + str(b2_time) + " = " + str(b2_current*2.8*b2_time))
#print("Binary 3,   I * V * s = " + str(b3_current) + " * " + str(2.8) + " * " + str(b3_time) + " = " + str(b3_current*2.8*b3_time))
#print("Binary 4,   I * V * s = " + str(b4_current) + " * " + str(2.8) + " * " + str(b4_time) + " = " + str(b4_current*2.8*b4_time))
#print("Last Layer, I * V * s = " + str(L1_current) + " * " + str(2.8) + " * " + str(L1_time) + " = " + str(L1_current*2.8*L1_time))
plt.plot(x1,y,'black', label='',linewidth=0.5)
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.xlim(-0.2-starttime, 1.1 -starttime)
#plt.xlim(1.73-1.732,  2.3372 -1.732)
#plt.xlim(1.73-1.732,  2.3392 -1.732)
plt.ylim(10, 56)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.xlabel('time (s)',fontsize=12)
p = plt.axvspan(0, 0+0.00656677, facecolor='blue', alpha=0.6)
p = plt.axvspan(0+0.00656677, 0+0.0258375, facecolor='green', alpha=0.6)
p = plt.axvspan(0+0.0258375, 0+0.0690523, facecolor='gray', alpha=0.6)
p = plt.axvspan(0+0.0690523, 0+0.0870845, facecolor='red', alpha=0.6)
plt.ylabel('Current (mA)',fontsize=12)
#plt.title('Current-Time')
#plt.legend()
plt.show()