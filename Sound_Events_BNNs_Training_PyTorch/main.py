from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import os
import torch
import argparse
import data
import pandas as pd
import util
import numpy as np
import torch.nn as nn
import torch.optim as optim
from torchvision import transforms, datasets

from models import nin
from torch.autograd import Variable

def save_state(model, best_acc):
    print('==> Saving model ...')
    state = {
            'best_acc': best_acc,
            'state_dict': model.state_dict(),
            }
    #for key in state['state_dict'].keys():
    for key in model.state_dict().keys():
        if 'module' in key:
            state['state_dict'][key.replace('module.', '')] = \
                    state['state_dict'].pop(key)
    torch.save(state, 'models/nin.pth.tar')

def train(epoch):
    model.train()
    for batch_idx, (data, target) in enumerate(trainloader):
        # process the weights including binarization
        bin_op.binarization()
        
        # forwarding
        data, target = Variable(data), Variable(target)
        #print (data, target)
        #print(data.size(), target.size())
        optimizer.zero_grad()
        output = model(data)
        output = output.cpu()
        # backwarding
        loss = criterion(output, target)
        loss.backward()
        
        # restore weights
        bin_op.restore()
        bin_op.updateBinaryGradWeight()
        
        optimizer.step()
        if batch_idx % 100 == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}\tLR: {}'.format(
                epoch, batch_idx * len(data), len(trainloader.dataset),
                100. * batch_idx / len(trainloader), loss.data[0],
                optimizer.param_groups[0]['lr']))
    return

def test():
    global best_acc
    model.eval()
    test_loss = 0
    correct = 0
    bin_op.binarization()
    for data, target in testloader:
        data, target = Variable(data), Variable(target)
                                    
        output = model(data)
        output = output.cpu()
        test_loss += criterion(output, target).data[0]
        pred = output.data.max(1, keepdim=True)[1]
        correct += pred.eq(target.data.view_as(pred)).cpu().sum()
    bin_op.restore()
    global acc
    acc = 100. * correct / len(testloader.dataset)

    if acc > best_acc:
        best_acc = acc
        save_state(model, best_acc)
    
    test_loss /= len(testloader.dataset)
    print('\nTest set: Average loss: {:.4f}, Accuracy: {}/{} ({:.2f}%)'.format(
        test_loss * 128., correct, len(testloader.dataset),
        100. * correct / len(testloader.dataset)))
    print('Best Accuracy: {:.2f}%\n'.format(best_acc))
    return

def test_on_traindata():
    global best_train_acc
    model.eval()
    test_train_loss = 0
    train_correct = 0
    bin_op.binarization()
    for data, target in trainloader:
        data, target = Variable(data), Variable(target)
                                    
        output = model(data)
        output = output.cpu()
        test_train_loss += criterion(output, target).data[0]
        pred = output.data.max(1, keepdim=True)[1]
        train_correct += pred.eq(target.data.view_as(pred)).cpu().sum()
    bin_op.restore()
    global train_acc
    train_acc = 100. * train_correct / len(trainloader.dataset)

    if train_acc > best_train_acc:
        best_train_acc = train_acc
        #save_state(model, best_train_acc)
    
    test_train_loss /= len(trainloader.dataset)
    print('\nTest on Trainset: Average loss: {:.4f}, Accuracy: {}/{} ({:.2f}%)'.format(
        test_train_loss * 128., train_correct, len(trainloader.dataset),
        100. * train_correct / len(trainloader.dataset)))
    print('Best Train Accuracy: {:.2f}%\n'.format(best_train_acc))
    return

def adjust_learning_rate(optimizer, epoch):
    update_list = [8]
    if epoch in update_list:
        for param_group in optimizer.param_groups:
            param_group['lr'] = param_group['lr'] * 0.01
    return

if __name__=='__main__':
    # prepare the options
    parser = argparse.ArgumentParser()
    parser.add_argument('--cpu', action='store_true',
            help='set if only CPU is available')
    parser.add_argument('--data', action='store', default='./data',
            help='dataset path')
    parser.add_argument('--arch', action='store', default='nin',
            help='the architecture for the network: nin')
    parser.add_argument('--lr', action='store', default='0.01',
            help='the intial learning rate')
    parser.add_argument('--pretrained', action='store', default=None,
            help='the path to the pretrained model')
    parser.add_argument('--evaluate', action='store_true',
            help='evaluate the model')
    args = parser.parse_args()
    print('==> Options:',args)

    # set the seed
    torch.manual_seed(1)
    torch.cuda.manual_seed(1)

    # prepare the data
    if not os.path.isfile(args.data+'/TrainOutF_Trunc'):
        # check the data path
        raise Exception\
                ('Please assign the correct data path with --data <DATA_PATH>')
    #read filnames and class label
    trainset = data.dataset(csv_file='./data/TrainOutF_Trunc/filename_class_train.csv',root_dir=args.data, train=True)
    trainloader = torch.utils.data.DataLoader(trainset, batch_size=64,
            shuffle=True, num_workers=2)

    testset = data.dataset(csv_file='./data/TestOutF_Trun_No256/filename_class_test.csv',root_dir=args.data, train=False)
    testloader = torch.utils.data.DataLoader(testset, batch_size=32,
            shuffle=True, num_workers=2)
            #shuffle=False, num_workers=2)

    # define classes
    classes = ('acoustic_guitar', 'airplane', 'applause', 'bird',
            'car', 'cat', 'child', 'church_bell', 'crowd','dog_barking',
            'engine','fireworks','footstep','glass_breaking','hammer',
            'helicopter','knock','laughter','mouse_click','ocean_surf',
            'rustle','scream','speech','squeak','tone','violin','water_tap','whistle')

    # define the model
    print('==> building model',args.arch,'...')
    if args.arch == 'nin':
        model = nin.Net()
    else:
        raise Exception(args.arch+' is currently not supported')

    # initialize the model
    if not args.pretrained:
        print('==> Initializing model parameters ...')
        best_acc = 0
        best_train_acc = 0
        for m in model.modules():
            if isinstance(m, nn.Conv2d):
                m.weight.data.normal_(0, 1)
                m.bias.data.zero_()
    else:
        print('==> Load pretrained model form', args.pretrained, '...')
        pretrained_model = torch.load(args.pretrained)
        best_acc = pretrained_model['best_acc']
        model.load_state_dict(pretrained_model['state_dict'])

    if not args.cpu:
        model.cuda()
        model = torch.nn.DataParallel(model, device_ids=range(torch.cuda.device_count()))
    print(model)

    # define solver and criterion
    base_lr = float(args.lr)
    param_dict = dict(model.named_parameters())
    params = []
    # select optimizer
    for key, value in param_dict.items():
        params += [{'params':[value], 'lr': base_lr,'weight_decay':0.00001}]

        #optimizer = optim.Adam(params, lr=0.10,weight_decay=0.00001)
        optimizer = optim.SGD(params, lr=0.01, momentum=0.9)
    criterion = nn.CrossEntropyLoss()

    # define the binarization operator
    bin_op = util.BinOp(model)

    # do the evaluation if specified
    if args.evaluate:
        test()
        exit(0)
    # record accuracy
    with open("test_acc_f16.csv", "w") as dest:
        with open("train_acc_f16.csv", "w") as dest2:
    # start training
            for epoch in range(1, 61):
                adjust_learning_rate(optimizer, epoch)
                train(epoch)
                test_on_traindata()
                test()
                dest2.write(str(epoch)+','+str(train_acc)+'\n')
                dest.write(str(epoch)+','+str(acc)+'\n')
    dest2.close()
    dest.close()
