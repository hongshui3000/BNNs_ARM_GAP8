function createModel()
   require 'cudnn'

   local function BinConvolution(nInputPlane, nOutputPlane, kW, kH, dW, dH, padW, padH, mW, mH)
      local C = nn.Sequential()

      C:add(nn.SpatialBatchNormalization(nInputPlane, 1e-4, false))
      C:add(nn.BinActiveZ())
      C:add(cudnn.SpatialConvolution(nInputPlane, nOutputPlane, kW, kH, dW, dH, padW, padH))
      C:add(cudnn.ReLU(true))

      return C
   end
   
   local model = nn.Sequential()
   
   model:add(cudnn.SpatialConvolution(1, 32, 3, 3, 1, 1, 1, 1))
   model:add(nn.SpatialBatchNormalization(32, 1e-5, false))
   model:add(cudnn.ReLU(true))
   
   model:add(BinConvolution( 32,  64, 3, 3, 2, 2, 1, 1))
   --model:add(nn.SpatialMaxPooling(2, 2, 2, 2))
   
   model:add(BinConvolution( 64, 128, 3, 3, 1, 1, 1, 1))
   model:add(BinConvolution(128, 128, 3, 3, 2, 2, 1, 1))
   --model:add(nn.SpatialMaxPooling(2, 2, 2, 2))
   
   model:add(BinConvolution(128, 128, 3, 3, 1, 1, 1, 1))
   model:add(BinConvolution(128, 128, 1, 1, 1, 1, 0, 0))
   
   model:add(nn.SpatialBatchNormalization(128, 1e-3, false))
   --model:add(cudnn.ReLU(true))

   model:add(cudnn.SpatialConvolution(128, opt.nClasses, 1, 1, 1, 1, 0, 0))
   --model:add(nn.SpatialBatchNormalization(opt.nClasses, 1e-3, false))
   --model:add(cudnn.ReLU(true))
   model:add(nn.SpatialAveragePooling(100, 16, 1, 1))
   model:add(nn.View(opt.nClasses))
   model:add(nn.LogSoftMax())

--[[model:add(cudnn.SpatialConvolution(1, 32, 3, 3, 1, 1, 1, 1))
   model:add(cudnn.ReLU(true))
   model:add(BinConvolution( 32, 64, 3, 3, 2, 2, 1, 1))
   
   model:add(BinConvolution( 64, 128, 3, 3, 1, 1, 1, 1))
   model:add(BinConvolution(128, 128, 3, 3, 2, 2, 1, 1))

   model:add(BinConvolution(128, 128, 3, 3, 1, 1, 1, 1))
   model:add(BinConvolution(128, 128, 1, 1, 1, 1, 0, 0))

   model:add(nn.SpatialBatchNormalization(128, 1e-3, false))
   model:add(cudnn.ReLU(true))

   model:add(BinConvolution(128, opt.nClasses, 1, 1, 1, 1, 0, 0))
   model:add(nn.SpatialAveragePooling(100, 16, 1, 1))
   model:add(nn.View(opt.nClasses))
   model:add(nn.LogSoftMax())]]

   return model
end
