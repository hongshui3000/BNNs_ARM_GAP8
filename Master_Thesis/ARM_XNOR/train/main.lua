require 'torch'
require 'cutorch'
require 'paths'
require 'xlua'
require 'optim'
require 'nn'
require 'newLayers'

torch.setdefaulttensortype('torch.FloatTensor')

local opts = paths.dofile('opts.lua')
opt = opts.parse(arg)

nClasses = opt.nClasses

paths.dofile('util.lua')
paths.dofile('model.lua')

print(opt)

cutorch.setDevice(opt.GPU)
torch.manualSeed(opt.manualSeed)

print('Saving everything to: ' .. opt.save)
os.execute('mkdir -p ' .. opt.save)

if opt.dataType == 'image' then
   paths.dofile('data.lua')
else
   paths.dofile('data-npy.lua')
end

paths.dofile('train.lua')
paths.dofile('test.lua')

epoch = 1
if opt.testOnly then
	test()
else
   for i=1,opt.nEpochs do
      train()
      test()
      epoch = epoch + 1
   end
end
