import numpy as np
from matplotlib import pyplot as plt

x = [0.217, 0.242, 0.249, 0.247] #0.08*3.3
xx = np.arange(0.1,1.01,.005)
yy = xx*(10**2) # 0.1 G MAC/s
yyy = xx*(10**3) # 1 G MAC/s
y = [74.32, 119.17, 138.02, 150.89]

plt.loglog(x,y, 'ro')
plt.loglog(xx, yy, '-.', color='gray')
plt.loglog(xx, yyy, '-.', color='gray')
plt.text(0.15, 27, r'0.1 G MAC/J', color='gray',rotation=20, fontsize=12)
plt.text(0.15, 250, r'1 G MAC/J', color='gray',rotation=20, fontsize=12)

plt.text(0.2145, 55.32, r'No Opt.', fontsize=12)
plt.text(0.1845, 110.32, r'Opt-F', fontsize=12)
plt.text(0.2526, 155.32, r'Opt-FIR', fontsize=12)
plt.text(0.2574, 126.32, r'Opt-FI', fontsize=12)
plt.xlabel('Power (W)',fontsize=12)
plt.ylabel('Throughput (M MAC/s)',fontsize=12)
plt.xlim(0.1, 1)
plt.ylim(10, 1000 )

#plt.plot([0.2145, 0.2409, 0.2586, 0.2574],[72.32*1000000, 119.18*1000000, 143.23*1000000, 118.28*1000000],'ro')
plt.grid(True,which="both",ls="-")
plt.show()