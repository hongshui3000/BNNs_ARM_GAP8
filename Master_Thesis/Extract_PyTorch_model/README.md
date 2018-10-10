---------------------------------------------------------
1. Setting up
---------------------------------------------------------
Learning rate:
	1~20 Epoch :   0.01
	21~40 Epoch:   0.0001
	41~60 Epoch:   0.000001

Batch size: 64

Initialization:
	weigt.data.normal(0,1)
	bias.data.zero_()

Optimizer:
	SGD(lr=0.1, momentum=0.9)

Criterion:
	nn.CrossEntropyLoss()

Network:
        nn.Conv2d(1, 32, kernel_size=3, stride=1, padding=1), 
        nn.BatchNorm2d(32, eps=1e-5, momentum=0.1, affine=True), 
        nn.ReLU(inplace=True), 
        BinConv2d(32, 64, kernel_size=3, stride=2, padding=1), 
        BinConv2d(64,  128, kernel_size=3, stride=1, padding=1), 
        BinConv2d(128, 128, kernel_size=3, stride=2, padding=1, dropout=0.5), 
        BinConv2d(128, 128, kernel_size=3, stride=1, padding=1), 
        BinConv2d(128, 128, kernel_size=1, stride=1, padding=0),#last layer 
        nn.BatchNorm2d(128, eps=1e-3, momentum=0.1, affine=True), 
        nn.Conv2d(128,  28, kernel_size=1, stride=1, padding=0), 
        nn.AvgPool2d(kernel_size=(16,100), stride=1, padding=0), 
                

---------------------------------------------------------
2. How to use
---------------------------------------------------------
The script must match the architecture for the training so that the model.pth.tar can be extracted.
This script will extract the model.pth.tar to C readable .dat file.
The alpha for scaling is calculated by this script using Rastegart XNOR-Net approximation. (L1 norm)

Move your trained model from 
/Sound_Events_BNNs_Training_PyTorch/models/model.pth.tar

to 
/Extract_PyTorch_model/models/model.pth.tar

and command the following in terminal(PyTorch need to be installed)

$ python main.py --pretrained ./trained_model.pth.tar 
