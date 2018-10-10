#	for(i = x0+dX; i < input->X-x0-dX; i += dX){
dX = 1;
dY = 1;
x0 = 0;
y0 = 0;
X = 64;
Y = 100;
Z = 1;
kY = 3;
kX = 3;
kX2 = 1;
kY2 = 1;
i = 1;
j = 1;
count = 0;
file = open("unrolling_code.c","w");
#for i in range(x0+dX, X-x0-dX, dX):
leftX = max(0, i-kX2);
rightX = min(X, i+kX2+1);
    #for(j = y0+dY; j < input->Y-y0-dY; j += dY){
#    for j in range(y0+dY, Y-y0-dY, dY):
#// Range on Y where to iterate with the filter
leftY = max(0, j-kY2);
rightY = min(Y, j+kY2+1);
	#// Calculate binary convolution
#	sum = 0;
file.write("//For 3x3 part\n")
file.write("if(input->Z == 1)\n")
file.write("{\n")
file.write("sum += ");
    #for(ki = leftX; ki < rightX; ki++){
for ki in range(leftX, rightX, 1):
    #for(kj = leftY; kj < rightY; kj++){
	for kj in range(leftY, rightY, 1):
        #for(kk = 0; kk < input->Z; kk++){
		for kk in range(0, Z, 1):
#		    sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
 #                    			     input->b[(ki*input->Y+kj)*input->Z+kk]);       
			temp1 = ((kX2+ki-i)*kY+kY2+kj-j)*Z+kk;
			temp2 = (ki*Y+kj)*Z+kk
			if count < 10:
				if count == 0:
					file.write("popcount(filter[ "+str(temp1))
				else:
					file.write("       popcount(filter[ "+str(temp1))
			else:
				file.write("       popcount(filter["+str(temp1))
			if (ki-leftX) == 0:
				if Z == 1:
					file.write("]*input->b[  leftX")
					file.write("   ")
				else:
					file.write("]*input->b[ (leftX")
					file.write("   ")
			else:
				if Z == 1:
					file.write("]*input->b[ (leftX+"+str(ki-leftX)+")")
				else:
					file.write("]*input->b[((leftX+"+str(ki-leftX)+")")
			file.write("*input->Y+leftY")
			if (kj-leftY) == 0:
				file.write("    ")
			else:
				file.write(" + "+str(kj-leftY))
			if Z == 1:
				file.write(" ")
			else:
				file.write(")*"+str(Z))
			if kk == 0:
				file.write("    ")
			else:
				file.write(" + "+str(kk))
			if count < 3*3*Z-1:
				file.write("])+\n")
			else:
				file.write("]);\n")
			count = count + 1;
count = 0;
file.write("}\n")
Z = 2
file.write("else if(input->Z == 2)\n")
file.write("{\n")
file.write("sum += ");
    #for(ki = leftX; ki < rightX; ki++){
for ki in range(leftX, rightX, 1):
    #for(kj = leftY; kj < rightY; kj++){
	for kj in range(leftY, rightY, 1):
        #for(kk = 0; kk < input->Z; kk++){
		for kk in range(0, Z, 1):
#		    sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
 #                    			     input->b[(ki*input->Y+kj)*input->Z+kk]);       
			temp1 = ((kX2+ki-i)*kY+kY2+kj-j)*Z+kk;
			temp2 = (ki*Y+kj)*Z+kk
			if count < 10:
				if count == 0:
					file.write("popcount(filter[ "+str(temp1))
				else:
					file.write("       popcount(filter[ "+str(temp1))
			else:
				file.write("       popcount(filter["+str(temp1))
			if (ki-leftX) == 0:
				if Z == 1:
					file.write("]^input->b[  leftX")
					file.write("   ")
				else:
					file.write("]^input->b[ (leftX")
					file.write("   ")
			else:
				if Z == 1:
					file.write("]^input->b[ (leftX+"+str(ki-leftX)+")")
				else:
					file.write("]^input->b[((leftX+"+str(ki-leftX)+")")
			file.write("*input->Y+leftY")
			if (kj-leftY) == 0:
				file.write("    ")
			else:
				file.write(" + "+str(kj-leftY))
			if Z == 1:
				file.write(" ")
			else:
				file.write(")*"+str(Z))
			if kk == 0:
				file.write("    ")
			else:
				file.write(" + "+str(kk))
			if count < 3*3*Z-1:
				file.write("])+\n")
			else:
				file.write("]);\n")
			count = count + 1;
file.write("}\n")
count = 0;
Z = 4;
file.write("else if (input->Z == 4)")
file.write("{\n")
file.write("sum += ");
    #for(ki = leftX; ki < rightX; ki++){
for ki in range(leftX, rightX, 1):
    #for(kj = leftY; kj < rightY; kj++){
	for kj in range(leftY, rightY, 1):
        #for(kk = 0; kk < input->Z; kk++){
		for kk in range(0, Z, 1):
#		    sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
 #                    			     input->b[(ki*input->Y+kj)*input->Z+kk]);       
			temp1 = ((kX2+ki-i)*kY+kY2+kj-j)*Z+kk;
			temp2 = (ki*Y+kj)*Z+kk
			if count < 10:
				if count == 0:
					file.write("popcount(filter[ "+str(temp1))
				else:
					file.write("       popcount(filter[ "+str(temp1))
			else:
				file.write("       popcount(filter["+str(temp1))
			if (ki-leftX) == 0:
				if Z == 1:
					file.write("]^input->b[  leftX")
					file.write("   ")
				else:
					file.write("]^input->b[ (leftX")
					file.write("   ")
			else:
				if Z == 1:
					file.write("]^input->b[ (leftX+"+str(ki-leftX)+")")
				else:
					file.write("]^input->b[((leftX+"+str(ki-leftX)+")")
			file.write("*input->Y+leftY")
			if (kj-leftY) == 0:
				file.write("    ")
			else:
				file.write(" + "+str(kj-leftY))
			if Z == 1:
				file.write(" ")
			else:
				file.write(")*"+str(Z))
			if kk == 0:
				file.write("    ")
			else:
				file.write(" + "+str(kk))
			if count < 3*3*Z-1:
				file.write("])+\n")
			else:
				file.write("]);\n")
			count = count + 1;
file.write("}")
file.close()
