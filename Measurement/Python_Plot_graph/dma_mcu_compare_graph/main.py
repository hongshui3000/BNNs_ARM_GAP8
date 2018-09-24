import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd
import numpy as np
import csv

x = [' First','B1.1','B1.2','B2.1','B2.2','B2.3','B2.4','B3.1','B3.2','B3.3','B3.4','B4.1','B4.2','B4.3','B4.4','Last']
y_dma = [732,12265,638,798,798,798,798,1178,1178,1194,1198,1194,1198,1198,1198,3584]
y_mcu = [17617,24421,12809,21449,21449,21449,21449,38749,38749,38755,38759,38755,38759,38759,38759,147253]
y_data = [2304,1664,1664,2816,2816,2816,2816,5120,5120,5120,5120,5120,5120,5120,5120,19568]

plt.plot(x, y_dma,label='DMA')
plt.plot(x, y_mcu,label='MCU')
plt.xticks(x)
plt.xlabel('Layer',fontsize=16)
plt.ylabel('Cycles ',fontsize=16)
#plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()
