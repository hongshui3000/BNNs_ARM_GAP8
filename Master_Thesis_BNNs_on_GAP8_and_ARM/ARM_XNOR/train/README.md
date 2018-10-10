## Setting up

Requires torch7 http://torch.ch
Optional numpy4th https://github.com/htwaijry/npy4th
         (for reading numpy arrays into torch)

When using numpy data the folder containing the data is expected
to have 4 files: X_train.npy, y_train.npy, X_test.npy, y_test.npy.

## Usage

All available options are listed in opts.lua

Training
th main.lua -data path-to-data/ -dataType numpy -nEpochs 30 -batchSize 64 -binaryWeight -optimType sgd -LR 1e-2 -weightDecay 5e-4

Re-training an existing model add 
-retrain path-to-model

Testing a model
th main.lua -data path-to-data/ -dataType numpy -binaryWeight -retrain path-to-model -testOnly

## Exporting

Exporting the data for usage with the C++ forward pass code can be done
using the export.lua script. The network structure and input file must be
modified from the code for the result to work.
