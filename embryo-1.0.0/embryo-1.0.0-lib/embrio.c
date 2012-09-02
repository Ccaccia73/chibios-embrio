/**
 * @file vm.c
 * @brief Virtual Machine code.
 * @addtogroup virtual_machine
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "embrio.h"
#include "embrio_private.h"
#include "embryo_private.h"

// Memory pools (defined extern in embrio.h)
MemoryPool EP_mp;
Embryo_Program EP_pool[MAX_EMBRIO_VM_NUM];

MemoryPool EVM_mp;
EmbrioVM EVM_pool[MAX_EMBRIO_VM_NUM];

MemoryPool Estp_mp;
int Estp_pool[MAX_EMBRIO_VM_NUM];


int currVM = 0;

// Memory heaps
// code
MemoryHeap code_mh;
Embryo_Cell code_buff[MAX_CODE_SIZE];

// program
MemoryHeap prog_mh;
Embryo_Cell prog_buff[MAX_CODE_SIZE];

// native calls
MemoryHeap nc_mh;
Embryo_Native nc_buff[MAX_NATIVE_CALLS];


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


	embrioPoolsSetup();

	embrioHeapsSetup();

	embrioPoolsTest();

}


void embrioPoolsSetup(void){
	// initialize and preload the blocks
	// Embryo Program


	chPoolInit( &EP_mp, sizeof(Embryo_Program), NULL);

	// Embrio Virtual Machine
	chPoolInit( &EVM_mp, sizeof(EmbrioVM), NULL);

	// Stack Top: the dimension of stack top is an int
	chPoolInit( &Estp_mp, sizeof(int), NULL);

}

void embrioHeapsSetup(void){
	// initialize a memory heap to put the code
	chHeapInit(&code_mh, (void*)code_buff, MAX_CODE_SIZE * sizeof(Embryo_Cell) );

	// initialize a memory heap to load the program (the heap is freed after the code is copied
	chHeapInit(&prog_mh, (void*)prog_buff, MAX_CODE_SIZE * sizeof(Embryo_Cell) );

	// initialize a memory heap to put the native calls
	chHeapInit(&nc_mh, (void*)nc_buff, MAX_NATIVE_CALLS * sizeof(Embryo_Native));

}

void embrioPoolsTest(void){

	int i;
	// test
	// void *test_alloc;

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(&EP_mp, (void*)(&EP_pool[i]));
	}


	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(&EVM_mp, (void*)(&EVM_pool[i]));
	}

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(&Estp_mp, (void*)(&Estp_pool[i]));
	}

	/* test
	palClearPad(GPIOC, GPIOC_LED_STATUS2);

	// tests
	test_alloc = chPoolAlloc(&EP_mp);

	if(test_alloc == NULL){
		palClearPad(GPIOC, GPIOC_LED_STATUS2);
	}else{
		palSetPad(GPIOC, GPIOC_LED_STATUS2);
	}

	chPoolFree(&EP_mp, (void*)(&EP_pool[0]));
	*/

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
