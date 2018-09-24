import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv

x = [' First','B1.1','B1.2','B2.1','B2.2','B2.3','B2.4','B3.1','B3.2','B3.3','B3.4','B4.1','B4.2','B4.3','B4.4','Last']
#y_dma = [732,689,614,734,734,744,744,1114,1134,1190,1210,1190,1210,1194,1210,3670]
y_dma = [0.73,0.69,0.61,0.73,0.73,0.74,0.74,1.11,1.13,1.19,1.21,1.19,1.21,1.19,1.21,3.67]
y_dma_label = ['0.73k','0.69k','0.61k','0.73k','0.73k','0.74k','0.74k','1.11k','1.13k','1.19k','1.21k','1.19k','1.21k','1.19k','1.21k','3.67k']
#y_dma_label = ['732','689','614','734','734','744','744','1114','1134','1190','1210','1190','1210','1194','1210','3670']
#y_mcu = [17617,12844,12579,21360,21360,21370,21370,38660,38680,38736,38756,38736,38756,38740,38756,147253]
y_mcu = [17.61,12.84,12.58,21.36,21.36,21.37,21.37,38.66,38.68,38.74,38.76,38.73,38.76,38.74,38.76,147.25]
y_mcu_label = ['17.61k','12.84k','12.58k','21.36k','21.36k','21.37k','21.37k','38.66k','38.68k','38.74k','38.76k','38.73k','38.76k','38.74k','38.76k','147.25k']
#y_mcu_label = ['17617','12844','12579','21360','21360','21370','21370','38660','38680','38736','38756','38736','38756','38740','38756','147253']
y_data = [2304,1664,1664,2816,2816,2816,2816,5120,5120,5120,5120,5120,5120,5120,5120,19568]
n_groups = 16

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.45

#opacity = 0.4
error_config = {'ecolor': '0.3'}

rects1 = ax.barh(index, y_dma, bar_width,
                color='blue',
                alpha=0.8,
                label='DMA')

rects2 = ax.barh(index + bar_width, y_mcu, bar_width,
                color='gray',
                alpha=0.35,
                label='MCU')
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
ax.set_ylabel('Layers',fontsize=12)
ax.set_xlabel('Cycles (x$10^3$)',fontsize=12)
#ax.set_title('Scores by group and gender')
for i in range(len(x)):
    plt.text(y = i-0.3 , x = y_dma[i]+2, s = y_dma_label[i], size = 9)
plt.text(y = 0+0.2 , x = 30, s = y_mcu_label[0], size = 9)
plt.text(y = 1+0.2 , x = 20, s = y_mcu_label[1], size = 9)
plt.text(y = 2+0.2 , x = 20, s = y_mcu_label[2], size = 9)
plt.text(y = 3+0.2 , x = 30, s = y_mcu_label[3], size = 9)
plt.text(y = 4+0.2 , x = 30, s = y_mcu_label[4], size = 9)
plt.text(y = 5+0.2 , x = 30, s = y_mcu_label[5], size = 9)
plt.text(y = 6+0.2 , x = 30, s = y_mcu_label[6], size = 9)
plt.text(y = 7+0.2 , x = 20, s = y_mcu_label[7], size = 9)
plt.text(y = 8+0.2 , x = 20, s = y_mcu_label[8], size = 9)
plt.text(y = 9+0.2 , x = 20, s = y_mcu_label[9], size = 9)
plt.text(y = 10+0.2 , x = 20, s = y_mcu_label[10], size = 9)
plt.text(y = 11+0.2 , x = 20, s = y_mcu_label[11], size = 9)
plt.text(y = 12+0.2 , x = 20, s = y_mcu_label[12], size = 9)
plt.text(y = 13+0.2 , x = 20, s = y_mcu_label[13], size = 9)
plt.text(y = 14+0.2 , x = 20, s = y_mcu_label[14], size = 9)
plt.text(y = 15+0.2 , x = 120, s = y_mcu_label[15], size = 9)
ax.set_yticks(index + bar_width / 2)
ax.set_yticklabels(('First','B1.1','B1.2','B2.1','B2.2','B2.3','B2.4','B3.1','B3.2','B3.3','B3.4','B4.1','B4.2','B4.3','B4.4','Last')) 
ax.legend()

fig.tight_layout()
plt.show()

