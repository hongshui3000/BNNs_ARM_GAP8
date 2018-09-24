import matplotlib.pyplot as plt
import pandas as pd
import csv
plt.figure(figsize=(6,4))
plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.98)
#acc = pd.read_csv("train74_test65/test_acc07_05.csv", header=None)
x = []
y = []

with open('train74_test65/test_acc07_01.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x.append(int(row[0]))
        y.append(float(row[1]))
x2 = []
y2 = []
with open('train74_test65/train_acc07_01.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x2.append(int(row[0]))
        y2.append(float(row[1]))

plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.plot(x,y, label='Testing Dataset')
plt.plot(x2,y2, label='Training Dataset')
plt.xlabel('Epoch',fontsize=12)
plt.ylabel('Top-1 Accuracy (%)',fontsize=12)
#plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()
