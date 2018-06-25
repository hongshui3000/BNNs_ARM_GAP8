import glob
#import cv2
from PIL import Image
import numpy as np

img = []
dest = []
for i in range(28):
    files = glob.glob('output/' + str(i+1) + '/*.csv')
    for f in files:
        img.append(np.loadtxt(f, delimiter=';'))
        dest.append('image' + f[6:-3] + 'png')

img = np.array(img).astype(np.uint8)

for i in range(img.shape[0]):
    aux = Image.fromarray(img[i])
    aux.save(dest[i])

mean = np.mean(img, axis=0)
std = np.std(img, axis=0)

aux = Image.fromarray(mean.astype(np.uint8))
aux.save('mean.png')

aux = Image.fromarray(std.astype(np.uint8))
aux.save('std.png')
