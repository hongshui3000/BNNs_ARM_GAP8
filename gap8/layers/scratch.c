



extern TENSOR* binaryLayer_forward
(
	TENSOR* input,
	BINARYLAYER* l
)
{
    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
    int32_t kX2, kY2, outX, outY;
    int32_t leftX, rightX, leftY, rightY; 
    int32_t shift, bink;
    int32_t  sum;
    uint32_t *filter;

    /* TODO: CHECK, here was a binarization */
    printf(" alloc \n");

    outX = (input->X + 2*l->padX - l->kX)/l->dX + 1;
    outY = (input->Y + 2*l->padY - l->kY)/l->dY + 1;
    /* allocate output tensor */
    TENSOR* output = (TENSOR*)calloc(1, sizeof(TENSOR));
    tensor_allocate(output, TENSOR_BINARY, outX, outY, l->nFilters);
    printf(" outX:%d  outY:%d  bin_channels:%d \n", output->X, output->Y, output->Z);

    printf(" alloc done \n");
    // #pragma omp parallel for schedule(static,1) collapse(3)
    for(i = 0; i < outX; i++){
	for(j = 0; j < outY; j++){
	for(k = 0; k < output->Z; k++)
	{
		// printf("index %d\n", (i*output->Y + j)*output->Z + k);
		output->memBin[(i*outY + j)*output->Z + k] = 0; 
	}}}
	printf(" out zero done \n");
	/* Half size of the filters */
    kX2 = l->kX / 2;
    kY2 = l->kY / 2;

    /* starting position */
    x0 = kX2 - l->padX;
    y0 = kY2 - l->padY;
    
    shift = 0;

    printf(" x:%d  y:%d  z:%d \n", input->X, input->Y, input->Z);
    /* switch over all filters */
    // #pragma omp parallel for schedule(static,1)
    for(k = 0; k < l->nFilters; k++)
    {
		filter = &(l->weight[k * l->kX * l->kY * input->Z]);
		bink = k / 32;
		
		outi = 0;
		/* move filter in x-direction */
		for(i = x0; i < input->X - x0; i += l->dX)
		{
		    leftX  = s32_max(0, i - kX2);
		    rightX = s32_min(input->X, i + kX2+1);

		    outj = 0;
		    /* move filter in y-direction */
		    for(j = y0; j < input->Y - y0; j += l->dY)
		    {
				leftY  = s32_max(0, j - kY2);
				rightY = s32_min(input->Y, j + kY2+1);
				
				sum = 0;
				/* binary convolution */
				for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
				for(kk = 0; kk < input->Z; kk++)
				{					
					// 				/*  */
					// printf("kX2: %d   ki: %d   i: %d  kY2: %d  kj: %d  j: %d \n", kX2,ki, i, kY2,kj,j);
					// printf("index: %d\n", ((kX2 + ki - i)*l->kY + kY2 + kj - j)*input->Z + kk );
					int32_t index = kX2 + ki - i;				
					index = index * l->kY + kY2 + kj - j;
					index = index * input->Z + kk; 

					int32_t mem_index = ki 
					mem_index = mem_index * input->Y + kj;
					mem_index = mem_index * input->Z + kk; 

				    sum += 32 - s32_popcount( filter[ index] ^ input->memBin[mem_index] ); /*shared*/
				}}}
				/* ReLU, Batch Normalization and Binarization using only compares */
				if(sum > l->relu[k])
				{
				    if(l->signGamma[k] * sum > l->treshhold[k])
				    { 
						output->memBin[(outi * output->Y + outj)*output->Z + bink] |= 1<<shift; /*shared*/
				    }
				} 
				else 
				{
				    if(l->compare[k])
				    {
						output->memBin[(outi * output->Y + outj)*output->Z + bink] |= 1<<shift; /*shared*/
				    }
				}

				outj++;
		    } 
				 // while(1);

		    outi++;
		}

		shift = (shift+1) % 32;
		// printf(" filter number: %d\n", k);
    }

    tensor_free(input);

    return output;
}