import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv

x = []
y = []

with open('model_mymodel_F_07_05.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x.append(int(row[0])-20)
        y.append(float(row[1]))
plt.bar(x, y)
plt.xticks(x)
plt.xlabel('Position of bits to represent the floating-point model data ',fontsize=16)
plt.ylabel('Number of model data',fontsize=16)
#plt.title('Interesting Graph\nCheck it out')
#plt.legend()
plt.show()
