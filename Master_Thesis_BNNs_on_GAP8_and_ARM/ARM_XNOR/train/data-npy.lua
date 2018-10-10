npy4th = require 'npy4th'

X_path = paths.concat(opt.data, 'X_train.npy')
y_path = paths.concat(opt.data, 'y_train.npy')
X_train = npy4th.loadnpy(X_path)
y_train = npy4th.loadnpy(y_path)

X_path = paths.concat(opt.data, 'X_test.npy')
y_path = paths.concat(opt.data, 'y_test.npy')
X_test = npy4th.loadnpy(X_path)
y_test = npy4th.loadnpy(y_path)

IMG_Z = X_train:size(2)
IMG_X = X_train:size(3)
IMG_Y = X_train:size(4)

function toTable(tensor)

   aux = {}
   for i = 1,tensor:size(1) do
      table.insert(aux, tensor[i])
   end
   
   return aux

end

X_train = toTable(X_train)
y_train = toTable(y_train)
X_test = toTable(X_test)
y_test = toTable(y_test)

-- Shuffle data
function shuffle(X, y)
   X_aux = {}
   y_aux = {}
   
   perm = torch.randperm(#X)
   for i = 1,perm:size(1) do
      table.insert(X_aux, X[perm[i]])
      table.insert(y_aux, y[perm[i]])
   end

   return X_aux, y_aux
end

if not opt.shuffle then
   X_train, y_train = shuffle(X_train, y_train)
end

print('Loaded data from: ' .. opt.data)

-- Create batches (call before epoch)
function makeBatches(X, y)

   --X, y = shuffle(X, y)

   nSamples = #y
   nBatches = math.floor(nSamples / opt.batchSize)

   batches = {}
   
   for i=0,nBatches-1 do
      table.insert(batches, i*opt.batchSize+1)
   end

end

-- Get a certain batch
function getBatch(n, X, y)

   local data, labels, j
   
   data = torch.Tensor(opt.batchSize, IMG_Z, IMG_X, IMG_Y)
   labels = torch.LongTensor(opt.batchSize)
   
   for i=1,opt.batchSize do
      j = batches[n]+i-1
      data[i]:copy(X[j])
      labels[i] = y[j]
   end

   return data, labels

end
