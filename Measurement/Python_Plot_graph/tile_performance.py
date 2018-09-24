import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv

x = ['25','20','16','10']
y_dma = [216.262502,191.949063,179.307836,144.444690]
x1 = ['25','20','16','10']
y_mcu = [231.285888,203.992223,188.963563,149.638306]
n_groups = 4

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.25

#opacity = 0.4
error_config = {'ecolor': '0.3'}

rects1 = ax.bar(index, y_dma, bar_width,
                color='blue',
                alpha=0.7,
                error_kw=error_config,
                label='With DMA')

rects2 = ax.bar(index + bar_width, y_mcu, bar_width,
                color='gray',
                alpha=0.5,
                error_kw=error_config,
                label='With MCU')
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.98)
ax.set_xlabel('Number of Tiles',fontsize=12)
ax.set_ylabel('BNNs Cycles (x$10^6$)',fontsize=12)
#ax.set_title('Scores by group and gender')
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(('25','20','16','10'))
ax.legend()

fig.tight_layout()
plt.show()
