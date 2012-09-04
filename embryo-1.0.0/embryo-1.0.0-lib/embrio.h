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

// array defining the stack top and memory pool
extern MemoryPool Estp_mp;
extern int Estp_pool[MAX_EMBRIO_VM_NUM];

// variable defining the actual VM used
extern int currVM;


// max number of embryo cells used for code
#define MAX_CODE_SIZE 256


// heap and buffer used to load the code
extern MemoryHeap code_mh;
extern Embryo_Cell code_buff[MAX_CODE_SIZE];

// heap and buffer to load the program
extern MemoryHeap prog_mh;
extern Embryo_Cell prog_buff[MAX_CODE_SIZE];


#define MAX_NATIVE_CALLS 128


// heap and buffer to use native calls
extern MemoryHeap nc_mh;
extern Embryo_Native nc_buff[MAX_NATIVE_CALLS];

// virtual machine manager
extern MemoryHeap EVMM_mh;
extern EmbrioVMManager EVMM_buff;


#ifdef __cplusplus
extern "C" {
#endif

	Embryo_Status embryo_program_run(EmbrioVM *vmp, Embryo_Function fn);

	void embrioInit(void);

	void embrioPoolsSetup(void);
	void embrioHeapsSetup(void);

	void embrioPoolsPrealloc(void);

	void embrioVMMinsert(EmbrioVMManager *vm_man, EmbrioVM *new_vm);

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
