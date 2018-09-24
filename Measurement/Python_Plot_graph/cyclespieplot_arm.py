import numpy as np
import matplotlib.pyplot as plt
#plt.figure(figsize=(5.5,3))
#fig, ax = plt.subplots(figsize=(6, 3), subplot_kw=dict(aspect="equal"))
#plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.95)

sizes = [57 , 216,
          155,
          658,
          309,
          351,
          131]

# Pie chart, where the slices will be ordered and plotted counter-clockwise:
labels = "MFCC\n (57 mJ)", 'First Layer\n (216 mJ)', '1.Binary\n (155 mJ)','2.Binary\n (658 mJ)', '3.Binary\n (309 mJ)','4.Binary\n (351 mJ)','Last Layer\n (131 mJ)'
colors1 = 'blue', 'green', 'gray', 'gray', 'gray', 'gray', 'red'
#sizes = [15, 30, 45, 10]
explode = (0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01)  # only "explode" the 2nd slice (i.e. 'Hogs')
xxx=1
fig1, ax1 = plt.subplots()
fig1.subplots_adjust(bottom=0.01,) # or whatever
_, texts, autotexts = ax1.pie(sizes, explode=explode, labels=labels, colors=colors1, autopct='%1.2f%%',
        shadow=False, startangle=90)
pctdists = [0.9, 0.6, 0.6, 0.6, 0.6, 0.6, 0.7]
for autotext,d in zip(autotexts, pctdists):
    autotext.set_color('white')
    xi,yi = autotext.get_position()
    ri = np.sqrt(xi**2+yi**2)
    phi = np.arctan2(yi,xi)
    x = d*ri*np.cos(phi)
    y = d*ri*np.sin(phi)
    autotext.set_position((x,y))
    autotext.set_fontsize(12)
for text in texts:
    text.set_fontsize(14)
ax1.axis('equal',fontsize=32)  # Equal aspect ratio ensures that pie is drawn as a circle.

plt.show()