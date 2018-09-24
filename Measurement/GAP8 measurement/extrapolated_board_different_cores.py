import numpy as np
import matplotlib.pyplot as plt

x = [50,100,150] #current 1 , 2 ,4 cores
y = [10.767,23.983,39.1912] # 10, not 9, so the fit isn't perfect
xx = [50,100,150,250]
yy = [10.767,23.983,39.1912,67.28] # 10, not 9, so the fit isn't perfect

fit = np.polyfit(x,y,1)
m, b = np.polyfit(x,y,1)
fit_fn = np.poly1d(fit) 
# fit_fn is now a function which takes in x and returns an estimate for y
print(m)
print(b)
F_150 = m*150 + b
F_250 = m*250 + b
print(F_150)
print(F_250)
print(fit_fn(x))
#plt.plot(x,y, 'yo', x, fit_fn(x), 'k')
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.plot(x,y, 'bo', xx, fit_fn(xx), 'k')
plt.text(230, 70.28, r'67.28 mA', fontsize=12)
plt.plot(250,67.28, 'rx')
plt.xlabel('Frequency (MHz)',fontsize=12)
plt.ylabel('Current (mA)',fontsize=12)
plt.xlim(0, 300)
plt.ylim(0, 80)
plt.show()