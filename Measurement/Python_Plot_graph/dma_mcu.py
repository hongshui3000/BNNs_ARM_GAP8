import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv

x = [' First','B1.1','B1.2','B2.1','B2.2','B2.3','B2.4','B3.1','B3.2','B3.3','B3.4','B4.1','B4.2','B4.3','B4.4','Last']
y_dma = [732,689,614,734,734,744,744,1114,1134,1190,1210,1190,1210,1194,1210,3670]
y_mcu = [17617,12844,12579,21360,21360,21370,21370,38660,38680,38736,38756,38736,38756,38740,38756,147253]
y_data = [2304,1664,1664,2816,2816,2816,2816,5120,5120,5120,5120,5120,5120,5120,5120,19568]
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.plot(x, y_dma,label='DMA',color='r')
plt.plot(x,y_mcu,label='MCU',color='b')
plt.plot(x,y_data,label='Data',color='k')
plt.xticks(x)
plt.xlabel('Layer',fontsize=16)
plt.ylabel('Cycles ',fontsize=16)
#plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()
