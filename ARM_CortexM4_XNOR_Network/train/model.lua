require 'nn'
require 'cunn'
require 'cudnn'
require 'optim'

--  if preloading option is set, preload weights from existing models appropriately
if opt.retrain ~= 'none' then
   assert(paths.filep(opt.retrain), 'File not found: ' .. opt.retrain)
   print('Loading model from file: ' .. opt.retrain)
   model = torch.load(opt.retrain)
   model:clearState()
else
   paths.dofile('models/' .. opt.netPath .. '.lua')
   print('=> Creating model from file: models/' .. opt.netPath .. '.lua')
   model = createModel()
   
   --Initializing the parameters 
   model:apply(rand_initialize)
end

-- apply parallel 
model:cuda()

-- getting the parameters and gradient pointers
parameters, gradParameters = model:getParameters()
realParams = parameters:clone()
convNodes = model:findModules('cudnn.SpatialConvolution')

-- much faster
cudnn.fastest = true
cudnn.benchmark = true

-- create criterion
criterion = nn.ClassNLLCriterion()

print('=> Model')
print(model)

print('=> Criterion')
print(criterion)

print('==> Converting model to CUDA')
criterion:cuda()

collectgarbage()
