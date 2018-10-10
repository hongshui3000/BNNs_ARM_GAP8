require 'optim'

--[[
   1. Setup SGD optimization state and learning rate schedule
   2. Create loggers.
   3. train - this function handles the high-level training loop,
              i.e. load data, train model, save model and state to disk
   4. trainBatch - Used by train() to train a single batch after the data is loaded.
]]--

-- Setup a reused optimization state (for sgd). If needed, reload it from disk
local optimState = {
    learningRate = opt.LR,
    learningRateDecay = 0.0,
    momentum = opt.momentum,
    dampening = 0.0,
    weightDecay = opt.weightDecay
}

-- 2. Create loggers.
trainLogger = optim.Logger(paths.concat(opt.save, 'train.log'))
local batchNumber
local top1Sum, top5Sum, loss_epoch

-- 3. train - this function handles the high-level training loop,
--            i.e. load data, train model, save model and state to disk
function train()
   print('==> doing epoch on training data:')
   print("==> online epoch # " .. epoch)
   
   batchNumber = 0
   cutorch.synchronize()

   -- set the dropouts to training mode
   model:training()

   local tm = torch.Timer()
   top1Sum = 0
   top5Sum = 0
   loss_epoch = 0

   if opt.shuffle then
      X_train, y_train = shuffle(X_train, y_train)
   end
   
   makeBatches(X_train, y_train)
   for i=1,nBatches do
      local inputs, labels = getBatch(i, X_train, y_train)
      trainBatch(inputs, labels)
   end

   cutorch.synchronize()
   
   loss_epoch = loss_epoch/nBatches

   trainLogger:add{
      ['% top1 accuracy (train set)'] = top1Sum/nBatches,
      ['% top5 accuracy (train set)'] = top5Sum/nBatches,
      ['avg loss (train set)'] = loss_epoch
   }

   print(string.format('Epoch: [%d][TRAINING SUMMARY] Total Time(s): %.2f\t'
                          .. 'average loss (per batch): %.2f \t '
                          .. 'accuracy(%%):\t top-1 %.2f\t',
                       epoch, tm:time().real, loss_epoch, top1Sum/nBatches))
   print('\n')

   -- save model
   collectgarbage()

   --saveDataParallel(paths.concat(opt.save, 'model_' .. epoch .. '.t7'), model) -- defined in util.lua
   --torch.save(paths.concat(opt.save, 'optimState_' .. epoch .. '.t7'), optimState)
end


-- GPU inputs (preallocate)
collectgarbage()
local inputs = torch.CudaTensor()
local labels = torch.CudaTensor()

local timer = torch.Timer()
local dataTimer = torch.Timer()
local procTimer = torch.Timer()

-- 4. trainBatch - Used by train() to train a single batch after the data is loaded.
function trainBatch(inputsCPU, labelsCPU)
   cutorch.synchronize()
   collectgarbage()
   local dataLoadingTime = dataTimer:time().real
   procTimer:reset()

   -- transfer over to GPU
   inputs:resize(inputsCPU:size()):copy(inputsCPU)
   labels:resize(labelsCPU:size()):copy(labelsCPU)
   
   local err, outputs

   if opt.binaryWeight then
      meancenterConvParms(convNodes)
      clampConvParms(convNodes)
      realParams:copy(parameters)
      binarizeConvParms(convNodes)
  end
   
   model:zeroGradParameters()
   outputs = model:forward(inputs)
   err = criterion:forward(outputs, labels)
   
   local gradOutputs = criterion:backward(outputs, labels)
   model:backward(inputs, gradOutputs)      
   
   if opt.binaryWeight then
      parameters:copy(realParams)
      updateBinaryGradWeight(convNodes)
      --if opt.optimType == 'adam' then
      --   gradParameters:mul(1e+9);
      --end
      --if opt.nGPU >1 then
      --   model:syncParameters()
      --end
   end
   
   local feval = function()
      return err, gradParameters
   end
   
   if opt.optimType == "sgd" then
      optim.sgd(feval, parameters, optimState)
   elseif opt.optimType == "adam" then
      optim.adam(feval, parameters, optimState)
   elseif opt.optimType == "adamax" then
      optim.adam(feval, parameters, optimState)
   end

   -- DataParallelTable's syncParameters
   if model.needsSync then
      model:syncParameters()
   end
   

   cutorch.synchronize()
   batchNumber = batchNumber + 1
   loss_epoch = loss_epoch + err
   
   local pred = outputs:float()
   local top1, top5 = computeScore(pred, labelsCPU, 1)
   top1Sum = top1Sum + top1
   top5Sum = top5Sum + top5
   -- Calculate top-1 error, and print information
   if opt.verbose then
      print(('Epoch: [%d][%d/%d]\tTime %.3f(%.3f) Err %.4f Top1-%%: %.2f (%.2f)  Top5-%%: %.2f (%.2f) LR %.0e DataTime %.3f'):format(
               epoch, batchNumber, nBatches, timer:time().real, procTimer:time().real, err, top1, top1Sum/batchNumber, top5, top5Sum/batchNumber, optimState.learningRate, dataLoadingTime))
   end

   dataTimer:reset()
   timer:reset()
end
