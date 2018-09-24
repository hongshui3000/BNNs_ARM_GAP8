from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import os
import torch
import argparse
#import data
import pandas as pd
import csv
import util
import numpy as np
import torch.nn as nn
import torch.optim as optim
import torch.utils.data as data
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


def adjust_learning_rate(optimizer, epoch):
    update_list = [12, 24, 36, 46]
    if epoch in update_list:
        for param_group in optimizer.param_groups:
            param_group['lr'] = param_group['lr'] * 0.1
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
        for m in model.modules():
            if isinstance(m, nn.Conv2d):
                m.weight.data.normal_(0, 1)
                #m.bias.data.zero_()
    else:
        print('==> Load pretrained model form', args.pretrained, '...')
        pretrained_model = torch.load(args.pretrained)
        best_acc = pretrained_model['best_acc']
        model.load_state_dict(pretrained_model['state_dict'])

    if not args.cpu:
        model.cuda()
        model = torch.nn.DataParallel(model, device_ids=range(torch.cuda.device_count()))
    print(model)
    bias_9 = pretrained_model['state_dict']
    print_acc = pretrained_model['best_acc']
    print(print_acc)
    #print(bias_9['xnor.0.bias'])
    print(pretrained_model)
    first = True
    # write for C++
    with open("model_mydata_07_12_Win256.dat", "w") as dest:
#---------------------------fData------------------------------------------
        dest.write('float fData[] = {')
        for bias0 in bias_9['xnor.0.bias'].cpu().numpy():
            dest.write(str(f'{bias0:.6f}')+',')    #32, bias = 0
        #for bias0 in range(0,32):
        #    dest.write('0.000000'+',')    #32, bias = 0
        for weight0_output in bias_9['xnor.0.weight'].cpu().numpy():
            for weight0_input in weight0_output:
                dest.write(str(f'{weight0_input[0][0]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[0][1]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[0][2]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[1][0]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[1][1]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[1][2]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[2][0]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[2][1]:.6f}')+',')    #32x1x3x3
                dest.write(str(f'{weight0_input[2][2]:.6f}')+',')    #32x1x3x3
        for mean1 in bias_9['xnor.1.running_mean'].cpu().numpy():	#mean1
            dest.write(str(f'{mean1:.6f}')+',')    #32
        for var1 in bias_9['xnor.1.running_var'].cpu().numpy():		#var1 = 1/std
            dest.write(str(f'{var1:.6f}')+',')    #32
        for gamma1 in bias_9['xnor.1.weight'].cpu().numpy():		#bn gamma
            dest.write(str(f'{gamma1:.6f}')+',')    #32
        for beta1 in bias_9['xnor.1.bias'].cpu().numpy():		#bn beta
            dest.write(str(f'{beta1:.6f}')+',')    #32
        for mean3 in bias_9['xnor.3.bn.running_mean'].cpu().numpy(): #mean2
            dest.write(str(f'{mean3:.6f}')+',')    #32
        for var3 in bias_9['xnor.3.bn.running_var'].cpu().numpy():#var2 = 1/std
            dest.write(str(f'{var3:.6f}')+',')    #32
        for weight3 in bias_9['xnor.3.bn.weight'].cpu().numpy(): # bn gamma2
            dest.write(str(f'{weight3:.6f}')+',')    #32
        for beta3 in bias_9['xnor.3.bn.bias'].cpu().numpy():     #bn beta2
            dest.write(str(f'{beta3:.6f}')+',')    #32
#---------------------------layer B1----------------------------------------#

        for bias3 in bias_9['xnor.3.conv.bias'].cpu().numpy(): #64 bias
            dest.write(str(f'{bias3:.6f}')+',')    #64
##########################
###calaulate BIN_1 alpha here
##########################
        for weight3_output in bias_9['xnor.3.conv.weight'].cpu().numpy(): #64x32x3x3
            sum_temp = []
            for weight3_input in weight3_output:
                for weight3_Y in weight3_input:
                    for weight3_X in weight3_Y:
                        #print (abs(weight3_X))
                        sum_temp.append(abs(weight3_X))
            alpha = sum(sum_temp)/(32*3*3)
            dest.write(str(f'{alpha:.6f}')+',')    #64
#########################
#########################
        for mean4 in bias_9['xnor.4.bn.running_mean'].cpu().numpy():# bn mean
            dest.write(str(f'{mean4:.6f}')+',')    #64
        for var4 in bias_9['xnor.4.bn.running_var'].cpu().numpy(): # bn var
            dest.write(str(f'{var4:.6f}')+',')    #64
        for gamma4 in bias_9['xnor.4.bn.weight'].cpu().numpy(): # bn gamma
            dest.write(str(f'{gamma4:.6f}')+',')    #64
        for beta4 in bias_9['xnor.4.bn.bias'].cpu().numpy():     # bn beta
            dest.write(str(f'{beta4:.6f}')+',')    #64
#---------------------------layer B2----------------------------------------#

        for bias4 in bias_9['xnor.4.conv.bias'].cpu().numpy():
            dest.write(str(f'{bias4:.6f}')+',')    #128
##########################
###calaulate BIN_2 alpha here
##########################
        for weight4_output in bias_9['xnor.4.conv.weight'].cpu().numpy(): #128x64x3x3
            sum_temp = []
            for weight4_input in weight4_output:
                for weight4_Y in weight4_input:
                    for weight4_X in weight4_Y:
                        #print (abs(weight4_X))
                        sum_temp.append(abs(weight4_X))
            alpha = sum(sum_temp)/(64*3*3)
            dest.write(str(f'{alpha:.6f}')+',')    #128
#########################
#########################
        for mean5 in bias_9['xnor.5.bn.running_mean'].cpu().numpy():
            dest.write(str(f'{mean5:.6f}')+',')    #128
        for var5 in bias_9['xnor.5.bn.running_var'].cpu().numpy():
            dest.write(str(f'{var5:.6f}')+',')    #128
        for gamma5 in bias_9['xnor.5.bn.weight'].cpu().numpy(): # bn gamma
            dest.write(str(f'{gamma5:.6f}')+',')    #128
        for beta5 in bias_9['xnor.5.bn.bias'].cpu().numpy():     #bn beta
            dest.write(str(f'{beta5:.6f}')+',')    #128
#---------------------------layer B3----------------------------------------#

        for bias5 in bias_9['xnor.5.conv.bias'].cpu().numpy():
            dest.write(str(f'{bias5:.6f}')+',')    #128
##########################
###calaulate BIN_3 alpha here
##########################
        for weight5_output in bias_9['xnor.5.conv.weight'].cpu().numpy(): #128x128x3x3
            sum_temp = []
            for weight5_input in weight5_output:
                for weight5_Y in weight5_input:
                    for weight5_X in weight5_Y:
                        #print (abs(weight5_X))
                        sum_temp.append(abs(weight5_X))
            alpha = sum(sum_temp)/(128*3*3)
            dest.write(str(f'{alpha:.6f}')+',')    #128
#########################
#########################
        for mean6 in bias_9['xnor.6.bn.running_mean'].cpu().numpy(): #bn mean
            dest.write(str(f'{mean6:.6f}')+',')    #128
        for var6 in bias_9['xnor.6.bn.running_var'].cpu().numpy(): #bn var
            dest.write(str(f'{var6:.6f}')+',')    #128
        for gamma6 in bias_9['xnor.6.bn.weight'].cpu().numpy(): # bn gamma
            dest.write(str(f'{gamma6:.6f}')+',')    #128
        for beta6 in bias_9['xnor.6.bn.bias'].cpu().numpy():     #bn beta
            dest.write(str(f'{beta6:.6f}')+',')    #128
#---------------------------layer B4----------------------------------------#
        for bias6 in bias_9['xnor.6.conv.bias'].cpu().numpy():
            dest.write(str(f'{bias6:.6f}')+',')    #128
##########################
###calaulate BIN_4 alpha here
##########################
        for weight6_output in bias_9['xnor.6.conv.weight'].cpu().numpy(): #128x128x3x3
            sum_temp = []
            for weight6_input in weight6_output:
                for weight6_Y in weight6_input:
                    for weight6_X in weight6_Y:
                        #print (abs(weight6_X))
                        sum_temp.append(abs(weight6_X))
            alpha = sum(sum_temp)/(128*3*3)
            dest.write(str(f'{alpha:.6f}')+',')    #128
#########################
#########################
        for mean7 in bias_9['xnor.7.bn.running_mean'].cpu().numpy(): # bn mean
            dest.write(str(f'{mean7:.6f}')+',')    #128
        for var7 in bias_9['xnor.7.bn.running_var'].cpu().numpy(): # bn var
            dest.write(str(f'{var7:.6f}')+',')    #128
        for gamma7 in bias_9['xnor.7.bn.weight'].cpu().numpy(): # bn gamma
            dest.write(str(f'{gamma7:.6f}')+',')    #128
        for beta7 in bias_9['xnor.7.bn.bias'].cpu().numpy():     #bn beta
            dest.write(str(f'{beta7:.6f}')+',')    #128
#---------------------------Last Layer----------------------------------------#

        for bias7 in bias_9['xnor.7.conv.bias'].cpu().numpy(): #Last Layer bias1
            dest.write(str(f'{bias7:.6f}')+',')    #128
##########################
###calaulate alpha here
##########################
        for weight7_output in bias_9['xnor.7.conv.weight'].cpu().numpy(): #128x128x1x1
            sum_temp = []
            for weight7_input in weight7_output:
                for weight7_Y in weight7_input:
                    for weight7_X in weight7_Y:
                        #print (abs(weight7_X))
                        sum_temp.append(abs(weight7_X))
            alpha = sum(sum_temp)/128
            dest.write(str(f'{alpha:.6f}')+',')    #128
        #print(str(f'{alpha:.6f}'))
#########################
#########################
        for mean8 in bias_9['xnor.8.running_mean'].cpu().numpy():#Last Layer mean
            dest.write(str(f'{mean8:.6f}')+',')    #128
        for var8 in bias_9['xnor.8.running_var'].cpu().numpy():#Last Layer bn std
            dest.write(str(f'{var8:.6f}')+',')    #128
        for gamma8 in bias_9['xnor.8.weight'].cpu().numpy(): #Last Layer bn gamma
            dest.write(str(f'{gamma8:.6f}')+',')    #128
        for beta8 in bias_9['xnor.8.bias'].cpu().numpy():     #Last Layer bn beta
            dest.write(str(f'{beta8:.6f}')+',')    #128
        for bias9 in bias_9['xnor.9.bias'].cpu().numpy():	#Last Layer bias2
            dest.write(str(f'{bias9:.6f}')+',')    #28
#---------------------------Last Layer fData 28x128x1x1 Float------------------
        for weight9_output in bias_9['xnor.9.weight'].cpu().numpy(): #28x128x1x1
            for weight9_input in weight9_output:
                if first:
                    dest.write(str(f'{weight9_input[0][0]:.6f}'))
                    first = False
                else:
                    dest.write(','+str(f'{weight9_input[0][0]:.6f}'))
        dest.write('};')
        dest.write('\n')
        first = True
#---------------------------bData----------------------------------------#
#---------------------------layer B1-------------------------------------#
        dest.write('unsigned bData[] = {')
        for weight3_output in bias_9['xnor.3.conv.weight'].cpu().numpy(): #64x1x3x3
            w1 = 0
            w2 = 0
            w3 = 0
            w4 = 0
            w5 = 0
            w6 = 0
            w7 = 0
            w8 = 0
            w9 = 0
            #print(weight3_output)
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[0][0] > 0:
                    w1 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w1)+',')
                    w1 = 0
                #print(weight3_input)
                #print(weight3_input[0][1])
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[0][1] > 0:
                    w4 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w4)+',')
                    w4 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[0][2] > 0:
                    w7 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w7)+',')
                    w7 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[1][0] > 0:
                    w2 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w2)+',')
                    w2 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[1][1] > 0:
                    w5 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w5)+',')
                    w5 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[1][2] > 0:
                    w8 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w8)+',')
                    w8 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[2][0] > 0:
                    w3 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w3)+',')
                    w3 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[2][1] > 0:
                    w6 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w6)+',')
                    w6 = 0
            shift = 0
            for weight3_input in weight3_output:
                shift = shift % 32	
                if weight3_input[2][2] > 0:
                    w9 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w9)+',')
                    w9 = 0
            #print (w1)
#---------------------------layer B2----------------------------------------
        for weight4_output in bias_9['xnor.4.conv.weight'].cpu().numpy(): #128x2x3x3
            w1 = 0
            w2 = 0
            w3 = 0
            w4 = 0
            w5 = 0
            w6 = 0
            w7 = 0
            w8 = 0
            w9 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[0][0] > 0:
                    w1 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w1)+',')
                    w1 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[0][1] > 0:
                    w4 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w4)+',')
                    w4 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[0][2] > 0:
                    w7 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w7)+',')
                    w7 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[1][0] > 0:
                    w2 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w2)+',')
                    w2 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[1][1] > 0:
                    w5 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w5)+',')
                    w5 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[1][2] > 0:
                    w8 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w8)+',')
                    w8 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[2][0] > 0:
                    w3 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w3)+',')
                    w3 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[2][1] > 0:
                    w6 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w6)+',')
                    w6 = 0
            shift = 0
            for weight4_input in weight4_output:
                shift = shift % 32	
                if weight4_input[2][2] > 0:
                    w9 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w9)+',')
                    w9 = 0
            #print (w9)
#---------------------------layer B3----------------------------------------
        for weight5_output in bias_9['xnor.5.conv.weight'].cpu().numpy(): #128x4x3x3
            w1 = 0
            w2 = 0
            w3 = 0
            w4 = 0
            w5 = 0
            w6 = 0
            w7 = 0
            w8 = 0
            w9 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[0][0] > 0:
                    w1 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w1)+',')
                    w1 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[0][1] > 0:
                    w4 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w4)+',')
                    w4 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[0][2] > 0:
                    w7 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w7)+',')
                    w7 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[1][0] > 0:
                    w2 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w2)+',')
                    w2 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[1][1] > 0:
                    w5 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w5)+',')
                    w5 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[1][2] > 0:
                    w8 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w8)+',')
                    w8 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[2][0] > 0:
                    w3 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w3)+',')
                    w3 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[2][1] > 0:
                    w6 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w6)+',')
                    w6 = 0
            shift = 0
            for weight5_input in weight5_output:
                shift = shift % 32	
                if weight5_input[2][2] > 0:
                    w9 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w9)+',')
                    w9 = 0
            #print (w1)

#---------------------------layer B4----------------------------------------
        for weight6_output in bias_9['xnor.6.conv.weight'].cpu().numpy(): #128x4x3x3
            w1 = 0
            w2 = 0
            w3 = 0
            w4 = 0
            w5 = 0
            w6 = 0
            w7 = 0
            w8 = 0
            w9 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[0][0] > 0:
                    w1 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w1)+',')
                    w1 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[0][1] > 0:
                    w4 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w4)+',')
                    w4 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[0][2] > 0:
                    w7 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w7)+',')
                    w7 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[1][0] > 0:
                    w2 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w2)+',')
                    w2 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[1][1] > 0:
                    w5 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w5)+',')
                    w5 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[1][2] > 0:
                    w8 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w8)+',')
                    w8 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[2][0] > 0:
                    w3 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w3)+',')
                    w3 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[2][1] > 0:
                    w6 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w6)+',')
                    w6 = 0
            shift = 0
            for weight6_input in weight6_output:
                shift = shift % 32	
                if weight6_input[2][2] > 0:
                    w9 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    dest.write(str(w9)+',')
                    w9 = 0
            #print (w1)
#---------------------------Last Layer 128x128x1x1 Binary----------------------------------------

        for weight7_output in bias_9['xnor.7.conv.weight'].cpu().numpy(): #128x4x1x1
            w1 = 0
            shift = 0
            for weight7_input in weight7_output:
                shift = shift % 32	
                if weight7_input[0][0] > 0:
                    w1 |= 1 <<shift;	
                shift = shift + 1
                if (shift % 32) == 0:
                    if first:
                        dest.write(str(w1))
                        w1 = 0
                        first = False
                    else:
                        dest.write(',' + str(w1))
                        w1 = 0
            #print (w1)
        dest.write('};')
        first = True

    dest.close()
