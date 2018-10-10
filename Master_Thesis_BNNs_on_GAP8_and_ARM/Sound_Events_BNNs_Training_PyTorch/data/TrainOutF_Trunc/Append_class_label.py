with open("filename.csv", "r") as src:
	with open("filename_class_train.csv", "w") as dest:
		for line in src:
			dest.write('%s,%s\n'%(line.rstrip('\n'),line.rsplit('_',1)[0]))

src.close()
dest.close()
