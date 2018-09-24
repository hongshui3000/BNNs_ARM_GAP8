import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv
import datetime
x = ('fBNN','xBNN','Par', 'Parf')

n_groups = 4

GAP8_time = (20.206, 4.719, 1.066, 0.562) #tiling e.g 64x580, dct change, 2 k
GAP8_energy = ( 1.7, 0.389, 0.2, 0.106) #64x460

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.30

#opacity = 0.4
ax1 = ax.twinx()
rects1 = ax.bar(index, GAP8_time, bar_width,
                color='blue',
                alpha=0.8)
rects2 = ax1.bar(index + bar_width, GAP8_energy, bar_width,
                color='red',
                alpha=0.8)
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
#plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.98)
#plt.figure(figsize=(6,3))
ax.set_xlabel('Configurations',fontsize=12)
ax.set_ylabel('Time (s)',fontsize=12, color='blue')
ax.tick_params('y', colors='blue')
ax1.set_ylabel('Energy (mJ)',fontsize=12, color='r')
ax1.tick_params('y', colors='r')
ax.set_ylim(0, 22)
ax1.set_ylim(0, 2.2)
#ax.set_title('Scores by group and gender')
#plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(('fBNN','xBNN','Par', 'Parf')) 
ax.legend()

#fig.tight_layout()
plt.show()
