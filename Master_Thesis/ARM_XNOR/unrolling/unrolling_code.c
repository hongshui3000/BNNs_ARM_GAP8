//For 3x3 part
if(input->Z == 1)
{
sum += popcount(filter[ 0]*input->b[  leftX   *input->Y+leftY         ])+
       popcount(filter[ 1]*input->b[  leftX   *input->Y+leftY + 1     ])+
       popcount(filter[ 2]*input->b[  leftX   *input->Y+leftY + 2     ])+
       popcount(filter[ 3]*input->b[ (leftX+1)*input->Y+leftY         ])+
       popcount(filter[ 4]*input->b[ (leftX+1)*input->Y+leftY + 1     ])+
       popcount(filter[ 5]*input->b[ (leftX+1)*input->Y+leftY + 2     ])+
       popcount(filter[ 6]*input->b[ (leftX+2)*input->Y+leftY         ])+
       popcount(filter[ 7]*input->b[ (leftX+2)*input->Y+leftY + 1     ])+
       popcount(filter[ 8]*input->b[ (leftX+2)*input->Y+leftY + 2     ]);
}
else if(input->Z == 2)
{
sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*2    ])+
       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*2 + 1])+
       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY + 1)*2    ])+
       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY + 1)*2 + 1])+
       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 2)*2    ])+
       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 2)*2 + 1])+
       popcount(filter[ 6]^input->b[((leftX+1)*input->Y+leftY    )*2    ])+
       popcount(filter[ 7]^input->b[((leftX+1)*input->Y+leftY    )*2 + 1])+
       popcount(filter[ 8]^input->b[((leftX+1)*input->Y+leftY + 1)*2    ])+
       popcount(filter[ 9]^input->b[((leftX+1)*input->Y+leftY + 1)*2 + 1])+
       popcount(filter[10]^input->b[((leftX+1)*input->Y+leftY + 2)*2    ])+
       popcount(filter[11]^input->b[((leftX+1)*input->Y+leftY + 2)*2 + 1])+
       popcount(filter[12]^input->b[((leftX+2)*input->Y+leftY    )*2    ])+
       popcount(filter[13]^input->b[((leftX+2)*input->Y+leftY    )*2 + 1])+
       popcount(filter[14]^input->b[((leftX+2)*input->Y+leftY + 1)*2    ])+
       popcount(filter[15]^input->b[((leftX+2)*input->Y+leftY + 1)*2 + 1])+
       popcount(filter[16]^input->b[((leftX+2)*input->Y+leftY + 2)*2    ])+
       popcount(filter[17]^input->b[((leftX+2)*input->Y+leftY + 2)*2 + 1]);
}
else if (input->Z == 4){
sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*4    ])+
       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*4 + 1])+
       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY    )*4 + 2])+
       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY    )*4 + 3])+
       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 1)*4    ])+
       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 1])+
       popcount(filter[ 6]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 2])+
       popcount(filter[ 7]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 3])+
       popcount(filter[ 8]^input->b[ (leftX   *input->Y+leftY + 2)*4    ])+
       popcount(filter[ 9]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 1])+
       popcount(filter[10]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 2])+
       popcount(filter[11]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 3])+
       popcount(filter[12]^input->b[((leftX+1)*input->Y+leftY    )*4    ])+
       popcount(filter[13]^input->b[((leftX+1)*input->Y+leftY    )*4 + 1])+
       popcount(filter[14]^input->b[((leftX+1)*input->Y+leftY    )*4 + 2])+
       popcount(filter[15]^input->b[((leftX+1)*input->Y+leftY    )*4 + 3])+
       popcount(filter[16]^input->b[((leftX+1)*input->Y+leftY + 1)*4    ])+
       popcount(filter[17]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 1])+
       popcount(filter[18]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 2])+
       popcount(filter[19]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 3])+
       popcount(filter[20]^input->b[((leftX+1)*input->Y+leftY + 2)*4    ])+
       popcount(filter[21]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 1])+
       popcount(filter[22]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 2])+
       popcount(filter[23]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 3])+
       popcount(filter[24]^input->b[((leftX+2)*input->Y+leftY    )*4    ])+
       popcount(filter[25]^input->b[((leftX+2)*input->Y+leftY    )*4 + 1])+
       popcount(filter[26]^input->b[((leftX+2)*input->Y+leftY    )*4 + 2])+
       popcount(filter[27]^input->b[((leftX+2)*input->Y+leftY    )*4 + 3])+
       popcount(filter[28]^input->b[((leftX+2)*input->Y+leftY + 1)*4    ])+
       popcount(filter[29]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 1])+
       popcount(filter[30]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 2])+
       popcount(filter[31]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 3])+
       popcount(filter[32]^input->b[((leftX+2)*input->Y+leftY + 2)*4    ])+
       popcount(filter[33]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 1])+
       popcount(filter[34]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 2])+
       popcount(filter[35]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 3]);
}