import os
import torch
import pickle
import numpy
import torchvision.transforms as transforms
import pandas as pd
from torch.utils.data.dataset import Dataset

class dataset():
    def __init__(self, csv_file, root_dir=None, train=True):
        self.filelist = pd.read_csv(csv_file, header=None)
        self.root_dir = root_dir
        self.train = train
        self.transform = transforms.ToTensor()

    def __len__(self):
        return len(self.filelist)
        #if self.train:
        #    return len(self.train_data)
        #else:
        #    return len(self.test_data)

    def __getitem__(self, index):
        if self.train:
            train_data_path = os.path.join(self.root_dir, 'TrainOutF_Trunc/'+self.filelist.iloc[index,0])
            self.train_data = pd.read_csv(train_data_path, header=None)
            self.train_data = self.train_data.values
            self.train_data = torch.from_numpy(self.train_data.astype('float32'))
            self.train_data = self.train_data.unsqueeze(0)
            label_num = self.filelist.values
            labels = label_num[:,1]-1
            train_labels = torch.from_numpy(labels.astype('int'))
            img, target = self.train_data, train_labels[index]
        else:
            test_data_path = os.path.join(self.root_dir, 'TestOutF_Trun_No256/'+ self.filelist.iloc[index,0])
            self.test_data = pd.read_csv(test_data_path, header=None)
            self.test_data = self.test_data.values
            self.test_data = torch.from_numpy(self.test_data.astype('float32'))
            self.test_data = self.test_data.unsqueeze(0)
            label_num = self.filelist.values
            labels = label_num[:,1]-1
            test_labels = torch.from_numpy(labels.astype('int'))
            img, target = self.test_data, test_labels[index]

        return img, target
