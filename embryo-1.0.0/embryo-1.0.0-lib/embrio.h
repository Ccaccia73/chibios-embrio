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
	int			hook_params;
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
  EmbrioVM			*vm_next;
} EmbrioVMManager;


// define the max number of virtual machines and programs
#define MAX_EMBRIO_VM_NUM 4

// arrays defining the Programs and VMs
// Embryo_Program EP_pool[MAX_EMBRIO_VM_NUM];
extern Embryo_Program EP_pool[MAX_EMBRIO_VM_NUM];
extern EmbrioVM EVM_pool[MAX_EMBRIO_VM_NUM];


// pointers to the memory pools used for programs and virtual machines
extern MemoryPool *EP_mp;
extern MemoryPool *EVM_mp;

// variable defining the actual VM used
extern int currVM;



// heap used to load the code
extern MemoryHeap *code_mh;

// max number of embryo cells used for code
#define MAX_CODE_SIZE 2048
// buffer to load the code
extern Embryo_Cell code_buff[MAX_CODE_SIZE];





#ifdef __cplusplus
extern "C" {
#endif

	Embryo_Status embryo_program_run(EmbrioVM *vmp, Embryo_Function fn);

	void embrioInit(void);
	Thread *vmStart(EmbrioVM *vm, tprio_t prio);
	void embrio_vm_sleep(EmbrioVM *vmp);
	void embrio_vm_resume(EmbrioVM *vmp);
	void embrio_native_call(EmbrioVM *vmp);
	void embrio_call(EmbrioVM *vmp);
#ifdef __cplusplus
}
#endif

#endif /* _VM_H_ */

/** @} */
