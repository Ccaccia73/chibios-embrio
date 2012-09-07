/**
 * @file vm.c
 * @brief Virtual Machine code.
 * @addtogroup virtual_machine
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "board.h"
#include "embrio.h"
#include "embrio_private.h"
#include "embryo_private.h"
#include "chprintf.h"

// Memory pools (defined extern in embrio.h)
MemoryPool EP_mp;
Embryo_Program EP_pool[MAX_EMBRIO_VM_NUM];

MemoryPool EVM_mp;
EmbrioVM EVM_pool[MAX_EMBRIO_VM_NUM];

int currVM = 0;

// memory pool for threads
MemoryPool THD_mp;

/// fixme: static allocation, should depend on MAX_EMBRIO_VM_NUM

struct {
	WORKING_AREA(T0, THD_STACK_SIZE);
	WORKING_AREA(T1, THD_STACK_SIZE);
	// WORKING_AREA(T2, THREADS_STACK_SIZE);
	// WORKING_AREA(T3, THREADS_STACK_SIZE);
	// WORKING_AREA(T4, THREADS_STACK_SIZE);
} THD_vm_wa;

void * ROMCONST thd_vm_wa_p[2] = {THD_vm_wa.T0, THD_vm_wa.T1};

// global Memory Heap for embrio elements
MemoryHeap embrio_mh;

// 4kb of buffer for embrio heap elements
stkalign_t embrio_buff[EMBRIO_HEAP_SIZE  / sizeof(stkalign_t)];

EmbrioVMManager *vm_man;
EmbrioVM *vm[MAX_EMBRIO_VM_NUM];


static msg_t vm_thread(void *p) {

	chRegSetThreadName("VM");
	// msg_t msg = RDY_OK;
	Embryo_Status es;
	EmbrioVM *vmp = (EmbrioVM *)p;
	int tmp;

	embryo_program_vm_push(vmp->ep);

	/* hello2 TEST OK
	// verify that we found the functions
	tmp = embryo_program_function_find(vmp->ep, "@test");

	if(tmp != EMBRYO_FUNCTION_NONE){
		chprintf((BaseChannel*)&SD3,"t OK ahi\r\n");
	}else{
		chprintf((BaseChannel*)&SD3,"t KO exp\r\n");
	}

	tmp = embryo_program_function_find(vmp->ep, "@event");

	if(tmp != EMBRYO_FUNCTION_NONE){
		chprintf((BaseChannel*)&SD3,"e OK exp\r\n");
	}else{
		chprintf((BaseChannel*)&SD3,"e KO ahi\r\n");
	}
	*/


	es = embryo_program_run(vmp, EMBRYO_FUNCTION_MAIN);
	//  es = embryo_program_run(vmp, embryo_program_function_find(vmp->ep, "@event"));

	chprintf((BaseChannel*)&SD3,"es: %d\r\n",(int)es);

	/*
	while (TRUE) {
	    palClearPad(GPIOC, GPIOC_LED_STATUS2);
	    chThdSleepMilliseconds(500);
	    palSetPad(GPIOC, GPIOC_LED_STATUS2);
	    chThdSleepMilliseconds(500);
	}
	*/
	return (msg_t)es;
}

/**
 * @brief Virtual Machine initialization.
 */
void embrioInit(void) {

	embrioPoolsSetup();

	embrioHeapSetup();

	embrioPoolsPrealloc();

	embrioHeapAlloc();

}


void embrioPoolsSetup(void){
	// initialize and preload the blocks
	// Embryo Program


	chPoolInit( &EP_mp, sizeof(Embryo_Program), NULL);

	// Embrio Virtual Machine
	chPoolInit( &EVM_mp, sizeof(EmbrioVM), NULL);

	// thread used for VMs
	chPoolInit( &THD_mp, THD_WA_SIZE(THD_STACK_SIZE), NULL);

}

void embrioHeapSetup(void){
	// initialize a memory heap for embrio
	chHeapInit(&embrio_mh, (void*)embrio_buff, sizeof(embrio_buff) );

}

void embrioPoolsPrealloc(void){

	int i;
	// test
	// void *test_alloc;

	/// todo: optimize!

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(&EP_mp, (void*)(&EP_pool[i]));
	}

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(&EVM_mp, (void*)(&EVM_pool[i]));
	}

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(&THD_mp, thd_vm_wa_p[i]);
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

void embrioHeapAlloc(void){

	// allocate memory for VM manager structure
	vm_man = (EmbrioVMManager*)chHeapAlloc(&embrio_mh, sizeof(EmbrioVMManager));

}

/*
	vm->ep = embryo_program_load_debug(s);
	vm->ep->max_run_cycles = 0;
	vmCreate(vm, NORMALPRIO);
 */

// inserts new virtual machine in linked list of machines
void embrioVMMinsert(EmbrioVMManager *vm_man, EmbrioVM *new_vm){
	new_vm->vm_next = (void*)(vm_man->vm_first);
	vm_man->vm_first = new_vm;
	vm_man->vm_count++;
}

Thread *vmStart(EmbrioVM *vm, tprio_t prio) {

	Thread* thdp = NULL;

	vm->state = EMBRIOVM_RUN;

	thdp = chThdCreateFromMemoryPool( &THD_mp, prio, vm_thread, (void *)vm);

	/*
	if(thdp == NULL){
		chprintf((BaseChannel*)&SD3,"Thd VM 0 ep NO\r\n");
	}else{
		chprintf((BaseChannel*)&SD3,"Thd VM 0 ep OK\r\n");
	}
	*/

	return thdp;
}


void embrio_vm_sleep(EmbrioVM *vmp) {

	vmp->state = EMBRIOVM_SLEEP;
	vmp->hook = NULL;
	vmp->hook_params = EMBRYO_FUNCTION_NONE;

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
	vmp->hook_params = EMBRYO_FUNCTION_NONE;
}

void embrio_call(EmbrioVM *vmp) {
	EmbrioVM tmpvm;
	Embryo_Program *ep;
	Embryo_Function func;
	Embryo_Status es;

	vmp->hook = NULL;
	vmp->hook_params = EMBRYO_FUNCTION_NONE;
	tmpvm = *vmp;
	//	memcpy(&tmpvm, vmp, sizeof(EmbrioVM));
	ep = tmpvm.ep;
	func = embryo_program_function_find(ep, "@event");
	es = embryo_program_run(&tmpvm, func);


}

/** @} */
