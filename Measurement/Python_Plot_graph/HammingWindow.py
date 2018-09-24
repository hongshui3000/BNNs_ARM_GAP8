import numpy as np
from numpy.fft import fft, fftshift
import math as math
from matplotlib import pyplot as plt
window = np.hamming(512)
xx = np.arange(0, 512, 1)
sing = np.random.rand(512)- 0.5
sig_after = sing*window
A = fft(sig_after, 512)
#mag = np.abs(fftshift(A))
#freq = np.linspace(-0.5, 0.5, len(A))
#response = 20 * np.log10(mag)
#plt.plot(freq, response)
#xx = np.arange(1, 512, 1)
#xxx = np.arange(1, 512, 1)
yy = 0.54 - 0.46*np.cos(2*math.pi*xx/511) # 0.1 G MAC/s
#yyy = np.multiply(xxx,(0.54 - 0.46*np.cos(2*math.pi*xx/511))) # 0.1 G MAC/s
#plt.xlabel('Frequency bin',fontsize=12)
plt.xlabel('Samples position [n]',fontsize=12)
plt.ylabel('Amplitude w[n]',fontsize=12)
#plt.plot(xx, sing, '-', color='gray')
plt.plot(xx, yy) #spectrum
#plt.plot(xxx, yyy, '.', color='gray')
plt.show()
#plt.xlabel('Samples position',fontsize=12)
#plt.ylabel('Amplitude',fontsize=12)
#plt.plot(window)
#plt.show()
#plt.xlabel('Samples position',fontsize=12)
#plt.ylabel('Amplitude',fontsize=12)
#plt.plot(xx, sig_after, '-', color='gray')
#plt.show()

