/**
 * @file vm.c
 * @brief Virtual Machine code.
 * @addtogroup virtual_machine
 * @{
 */

#include "ch.h"
#include "embrio.h"
#include "embrio_private.h"
#include "embryo_private.h"

static msg_t vm_thread(void *p) {
	msg_t msg = RDY_OK;
	Embryo_Status es;
	EmbrioVM *vmp = (EmbrioVM *)p;
	int tmp;

	embryo_program_vm_push(vmp->ep);
	//  tmp = embryo_program_function_find(vmp->ep, "@test");
	//  tmp = embryo_program_function_find(vmp->ep, "@event");
	es = embryo_program_run(vmp, EMBRYO_FUNCTION_MAIN);
	//  es = embryo_program_run(vmp, embryo_program_function_find(vmp->ep, "@event"));

	while (TRUE) {

	}

	return msg;
}

/**
 * @brief Virtual Machine initialization.
 */
void embrioInit(void) {

	int i;

	// initialize and preload the blocks
	// Embryo Program
	chPoolInit( EP_mp, MAX_EMBRIO_VM_NUM * sizeof(Embryo_Program), NULL);

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(EP_mp, (void*)&EP_pool[i]);
	}

	// Embrio Virtual Machine
	chPoolInit(EVM_mp, MAX_EMBRIO_VM_NUM * sizeof(EmbrioVM), NULL);

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(EVM_mp, (void*)&EVM_pool[i]);
	}

	// Stack Top: the dimension of stack top is an int
	chPoolInit(Estp_mp, MAX_EMBRIO_VM_NUM * sizeof(int), NULL);

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(Estp_mp, (void*)&Estp_pool[i]);
	}

	// initialize a memory heap to put the code
	chHeapInit(code_mh, (void*)code_buff, MAX_CODE_SIZE * sizeof(Embryo_Cell) );

	// initialize a memory heap to load the program (the heap is freed after the code is copied
	chHeapInit(prog_mh, (void*)prog_buff, MAX_CODE_SIZE * sizeof(Embryo_Cell) );

	// initialize a memory heap to put the native calls
	chHeapInit(nc_mh, (void*)nc_buff, MAX_NATIVE_CALLS * sizeof(Embryo_Native));

}

/*
	vm->ep = embryo_program_load_debug(s);
	vm->ep->max_run_cycles = 0;
	vmCreate(vm, NORMALPRIO);
 */

Thread *vmStart(EmbrioVM *vm, tprio_t prio) {
	vm->state = EMBRIOVM_RUN;
	return chThdCreateFromHeap(NULL, 4096, prio, vm_thread, (void *)vm);
}


void embrio_vm_sleep(EmbrioVM *vmp) {

	vmp->state = EMBRIOVM_SLEEP;
	vmp->hook = NULL;
	vmp->hook_params = NULL;

	chThdSleep(TIME_INFINITE);
}

void embrio_vm_resume(EmbrioVM *vmp) {
	//  if(vmp->state != EMBRIOVM_SLEEP)
	//	return;

	vmp->state = EMBRIOVM_RUN;

	chThdResume(vmp->tp);
}

void embrio_native_call(EmbrioVM *vmp) {

	_embryo_native_call(vmp->ep, vmp->hook_params, &(vmp->ep->pri), (Embryo_Cell *)(vmp->ep->data + (int)vmp->ep->stk));
	vmp->hook = NULL;
	vmp->hook_params = NULL;
}

void embrio_call(EmbrioVM *vmp) {
	EmbrioVM tmpvm;
	Embryo_Program *ep;
	Embryo_Function func;
	Embryo_Status es;

	vmp->hook = NULL;
	vmp->hook_params = NULL;
	tmpvm = *vmp;
	//	memcpy(&tmpvm, vmp, sizeof(EmbrioVM));
	ep = tmpvm.ep;
	func = embryo_program_function_find(ep, "@event");
	es = embryo_program_run(&tmpvm, func);


}

/** @} */
