import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv
import datetime
x = ('Preprocessing','First','B1','B2','B3','B4', 'Last')
#top_1 = [22.97, 30.89, 57.18, 62.55, 62.76, 62.88, 62.95]
#top_5 = [54.51, 62.32, 88.96, 90.71, 90.97,91.02,91.17]
#plt.plot(x, top_1,label='Top 1 accuracy')
#plt.plot(x, top_5,label='Top 5 accuracy')
#plt.xticks(x)
#plt.xlabel('Q Format',fontsize=16)
#plt.ylabel('Accuracy(%) ',fontsize=16)
#plt.title('Interesting Graph\nCheck it out')

n_groups = 7

#GAP8 = (0.26,2.04,0.2,4.24,2.4,0.17) #no tiling e.g 64x400
#GAP8 = (0.26,2.04,0.2,4.24,0.8,0.17) #no tiling e.g 64x400, dct change, 2 k
#CORTEXM4 = (3.8984,14.7312,1.3312,5.7052,6.4052,0.78) #64x400
GAP8 = (7.838, 24.029, 9.792, 26.113, 8.976, 9.792, 18.596) #tiling e.g 64x580, dct change, 2 k
CORTEXM4 = ( 57, 216, 155, 658, 309, 351, 131) #64x460

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.35

#opacity = 0.4

rects1 = ax.bar(index, GAP8, bar_width,
                color='blue',
                alpha=0.8,
                label='GAP8')
rects2 = ax.bar(index + bar_width, CORTEXM4, bar_width,
                color='gray',
                alpha=0.5,
                label='Cortex-M4')
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.98)
#plt.figure(figsize=(6,3))
ax.set_xlabel('The whole process',fontsize=12)
ax.set_ylabel('Energy (mJ)',fontsize=12)
#ax.set_title('Scores by group and gender')
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(('Preprocessing','First','1.Binary','2.Binary','3.Binary','4.Binary', 'Last')) 
ax.legend()

#fig.tight_layout()
plt.show()
