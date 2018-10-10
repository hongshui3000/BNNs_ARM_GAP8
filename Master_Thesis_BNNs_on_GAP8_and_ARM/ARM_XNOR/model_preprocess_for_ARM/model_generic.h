


/*
* 	model_generic data structure 
*	here: start points are defined 
*/

/* fData:
*/
/* start layer */
#define 	SL_FILTERS	 	 32	
#define 	SL_BIAS		 	 0
#define 	SL_WEIGHTS	 	 32
#define 	SL_MEAN_1	 	 320
#define 	SL_STD_1		(SL_MEAN_1 + 1*SL_FILTERS)
#define 	SL_GAMMA_1		(SL_MEAN_1 + 2*SL_FILTERS)
#define 	SL_BETA_1		(SL_MEAN_1 + 3*SL_FILTERS)
#define 	SL_MEAN_2		(SL_MEAN_1 + 4*SL_FILTERS)
#define 	SL_STD_2		(SL_MEAN_1 + 5*SL_FILTERS)
#define 	SL_GAMMA_2		(SL_MEAN_1 + 6*SL_FILTERS)
#define 	SL_BETA_2		(SL_MEAN_1 + 7*SL_FILTERS)
#define 	SL_1_END		(SL_BETA_2 + SL_FILTERS)

/* binary layer 1 */
#define 	BIN_1_FILTERS	 64
#define 	BIN_1_BIAS		 SL_1_END
#define 	BIN_1_ALPAH		(BIN_1_BIAS + 1*BIN_1_FILTERS)
#define 	BIN_1_MEAN		(BIN_1_BIAS + 2*BIN_1_FILTERS)
#define 	BIN_1_STD		(BIN_1_BIAS + 3*BIN_1_FILTERS)
#define 	BIN_1_GAMMA		(BIN_1_BIAS + 4*BIN_1_FILTERS)
#define 	BIN_1_BETA		(BIN_1_BIAS + 5*BIN_1_FILTERS)
#define 	BIN_1_END		(BIN_1_BETA + BIN_1_FILTERS)	

/* binary layer 2 */
#define 	BIN_2_FILTERS	 128
#define 	BIN_2_BIAS		 BIN_1_END
#define 	BIN_2_ALPAH		(BIN_2_BIAS + 1*BIN_2_FILTERS)
#define 	BIN_2_MEAN		(BIN_2_BIAS + 2*BIN_2_FILTERS)
#define 	BIN_2_STD		(BIN_2_BIAS + 3*BIN_2_FILTERS)
#define 	BIN_2_GAMMA		(BIN_2_BIAS + 4*BIN_2_FILTERS)
#define 	BIN_2_BETA		(BIN_2_BIAS + 5*BIN_2_FILTERS)
#define 	BIN_2_END		(BIN_2_BETA + BIN_2_FILTERS)

/* binary layer 3 */
#define 	BIN_3_FILTERS	 128
#define 	BIN_3_BIAS		 BIN_2_END
#define 	BIN_3_ALPAH		(BIN_3_BIAS + 1*BIN_3_FILTERS)
#define 	BIN_3_MEAN		(BIN_3_BIAS + 2*BIN_3_FILTERS)
#define 	BIN_3_STD		(BIN_3_BIAS + 3*BIN_3_FILTERS)
#define 	BIN_3_GAMMA		(BIN_3_BIAS + 4*BIN_3_FILTERS)
#define 	BIN_3_BETA		(BIN_3_BIAS + 5*BIN_3_FILTERS)
#define 	BIN_3_END		(BIN_3_BETA + BIN_3_FILTERS)

/* binary layer 4 */
#define 	BIN_4_FILTERS	 128
#define 	BIN_4_BIAS		 BIN_3_END
#define 	BIN_4_ALPAH		(BIN_4_BIAS + 1*BIN_4_FILTERS)
#define 	BIN_4_MEAN		(BIN_4_BIAS + 2*BIN_4_FILTERS)
#define 	BIN_4_STD		(BIN_4_BIAS + 3*BIN_4_FILTERS)
#define 	BIN_4_GAMMA		(BIN_4_BIAS + 4*BIN_4_FILTERS)
#define 	BIN_4_BETA		(BIN_4_BIAS + 5*BIN_4_FILTERS)
#define 	BIN_4_END		(BIN_4_BETA + BIN_4_FILTERS)		

/* last layer */
#define 	LL_FILTERS	 	 128	
#define 	LL_START		 BIN_4_END
#define 	LL_BIAS_1		 LL_START
#define 	LL_ALPHA_1		(LL_BIAS_1 + 1*LL_FILTERS)
#define 	LL_MEAN 		(LL_BIAS_1 + 2*LL_FILTERS)
#define 	LL_STD			(LL_BIAS_1 + 3*LL_FILTERS)
#define 	LL_GAMMA		(LL_BIAS_1 + 4*LL_FILTERS)
#define 	LL_BETA			(LL_BIAS_1 + 5*LL_FILTERS)
#define 	LL_BIAS_2		(LL_BIAS_1 + 6*LL_FILTERS)
#define 	LL_WEIGHTS_2    (LL_BIAS_2 + 28)
#define 	LL_END_FLOAT	(LL_WEIGHTS_2 + 3584)

#define 	FLOAT_END		LL_END_FLOAT


/* bData:
*/
#define 	BIN_1_WEIGHTS 	0
#define 	BIN_1_WSEIZE	(9*BIN_1_FILTERS*1)

#define 	BIN_2_WEIGHTS   (BIN_1_WEIGHTS + BIN_1_WSEIZE)
#define 	BIN_2_WSEIZE	(9*BIN_2_FILTERS*2)

#define 	BIN_3_WEIGHTS   (BIN_2_WEIGHTS + BIN_2_WSEIZE)
#define 	BIN_3_WSEIZE 	(9*BIN_3_FILTERS*4)

#define 	BIN_4_WEIGHTS   (BIN_3_WEIGHTS + BIN_3_WSEIZE)
#define 	BIN_4_WSEIZE	(9*BIN_4_FILTERS*4)

#define 	LL_WEIGHTS_1    (BIN_4_WEIGHTS + BIN_4_WSEIZE)
#define 	LL_END_BIN      (LL_WEIGHTS_1 + 512)

#define 	BIN_END 		LL_END_BIN
