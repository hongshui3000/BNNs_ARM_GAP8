import torch.nn as nn
import torch
import torch.nn.functional as F

class BinActive(torch.autograd.Function):
    '''
    Binarize the input activations and calculate the mean across channel dimension.
    '''
    def forward(self, input):
        self.save_for_backward(input)
        size = input.size()
        mean = torch.mean(input.abs(), 1, keepdim=True)
        input = input.sign()
        return input, mean

    def backward(self, grad_output, grad_output_mean):
        input, = self.saved_tensors
        grad_input = grad_output.clone()
        grad_input[input.ge(1)] = 0
        grad_input[input.le(-1)] = 0
        return grad_input

class BinConv2d(nn.Module):
    def __init__(self, input_channels, output_channels,
            kernel_size=-1, stride=-1, padding=-1, dropout=0):
        super(BinConv2d, self).__init__()
        self.layer_type = 'BinConv2d'
        self.kernel_size = kernel_size
        self.stride = stride
        self.padding = padding
        self.dropout_ratio = dropout

        self.bn = nn.BatchNorm2d(input_channels, eps=1e-4, momentum=0.1, affine=False)
        if dropout!=0:
            self.dropout = nn.Dropout(dropout)
        self.conv = nn.Conv2d(input_channels, output_channels,
                kernel_size=kernel_size, stride=stride, padding=padding)
        self.relu = nn.ReLU(inplace=True)
    
    def forward(self, x):
        x = self.bn(x)
        x, mean = BinActive()(x)
        if self.dropout_ratio!=0:
            x = self.dropout(x)
        x = self.conv(x)
        x = self.relu(x)
        return x

class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.xnor = nn.Sequential(
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
                #nn.ReLU(inplace=True),
                nn.AvgPool2d(kernel_size=(16,100), stride=1, padding=0),
                #nn.LogSoftmax(),
                )

    def forward(self, x):
        x = self.xnor(x)
        #print (x.size())
        x = x.view(x.size(0), 28)
        #print (x.size())
        #m = nn.LogSoftmax(dim=1)
        #x = m(x)
        return x
