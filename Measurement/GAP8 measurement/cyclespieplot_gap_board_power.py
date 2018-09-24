import numpy as np
import matplotlib.pyplot as plt
#plt.figure(figsize=(5.5,3))
#fig, ax = plt.subplots(figsize=(6, 3), subplot_kw=dict(aspect="equal"))
#plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.95)

# Pie chart, where the slices will be ordered and plotted counter-clockwise:
#sizes = [188.4 , 49.9, 17.5, 72.5] #running bnn
#labels = "GAP8 Processor\n (188.4 mW)",'GAPuino board \n (49.9 mW)', 'Sensor board \n (17.5 mW)','Recording \n (72.5 mW)'
sizes = [47.6 , 49.9, 17.5, 72.5] # only recording, gap8 processor waiting
labels = "GAP8 Processor\n (47.6 mW)",'GAPuino board \n (49.9 mW)', 'Sensor board \n (17.5 mW)','Recording \n (72.5 mW)'

colors1 = 'blue', 'red','gray','orange'
#sizes = [15, 30, 45, 10]
explode = (0.01, 0.01, 0.01, 0.01)  # only "explode" the 2nd slice (i.e. 'Hogs')

fig1, ax1 = plt.subplots()
fig1.subplots_adjust(bottom=0.04,) # or whatever
_, texts, autotexts = ax1.pie(sizes, explode=explode, labels=labels, colors=colors1, autopct='%1.2f%%',
        shadow=False, startangle=0)
for autotext in autotexts:
    autotext.set_color('white')
    autotext.set_fontsize(12)
for text in texts:
    text.set_fontsize(14)
ax1.axis('equal',fontsize=32)  # Equal aspect ratio ensures that pie is drawn as a circle.

plt.show()