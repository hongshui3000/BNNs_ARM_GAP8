/**
 * \file api.c
 * \brief
 * \date 16.06.2018
 * 
 * \details 
 */


/**
 * \defgroup Application  <DESCRIPTION>
 * [\addtogroup] Application
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/

#include <stdio.h>
#include "config.h"
#include "utility/main_function.h"
#ifdef I2S_DEMO
//    #include "wav_synchro.h"
//#include "wavOut3.dat"
#define WAV_HEADER_SIZE 44 //bytes
#define NB_BUF 4
#define NB_SUBUF_SAMPLES 13000
short int buffee[NB_SUBUF_SAMPLES*4];
typedef struct {
	unsigned char header[WAV_HEADER_SIZE];
	unsigned short buff[NB_BUF][NB_SUBUF_SAMPLES];
} recordBuf;
recordBuf buff;
static int enqueued = 0;
static int finished2 = 0;
static rt_i2s_t *i2s;

static void buffer_ready(void *arg)
{
	int id = ((int)arg)+2;//increase subbuffer index given as argument
	finished2++;
	enqueued++;
	if (id < NB_BUF)
	{	rt_i2s_capture(i2s, buff.buff[id], NB_SUBUF_SAMPLES*2, rt_event_get(NULL, buffer_ready, (void *)id));
		//printf("buffer2 %d ok\n",id);
	}
}

#endif

#define STACK_SIZE 2048
//#define STACK_SIZE 1024
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/

unsigned int PMU_set_voltage(unsigned int Voltage, unsigned int CheckFrequencies);

static int finished = 1;

static void pe_entry(void *arg)
{
	printf("(%d, %d) Entered PE with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);
}

static void cluster_entry(void *arg)
{
	printf("(%d, %d) Entered cluster with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);

	rt_team_fork(0, pe_entry, (void *)0x12345678);

}

static void end_of_call(void *arg)
{
	finished--;
}


/* ==================================================================================
 * Main function implementations
 * ================================================================================*/
int main(int argc, char *argv[])
{
	//	PMU_set_voltage(800, 1); // #Todo still can run 250MHz with 800mV?
	//Set up cluster and fabric controller frequency
	rt_freq_set(RT_FREQ_DOMAIN_CL, GAP8_MAX_NV_FREQUENCY);// 250M
	//rt_freq_set(RT_FREQ_DOMAIN_FC, GAP8_MAX_NV_FREQUENCY);// 250M
	//rt_freq_set(RT_FREQ_DOMAIN_CL, GAP8_MAX_LV_FREQUENCY);// 150M  //command out for Mr. Wolf
	rt_freq_set(RT_FREQ_DOMAIN_FC, GAP8_MAX_LV_FREQUENCY);// 150M
	//rt_freq_set(RT_FREQ_DOMAIN_CL, 100000000);// 100M
	//rt_freq_set(RT_FREQ_DOMAIN_FC, 100000000);// 100M
	//rt_freq_set(RT_FREQ_DOMAIN_CL, 50000000);// 50M
	//rt_freq_set(RT_FREQ_DOMAIN_FC, 50000000);// 50M
	rt_event_sched_t sched;
	rt_thread_t thread;

	rt_event_sched_init(&sched);
	if (rt_event_alloc(&sched, 1)) return -1;
	rt_cluster_mount(1, 0, 0, NULL);
	//printf("Entering main controller\n");
#ifdef I2S_DEMO
	// Allocate events to schedule the end-of-transfer events
	if (rt_event_alloc(NULL, 8)) return -1;

	// Initialize desired configuration
	rt_i2s_conf_t i2s_conf;
	rt_i2s_conf_init(&i2s_conf);

	i2s_conf.frequency = 16000;
	i2s_conf.dual = 0;	//mono
	i2s_conf.width = 16;   //16 bits
	i2s_conf.pdm = 1; 
	i2s_conf.decimation_log2 = 6;
	i2s_conf.id = 0;
	// Open the microphone
	i2s = rt_i2s_open(NULL, &i2s_conf, NULL);
	if (i2s == NULL) return -1;

	//printf("microphone init\n");
#endif
	void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
	if (stacks == NULL) return -1;
#ifdef I2S_DEMO
	//init buffer
	for(unsigned cntJ=0; cntJ<(NB_BUF);cntJ++) {
		for(unsigned cnt=0; cnt<(NB_SUBUF_SAMPLES);cnt++) {
			buff.buff[cntJ][cnt] = 0xaaaa;
		}
	}
#endif


#ifdef BOARD_MODE
	while(1)
	{
#endif

	finished = 1;
	#ifdef I2S_DEMO
	rt_cluster_call(NULL, 0, firstfunction, buffee, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), rt_event_get(&sched, end_of_call, (void *)0));
	#else
	rt_cluster_call(NULL, 0, firstfunction, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), rt_event_get(&sched, end_of_call, (void *)0));
	#endif


#ifdef I2S_DEMO

	finished2 = 0;
	enqueued = 2;
	//rt_i2s_capture(i2s, buff.buff[0], NB_SUBUF_SAMPLES*2, NULL);
	rt_i2s_capture(i2s, buff.buff[0], NB_SUBUF_SAMPLES*2, rt_event_get(NULL, buffer_ready, (void *)0));
	rt_i2s_capture(i2s, buff.buff[1], NB_SUBUF_SAMPLES*2, rt_event_get(NULL, buffer_ready, (void *)1));

	// Start acquisition
	rt_i2s_start(i2s);

	// Loop until we processed enough buffers
	while(finished2 < NB_BUF) {
		rt_event_execute(NULL, 1);
	}
#endif



	while(finished) {
		rt_event_execute(&sched, 1);
	}

#ifdef I2S_DEMO
        short int scal = 40;
        short int ofsett = 0;
	for(int k = 0; k < NB_SUBUF_SAMPLES; k++)
	{
		buffee[k] = (short int) scal * (short int)(buff.buff[0][k]-ofsett);
	}	
	for(int k = 0; k < NB_SUBUF_SAMPLES; k++)
	{
		buffee[NB_SUBUF_SAMPLES + k] = (short int) scal * (short int) (buff.buff[1][k]-ofsett);
	}	
	for(int k = 0; k < NB_SUBUF_SAMPLES; k++)
	{
		buffee[2*NB_SUBUF_SAMPLES+k] = (short int) scal * (short int) (buff.buff[2][k]-ofsett);
	}	
	for(int k = 0; k < NB_SUBUF_SAMPLES; k++)
	{
		buffee[3*NB_SUBUF_SAMPLES+k] = (short int) scal *  (short int) (buff.buff[3][k]-ofsett);
	}

#endif
#ifdef BOARD_MODE
}
#endif
	printf("c: Closing microphones\n");
#ifdef I2S_DEMO

	rt_i2s_close(i2s, NULL);
#endif
	rt_cluster_mount(0, 0, 0, NULL);

	return 0; 
}
