import numpy as np
from matplotlib import pyplot as plt

#x = [0.084308, 0.0825552, 0.14898324,0.149215763,0.1467704,0.1461488]
x = [0.084308, 0.0825552, 0.188, 0.188]
xx = np.arange(0.001,2.001,.005)
yy = xx*(10**(-1)) # 0.1 G MAC/s
yyy = xx*(10**0) # 1 G MAC/s
yyyy = xx*(10**1) # 10 G MAC/s
#y = [54.44,	233.10,	983.48,	1031.89, 1784.90, 2217.74] #M MAC/s
y = [0.05444, 0.23310, 1.074, 2.115] #G MAC/s

plt.loglog(x,y, 'ro')
plt.loglog(xx, yy, '-.', color='gray')
plt.loglog(xx, yyy, '-.', color='gray')
plt.loglog(xx, yyyy, '-.', color='gray')
plt.text(0.092308, 0.05044, r'fBNN', fontsize=12)
plt.text(0.0905552, 0.23010, r'xBNN', fontsize=12)
plt.text(0.210215763, 0.93189, r'Par(only BNN)', fontsize=12)
plt.text(0.21071488, 1.85660, r'Parf(only BNN)', fontsize=12)

plt.text(0.01, 0.003, r'0.1 G MAC/J', color='gray',rotation=20, fontsize=12)
plt.text(0.01, 0.03, r'1 G MAC/J', color='gray',rotation=20, fontsize=12)
plt.text(0.01, 0.3, r'10 G MAC/J', color='gray',rotation=20, fontsize=12)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.xlabel('Power (W)',fontsize=12)
plt.ylabel('Throughput (G MAC/s)',fontsize=12)
plt.xlim(0.008, 1)
plt.ylim(0.0006, 11 )
#plt.plot([0.2145, 0.2409, 0.2586, 0.2574],[72.32*1000000, 119.18*1000000, 143.23*1000000, 118.28*1000000],'ro')
plt.grid(True,which="both",ls="-")
plt.show()