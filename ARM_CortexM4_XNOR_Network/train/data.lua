require 'image'

-- Load images from class subfolders
function loadFolder(path)

   images = {}
   classes = {}
   
   for c = 1,28 do
      local cpath = path .. tostring(c) .. '/'
      for file in paths.files(cpath) do
         if file:find('png$') then
            img = image.load(cpath .. file, 1)
            img = (img - mean):cdiv(std)
            table.insert(images, img)
            table.insert(classes, c)
         end
      end
   end

   return images, classes

end

-- Load mean and std file
mean = image.load(opt.data .. 'mean.png')
std = image.load(opt.data .. 'std.png')

X_train, y_train = loadFolder(opt.data .. 'train/')
X_test, y_test = loadFolder(opt.data .. 'test/')

-- Permute training set
X_aux = {}
y_aux = {}

perm = torch.randperm(#X_train)
for i = 1,perm:size()[1] do
   table.insert(X_aux, X_train[perm[i]])
   table.insert(y_aux, y_train[perm[i]])
end

X_train = X_aux
y_train = y_aux

IMG_X = 64
IMG_Y = 400
IMG_Z = 1

print('Loaded data from: ' .. opt.data)

-- Create batches (call before epoch)
function makeBatches(X, y)

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
