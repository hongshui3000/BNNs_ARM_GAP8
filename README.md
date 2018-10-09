# Topic: Sound Events Classification using BNNs on Tightly Power-Restricted IoT Devices
This is the source code of my thesis at ETH Zurich.
## File Organization
Folder:
	Measurement				: The results of optimized binary network in ARM and GAP8.
	ARM_XNOR				: XNOR Network on ARM Cortex-M4 STM32F469NI discovery board
	gap8					: XNOR Network on GAP8
	fulmine_xnor				: XNOR Network on Fulmine (pulp SDK does not support anymore)
	Sound_Events_BNNs_Training_PyTorch	:Training BNNs for classifying 28 sound events with dataset from computer vision group (https://data.vision.ee.ethz.ch/cvl/ae_dataset/)
	Extract_PyTorch_model			: Convert the model data trained with PyTorch into C/C++ readable form.
	Report          		        : Sound-Event Classification using BNNs in IoT
	Presentation       			: final power point
	MFCC_preprocessing		        : Generating the MFCCs feature maps from .wav data. Please select dataset path and generate a file list
-----------------------------

Yout will see more details in individual folder "README.md"
	
	
