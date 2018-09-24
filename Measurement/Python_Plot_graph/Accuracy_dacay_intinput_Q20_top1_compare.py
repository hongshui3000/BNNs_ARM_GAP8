import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv
from matplotlib.dates import date2num
import datetime
plt.figure(figsize=(6,3))
plt.gcf().subplots_adjust(bottom=0.10,left=0.1,right=0.98)
x = ['8','9','10','12','16','20','flaot']
#top_1 = [22.97, 30.89, 57.18, 62.55, 62.76, 62.88, 62.95]
#top_5 = [54.51, 62.32, 88.96, 90.71, 90.97,91.02,91.17]
#plt.plot(x, top_1,label='Top 1 accuracy')
#plt.plot(x, top_5,label='Top 5 accuracy')
#plt.xticks(x)
#plt.xlabel('Q Format',fontsize=16)
#plt.ylabel('Accuracy(%) ',fontsize=16)
#plt.title('Interesting Graph\nCheck it out')

n_groups = 6

top_1 = (21.31, 31.83, 56.66, 61.64, 61.93, 61.93) #Q8.0
top_5 = (22.97, 30.89, 57.18, 62.55, 62.76, 62.88) #Q8.20

#top_5 = (54.51, 62.32, 88.96, 90.71, 90.97, 91.02, 91.17)

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.35

#opacity = 0.4
error_config = {'ecolor': '0.3'}

rects1 = ax.bar(index, top_1, bar_width,
                color='blue',
                alpha=0.7,
                error_kw=error_config,
                label='Q8.0 MFCCs')

rects2 = ax.bar(index + bar_width, top_5, bar_width,
                color='red',
                alpha=0.7,
                error_kw=error_config,
                label='Q8.20 MFCCs')
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
ax.set_xlabel('Model data',fontsize=12)
ax.set_ylabel('Top-1 Accuracy (%)',fontsize=12)
#ax.set_title('Scores by group and gender')
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(('Q7.8.','Q7.9','Q7.10','Q7.12','Q7.16','Q7.20'))
ax.legend()

fig.tight_layout()
plt.show()
