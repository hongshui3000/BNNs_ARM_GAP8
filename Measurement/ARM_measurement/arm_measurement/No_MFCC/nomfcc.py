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
with open('whole_period_NoMFCC_32000_point 0.csv','r') as csvfile:
#with open('first_period_nomfcc.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    #plots.next()
    for row in plots:
    	if header == 0:
    		print('continue')
    		header = header +1
    	else:
        	x1.append(float(row[0])+3.876)
        	y.append(float(row[1])*1000) #current
#current = sum(y)/5610
current = sum(y)/23903
print("I * V * s = " + str(current) + " * " + str(3.3) + " * " + str(7.3) + " = " + str(current*3.3*7.3))
plt.plot(x1,y, label='')
#plt.xlim(-.2, 1.9)
plt.ylim(55, 95)
plt.xticks(fontsize=28)
plt.yticks(fontsize=28)
plt.xlabel('time (s)',fontsize=28)
p = plt.axvspan(-3.876+3.876, -3.66+3.876, facecolor='yellow', alpha=0.5)
p = plt.axvspan(-3.66+3.876, -3.493+3.876, facecolor='green', alpha=0.5)
p = plt.axvspan(-3.493+3.876, -2.866+3.876, facecolor='green', alpha=0.5)
p = plt.axvspan(-2.866+3.876, -2.594+3.876, facecolor='green', alpha=0.5)
p = plt.axvspan(-2.594+3.876, -2.30+3.876, facecolor='green', alpha=0.5)
p = plt.axvspan(-2.30+3.876, -2.155+3.876, facecolor='gray', alpha=0.5)
plt.ylabel('Current (mA)',fontsize=28)
#plt.title('Current-Time')
#plt.legend()
plt.show()