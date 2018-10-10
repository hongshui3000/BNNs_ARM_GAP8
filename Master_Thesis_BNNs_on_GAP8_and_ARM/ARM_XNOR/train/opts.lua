local M = { }

function M.parse(arg)
    local cmd = torch.CmdLine()
    cmd:text()
    cmd:text('Torch-7 XNOR Training script')
    cmd:text()
    cmd:text('Options:')

    ------------ General options --------------------
    cmd:option('-cache',       'cache/', 'subdirectory in which to save/log experiments')
    cmd:option('-data',        'data/',  'dataset folder')
    cmd:option('-manualSeed',  2,        'Manually set RNG seed')
    cmd:option('-GPU',         1,        'Default preferred GPU')

    ------------- Data options ------------------------
    cmd:option('-nClasses',   28,      'number of classes in the dataset')
    cmd:option('-dataType',   'image', 'Options: image | numpy' )

    ------------- Training options --------------------
    cmd:option('-nEpochs',      16,     'Number of total epochs to run')
    cmd:option('-batchSize',    64,     'mini-batch size (1 = pure stochastic)')
    cmd:option('-binaryWeight', false,  'binarize the convolutional layers')
    cmd:option('-shuffle',      false,  'Shuffle before each epoch')
    cmd:option('-verbose',      false,  'display accuracy on each batch')

    ---------- Optimization options ----------------------
    cmd:option('-LR',              0.0,   'learning rate; if set, overrides default LR/WD recipe')
    cmd:option('-momentum',        0.9,   'momentum')
    cmd:option('-weightDecay',     0,     'weight decay')

    ---------- Model options ----------------------------------
    cmd:option('-netPath',     'alexnetxnor',  'net folder name')
    cmd:option('-optimType',   'sgd',          'Options: sgd | adam')
    cmd:option('-retrain',     'none',         'path to model to retrain with')
    cmd:option('-testOnly',     false,         'test only')
    cmd:option('-loadParams',  'none',         'path to model to load the parameters')
    cmd:option('-optimState',  'none',         'path to an optimState to reload from')

    cmd:text()

    local opt = cmd:parse(arg or {})
 
    -- create save directory
    opt.save = paths.concat(opt.cache, '' .. os.date():gsub(' ',''))
    
    return opt
end

return M
