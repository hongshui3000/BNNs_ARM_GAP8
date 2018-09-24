Files structure:

main.py:	configure hyperparameter, data path, learning rate, epoch, optimizer, initialization, loss function
models/nin.py:	Network structure configuration
data.py		read data, data path
util.py		BNNs configuration

-------------------------------------------------------
These scripts are used to train 28 sound events
-------------------------------------------------------

dataset: https://data.vision.ee.ethz.ch/cvl/ae_dataset/

-------------------------------------------------------
Data preprocessing needed for training
-------------------------------------------------------

The input data is the MFCC spectrogram of the dataset.

See file for further information: /ARM_XNOR/preprocess/mfcc_calculation.cpp

The Aquila DSP library is required for the preprocessing

--After generating MFCC files which are thousands of .csv files, please put them into

	{BNNs Training script}/data/TrainOutF_Trunc/ <<--for Training dataset

	{BNNs Training script}/data/TestOutF_Trunc/ <<--for Testing dataset

Generate a filename_class.csv that contains the name of the files for training and testing dataset separately:
{BNNs Training script}/data/TrainOutF_Trunc/filename_class_train.csv
Ex.:
10_8109.csv,10
10_8110.csv,10
10_8111.csv,10
...
...
(name of files, class)

Ex.:
{BNNs Training script}/data/TestOutF_Trunc/filename_class_test.csv

10_2702.csv,10
10_2703.csv,10
10_2704.csv,10
...
...
(name of files, class)

-------------------------------------------------------
How to use
-------------------------------------------------------
Install PyTorch on:   https://pytorch.org/
I use Anaconda3 as package control
--------Version:-----------
	Python 3.6
	PyTorch version v0.3.1. 
	CUDA 8 
---------------------------
-------------------------------------------------------
(default: when Nvidia GPU available)

	pytohn3 main.py

(For CPU user)

	pytohn3 main.py --cpu
