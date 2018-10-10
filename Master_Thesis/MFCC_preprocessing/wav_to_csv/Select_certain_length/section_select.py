#with open("acoustic_guitar_16.csv", "r") as src:
with open("applause_175.csv", "r") as src:
	#with open("A_section_of_waveF16.dat", "w") as dest:
	with open("Class3_175.dat", "w") as dest:
		#dest.write('RT_L2_DATA short int wav_sample[] = {')
		dest.write('RT_L2_DATA short int wav_sample[] = {')
		for i, line in enumerate(src):
			if i >=2000 and i<57000:
				dest.write('%d,\n'%int((float(line.rsplit(',',1)[1]))*32768))
				#dest.write('%f,\n'%(float(line.rsplit(',',1)[1])))
		dest.write('};')

src.close()
dest.close()
