/**
 * @file embrio.h
 * @brief EmbrIO.
 * @addtogroup EmbrIO
 * @{
 */

#ifndef _EMBRIO_H_
#define _EMBRIO_H_

#include "ch.h"
#include "Embryo.h"
#include "embryo_private.h"



// hook for callback functions
typedef void (*Embrio_hook)(void *params);

typedef enum {
  EMBRIOVM_UNINIT	= 0,
  EMBRIOVM_STOP		= 1,
  EMBRIOVM_RUN		= 2,
  EMBRIOVM_SLEEP	= 3,
  EMBRIOVM_FAIL		= 4,
} EmbrioVMstate_t;


struct _EmbrioVM {
	EmbrioVMstate_t	state;
	Thread			*tp;
	Embryo_Program	*ep;
	Embrio_hook		hook;
	int				hook_params;
	void			*vm_next;
/*
	Embryo_Cell		pri;
	Embryo_Cell		alt;
	Embryo_Cell		stk;
	Embryo_Cell		frm;
	Embryo_Cell		hea;
	Embryo_Cell		hea_start;
	Embryo_Cell		reset_stk;
	Embryo_Cell		reset_hea;
	Embryo_Cell		*cip;

	unsigned char	*code;
	unsigned char	*data;
*/
};


typedef enum {
  EMBRIOVMM_STOP	= 0,
  EMBRIOVMM_RUN		= 1,
  EMBRIOVMM_SLEEP	= 2,
  EMBRIOVMM_IRQ		= 3,
} EmbrioVMMstate_t;


typedef struct _EmbrioVM EmbrioVM;

typedef struct {
  EmbrioVMMstate_t	state;
  uint8_t			vm_count;
  EmbrioVM			*vm_first;
} EmbrioVMManager;


// define the max number of virtual machines and programs
#define MAX_EMBRIO_VM_NUM 2


// array defining the Programs and pointer to memory pool
extern MemoryPool EP_mp;
extern Embryo_Program EP_pool[MAX_EMBRIO_VM_NUM];


// array defining the Virtual Machines and memory pool
extern MemoryPool EVM_mp;
extern EmbrioVM EVM_pool[MAX_EMBRIO_VM_NUM];

// variable defining the actual VM used
extern int currVM;



// array defining the Thread used for running VMs
// size of thread (used for macro THD_WA_SIZE)
#define THD_STACK_SIZE 512

extern MemoryPool THD_mp;
// extern struct THD_vm_wa;


#define EMBRIO_HEAP_SIZE 4096

// global Memory Heap for embrio elements
extern MemoryHeap embrio_mh;

// 4kb of buffer for embrio heap elements
extern stkalign_t embrio_buff[EMBRIO_HEAP_SIZE  / sizeof(stkalign_t)];

extern EmbrioVMManager *vm_man;
extern EmbrioVM *vm[MAX_EMBRIO_VM_NUM];

#ifdef __cplusplus
// extern "C" {
#endif

	Embryo_Status embryo_program_run(EmbrioVM *vmp, Embryo_Function fn);

	void embrioInit(void);

	void embrioPoolsSetup(void);
	void embrioHeapSetup(void);

	void embrioPoolsPrealloc(void);
	void embrioHeapAlloc(void);


	void embrioVMMinsert(EmbrioVMManager *vm_man, EmbrioVM *new_vm);

	Thread *vmStart(EmbrioVM *vm, tprio_t prio, const char *name);
	void embrio_vm_sleep(EmbrioVM *vmp);
	void embrio_vm_resume(EmbrioVM *vmp);
	void embrio_native_call(EmbrioVM *vmp);
	void embrio_call(EmbrioVM *vmp);
#ifdef __cplusplus
// }
#endif

#ifndef _TEST_ADC_
/* ADC data */

/* Total number of channels to be sampled by a single ADC operation.*/
#define ADC_GRP1_NUM_CHANNELS	4

/* Depth of the conversion buffer, channels are sampled four times each.*/
#define ADC_GRP1_BUF_DEPTH		4

extern adcsample_t samples[];
extern const ADCConversionGroup adcgrpcfg;
#endif

#endif /* _VM_H_ */

/** @} */
