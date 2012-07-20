#include <stdio.h>
#include "Embryo.h"


int main(int argc,char *argv[]) {
	Embryo_Program* ep;
	Embryo_Status es;

	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return -1;
	}

	embryo_init();
	ep = embryo_program_load(argv[1]);
	printf("strlen(): %d\n", embryo_program_function_find(ep, "strlen"));
	printf("printXXX(): %d\n", embryo_program_function_find(ep, "printXXX"));
	embryo_program_vm_push(ep);
	es = embryo_program_run(ep, EMBRYO_FUNCTION_MAIN);
	embryo_program_free(ep);

	printf("Return code: %d\n", es);

	embryo_shutdown();

	return 0;
}
