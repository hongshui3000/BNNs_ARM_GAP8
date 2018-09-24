import numpy as np
from matplotlib import pyplot as plt

x = [0.2145, 0.2409, 0.2586, 0.2574]
xx = np.arange(0.1,1.01,.005)
yy = xx*(10**8) # 1 M MAC/s
yyy = xx*(10**9) # 10 G MAC/s
y = [72.32*1000000, 119.18*1000000, 143.23*1000000, 118.28*1000000]

plt.loglog([0.2145, 0.2409, 0.2586, 0.2574],[72.32*1000000, 119.18*1000000, 143.23*1000000, 118.28*1000000], 'ro')
plt.loglog(xx, yy, '--')
plt.loglog(xx, yyy, '--')
plt.text(0.2145, 55.32*1000000, r'No Opt.', fontsize=12)
plt.text(0.1845, 110.32*1000000, r'Opt-F', fontsize=12)
plt.text(0.2586, 155.32*1000000, r'Opt-FI', fontsize=12)
plt.text(0.2674, 106.32*1000000, r'Opt-FIR', fontsize=12)
#plt.plot([0.2145, 0.2409, 0.2586, 0.2574],[72.32*1000000, 119.18*1000000, 143.23*1000000, 118.28*1000000],'ro')
plt.grid(True,which="both",ls="-")
plt.show()