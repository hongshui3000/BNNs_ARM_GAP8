import numpy
import scipy.io.wavfile
from scipy.fftpack import dct
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.image as mpimg
import math
from random import randint
import numpy as np

plt.figure(figsize=(5.5,3))
plt.gcf().subplots_adjust(bottom=0.15,left=0.1,right=0.95)

NFFT = 512
nfilt = 24
mel_freq_delta = 200
sample_rate = 16000
fbank = numpy.zeros((nfilt, 3))
index = [0,1,0]
x = np.arange(10)
ys = [i+x+(i*x)**2 for i in range(24)]

colors = cm.rainbow(np.linspace(0, 1, len(ys)))

for bin in range(0, nfilt):
    left_mel = bin*mel_freq_delta/2.0
    center_mel = left_mel + mel_freq_delta / 2.0
    right_mel = left_mel + mel_freq_delta
    low_freq = 700.0 * (math.exp(left_mel / 1127.0) - 1.0) #meltoLinear
    center_freq = 700.0 * (math.exp(center_mel / 1127.0) - 1.0) #meltoLinear
    high_freq = 700.0 * (math.exp(right_mel / 1127.0) - 1.0)  #meltoLinear
    mel_points = numpy.linspace(left_mel, right_mel, nfilt + 2)  # Equally spaced in Mel scale
    fbank[bin, 0] = low_freq
    fbank[bin, 1] = center_freq
    fbank[bin, 2] = high_freq
for bin in range(0, nfilt):
    plt.plot(fbank[bin], index,colors[bin],linewidth=0.5, alpha=1.0)
    plt.plot(fbank[bin], index,colors[bin],linewidth=0.5, alpha=1.0)
    plt.plot(fbank[bin], index,colors[bin],linewidth=0.5, alpha=1.0)
plt.grid(color='gray', alpha=0.4, linestyle='-', linewidth=0.4)
plt.xlabel('Frequency (Hz)',fontsize=12)
plt.ylabel('Amplitude',fontsize=12)

#print(fbank)
plt.show()

#filter_banks = 20 * numpy.log10(filter_banks)  # dB
#plt.imshow(bin)


