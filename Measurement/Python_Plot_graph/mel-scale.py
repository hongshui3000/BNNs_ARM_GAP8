import numpy as np
from numpy.fft import fft, fftshift
import math as math
from matplotlib import pyplot as plt
window = np.hamming(512)
xx = np.arange(0, 10000, 1)
sing = np.random.rand(512)- 0.5
sig_after = sing*window
A = fft(sig_after, 512)
#mag = np.abs(fftshift(A))
#freq = np.linspace(-0.5, 0.5, len(A))
#response = 20 * np.log10(mag)
#plt.plot(freq, response)
#xx = np.arange(1, 512, 1)
#xxx = np.arange(1, 512, 1)
#yy = 0.54 - 0.46*np.cos(2*math.pi*xx/511) # 0.1 G MAC/s
yy = 1125*np.log(1+xx/700)
f1 = 440.0
f2 = 880.0
f3 = 1760.0
m1 = 1125*math.log(1+f1/700)
m2 = 1125*math.log(1+f2/700)
m3 = 1125*math.log(1+f3/700)
print("Freq:" + str(f1) + ". Mel:" + str(m1))
print("Freq:"+str(f2)+". Mel:"+str(m2))
print("Freq:"+str(f3)+". Mel:"+str(m3))
#yyy = np.multiply(xxx,(0.54 - 0.46*np.cos(2*math.pi*xx/511))) # 0.1 G MAC/s
#plt.xlabel('Frequency bin',fontsize=12)
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.xlabel('Frequency (Hz)',fontsize=12)
plt.ylabel('Mel-Scale',fontsize=12)
#plt.plot(xx, sing, '-', color='gray')
plt.plot(xx, yy) #spectrum
#plt.plot(xxx, yyy, '.', color='gray')
#plt.show()
#plt.xlabel('Samples position',fontsize=12)
#plt.ylabel('Amplitude',fontsize=12)
#plt.plot(window)
#plt.show()
#plt.xlabel('Samples position',fontsize=12)
#plt.ylabel('Amplitude',fontsize=12)
#plt.plot(xx, sig_after, '-', color='gray')
#plt.show()

