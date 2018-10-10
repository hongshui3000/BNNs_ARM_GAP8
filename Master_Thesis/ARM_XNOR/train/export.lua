require 'torch'
require 'nn'
require 'cudnn'
require 'newLayers'

function parseConvLayer(L, binary)
   
   for filter = 1,L.bias:size(1) do
      table.insert(fparam, L.bias[filter])
   end

   if binary then
      local n = L.weight[1]:nElement()
      local m = L.weight:norm(1,4):sum(3):sum(2):div(n);
      
      L.weight:sign():cmul(m:clone():sign():expand(L.weight:size()))
      m:resize(m:nElement()):abs()
      
      for filter = 1,m:size(1) do
         table.insert(fparam, m[filter])
      end
   end

   for filter = 1,L.weight:size(1) do
      for i = 1,L.weight:size(3) do
         for j = 1,L.weight:size(4) do
            if binary then
               k = 1
               while k <= L.weight:size(2) do
                  b = 0
                  shift = 0
                  
                  while shift < 32 and k <= L.weight:size(2) do
                     if L.weight[filter][k][i][j] > 0 then
                        b = b + math.pow(2, shift)
                     end
                     k = k + 1
                     shift = shift + 1
                  end
                  
                  table.insert(bparam, b)
               end
            else
               for k = 1,L.weight:size(2) do
                  table.insert(fparam, L.weight[filter][k][i][j])
               end
            end
         end
      end
   end

end

function parseBatchLayer(L)

   for i=1,L.running_mean:size(1) do
      table.insert(fparam, L.running_mean[i])
   end
   
   for i=1,L.running_var:size(1) do
      table.insert(fparam, L.running_var[i])
   end
   
   for i=1,L.weight:size(1) do
      table.insert(fparam, L.weight[i])
   end
   
   for i=1,L.bias:size(1) do
      table.insert(fparam, L.bias[i])
   end

end

model = torch.load('model-74.t7')
model:clearState()

convLayer = model:findModules('cudnn.SpatialConvolution')
batchLayer = model:findModules('nn.SpatialBatchNormalization')

bparam = {}
fparam = {}

parseConvLayer(convLayer[1], false)
parseBatchLayer(batchLayer[1])
parseConvLayer(convLayer[2], true)
parseBatchLayer(batchLayer[2])
parseConvLayer(convLayer[3], true)
parseBatchLayer(batchLayer[3])
parseConvLayer(convLayer[4], true)
parseBatchLayer(batchLayer[4])
parseConvLayer(convLayer[5], true)
parseBatchLayer(batchLayer[5])
parseConvLayer(convLayer[6], true)
parseBatchLayer(batchLayer[6])
parseConvLayer(convLayer[7], true)
parseBatchLayer(batchLayer[7])

fpData = io.open("model.dat", "w")

fpData:write("float fData[] = {")
for i=1,#fparam do
   fpData:write(string.format("%f", fparam[i]))
   if i < #fparam then
      fpData:write(",")
   end
end
fpData:write("};\n")

fpData:write("unsigned bData[] = {")
for i=1,#bparam do
   fpData:write(string.format("%d", bparam[i]))
   if i < #bparam then
      fpData:write(",")
   end
end
fpData:write("};\n")

print(#convLayer)
print(#batchLayer)
print(#fparam)
print(#bparam)
