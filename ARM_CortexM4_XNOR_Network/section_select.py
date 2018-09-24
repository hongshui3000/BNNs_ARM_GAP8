with open("acoustic_guitar_65.csv", "r") as src:
	with open("A_section_of_wave.csv", "w") as dest:
		dest.write('const float wav_sample = {')
		for i, line in enumerate(src):
			if i >=384 and i<100000:
				dest.write('%s,'%(line.rsplit(',',1)[1]))
		dest.write('};')

src.close()
dest.close()
