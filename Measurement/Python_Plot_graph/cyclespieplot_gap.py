import numpy as np
import matplotlib.pyplot as plt
#plt.figure(figsize=(5.5,3))
#fig, ax = plt.subplots(figsize=(6, 3), subplot_kw=dict(aspect="equal"))
#plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.95)

sizes = [7.838 , 24.029,
          9.792,
          26.113,
          8.976,
          9.792,
          18.596]

# Pie chart, where the slices will be ordered and plotted counter-clockwise:
labels = "MFCC\n (7.838 mJ)", 'First Layer\n (24.029 mJ)', '1.Binary\n (9.792 mJ)','2.Binary\n (26.113 mJ)', '3.Binary\n (8.976 mJ)','4.Binary\n (9.792 mJ)','Last Layer\n (18.596 mJ)'
colors1 = 'blue', 'green', 'gray', 'gray', 'gray', 'gray', 'red'
#sizes = [15, 30, 45, 10]
explode = (0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01)  # only "explode" the 2nd slice (i.e. 'Hogs')

fig1, ax1 = plt.subplots()
fig1.subplots_adjust(bottom=0.04,) # or whatever
_, texts, autotexts = ax1.pie(sizes, explode=explode, labels=labels, colors=colors1, autopct='%1.2f%%',
        shadow=False, startangle=90)
for autotext in autotexts:
    autotext.set_color('white')
    autotext.set_fontsize(12)
for text in texts:
    text.set_fontsize(14)
ax1.axis('equal',fontsize=32)  # Equal aspect ratio ensures that pie is drawn as a circle.

plt.show()