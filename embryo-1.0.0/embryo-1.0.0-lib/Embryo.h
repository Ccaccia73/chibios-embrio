#ifndef _EMBRYO_H
#define _EMBRYO_H


// #include "embryo_private.h"

#ifdef _CHIBIOS_VM_
	#include "ch.h"
	#include "hal.h"
	#define DEBUG_PRINT(_A)
#else
	#define DEBUG_PRINT(_A) printf _A
#endif

#ifdef EAPI
	#undef EAPI
#endif

#define EAPI // __attribute__ ((visibility("default")))
/*
#ifdef _WIN32
	#ifdef EFL_EMBRYO_BUILD
		#ifdef DLL_EXPORT
			#define EAPI __declspec(dllexport)
		#else
			#define EAPI
		#endif // DLL_EXPORT
	#else
		#define EAPI // __declspec(dllimport)
	#endif // ! EFL_EMBRYO_BUILD
#else
	#ifdef __GNUC__
		#if __GNUC__ >= 4
			#define EAPI __attribute__ ((visibility("default")))
		#else
			#define EAPI
		#endif
	#else
		#define EAPI
	#endif
#endif // ! _WIN32
*/

/*
#ifdef  __cplusplus
extern "C" {
#endif
*/
#define VMAJ 1
#define VMIN 0
#define VMIC 0
#define VREV 0

#define EMBRYO_VERSION_MAJOR 1
#define EMBRYO_VERSION_MINOR 0

// #define _CHIBIOS_VM_

typedef struct _Embryo_Version
{
	int major;
	int minor;
	int micro;
	int revision;
} Embryo_Version;

EAPI extern Embryo_Version *embryo_version;

/* potential error values */
typedef enum _Embryo_Error
{
	EMBRYO_ERROR_NONE,
	/* reserve the first 15 error codes for exit codes of the abstract machine */
	EMBRYO_ERROR_EXIT,         /** Forced exit */
	EMBRYO_ERROR_ASSERT,       /** Assertion failed */
	EMBRYO_ERROR_STACKERR,     /** Stack/heap collision */
	EMBRYO_ERROR_BOUNDS,       /** Index out of bounds */
	EMBRYO_ERROR_MEMACCESS,    /** Invalid memory access */
	EMBRYO_ERROR_INVINSTR,     /** Invalid instruction */
	EMBRYO_ERROR_STACKLOW,     /** Stack underflow */
	EMBRYO_ERROR_HEAPLOW,      /** Heap underflow */
	EMBRYO_ERROR_CALLBACK,     /** No callback, or invalid callback */
	EMBRYO_ERROR_NATIVE,       /** Native function failed */
	EMBRYO_ERROR_DIVIDE,       /** Divide by zero */
	EMBRYO_ERROR_SLEEP,        /** Go into sleepmode - code can be restarted */

	EMBRYO_ERROR_MEMORY = 16,  /** Out of memory */
	EMBRYO_ERROR_FORMAT,       /** Invalid file format */
	EMBRYO_ERROR_VERSION,      /** File is for a newer version of the Embryo_Program */
	EMBRYO_ERROR_NOTFOUND,     /** Function not found */
	EMBRYO_ERROR_INDEX,        /** Invalid index parameter (bad entry point) */
	EMBRYO_ERROR_DEBUG,        /** Debugger cannot run */
	EMBRYO_ERROR_INIT,         /** Embryo_Program not initialized (or doubly initialized) */
	EMBRYO_ERROR_USERDATA,     /** Unable to set user data field (table full) */
	EMBRYO_ERROR_INIT_JIT,     /** Cannot initialize the JIT */
	EMBRYO_ERROR_PARAMS,       /** Parameter error */
	EMBRYO_ERROR_DOMAIN,       /** Domain error, expression result does not fit in range */
} Embryo_Error;

/* program run return values */
typedef enum _Embryo_Status
{
	EMBRYO_PROGRAM_FAIL = 0,
	EMBRYO_PROGRAM_OK = 1,
	EMBRYO_PROGRAM_SLEEP = 2,
	EMBRYO_PROGRAM_BUSY = 3,
	EMBRYO_PROGRAM_TOOLONG = 4
} Embryo_Status;

typedef unsigned int		Embryo_UCell;
typedef int					Embryo_Cell;
/** An invalid cell reference */
#define EMBRYO_CELL_NONE	0x7fffffff

typedef struct _Embryo_Program		Embryo_Program;
typedef int							Embryo_Function;
/* possible function type values that are enumerated */
#define EMBRYO_FUNCTION_NONE 0x7fffffff /* An invalid/non existent function */
#define EMBRYO_FUNCTION_MAIN -1         /* Start at program entry point */
#define EMBRYO_FUNCTION_CONT -2         /* Continue from last address */




typedef union
{
	float       f;
	Embryo_Cell c;
} Embryo_Float_Cell;

#if defined _MSC_VER || defined __SUNPRO_C
/** Float to Embryo_Cell */
# define EMBRYO_FLOAT_TO_CELL(f) (((Embryo_Float_Cell *)&(f))->c)
/** Embryo_Cell to float */
# define EMBRYO_CELL_TO_FLOAT(c) (((Embryo_Float_Cell *)&(c))->f)
#else
/** Float to Embryo_Cell */
# define EMBRYO_FLOAT_TO_CELL(f) ((Embryo_Float_Cell) f).c
/** Embryo_Cell to float */
# define EMBRYO_CELL_TO_FLOAT(c) ((Embryo_Float_Cell) c).f
#endif

EAPI int              embryo_init(void);
EAPI int              embryo_shutdown(void);

EAPI Embryo_Program  *embryo_program_new(void *data, int size);
#ifdef _NO_
EAPI Embryo_Program  *embryo_program_const_new(void *data, int size);
#endif
EAPI Embryo_Program  *embryo_program_load(const char *file);

#ifdef _CHIBIOS_VM_
	Embryo_Program *embryo_program_load_local(unsigned char *start, unsigned char *end, unsigned char *size, BaseChannel *chp, bool_t verbose);
#endif

EAPI void             embryo_program_free(Embryo_Program *ep);
EAPI void             embryo_program_native_call_add(Embryo_Program *ep, const char *name, Embryo_Cell (*func) (Embryo_Program *ep, Embryo_Cell *params));
EAPI void             embryo_program_vm_reset(Embryo_Program *ep);
EAPI void             embryo_program_vm_push(Embryo_Program *ep);
EAPI void             embryo_program_vm_pop(Embryo_Program *ep);
EAPI void             embryo_swap_16(unsigned short *v);
EAPI void             embryo_swap_32(unsigned int *v);
EAPI Embryo_Function  embryo_program_function_find(Embryo_Program *ep, const char *name);
EAPI Embryo_Cell      embryo_program_variable_find(Embryo_Program *ep, const char *name);
EAPI int              embryo_program_variable_count_get(Embryo_Program *ep);
EAPI Embryo_Cell      embryo_program_variable_get(Embryo_Program *ep, int num);
EAPI void             embryo_program_error_set(Embryo_Program *ep, Embryo_Error error);
EAPI Embryo_Error     embryo_program_error_get(Embryo_Program *ep);
EAPI void             embryo_program_data_set(Embryo_Program *ep, void *data);
EAPI void            *embryo_program_data_get(Embryo_Program *ep);
EAPI const char      *embryo_error_string_get(Embryo_Error error);
EAPI int              embryo_data_string_length_get(Embryo_Program *ep, Embryo_Cell *str_cell);
EAPI void             embryo_data_string_get(Embryo_Program *ep, Embryo_Cell *str_cell, char *dst);
EAPI void             embryo_data_string_set(Embryo_Program *ep, const char *src, Embryo_Cell *str_cell);
EAPI Embryo_Cell     *embryo_data_address_get(Embryo_Program *ep, Embryo_Cell addr);
EAPI Embryo_Cell      embryo_data_heap_push(Embryo_Program *ep, int cells);
EAPI void             embryo_data_heap_pop(Embryo_Program *ep, Embryo_Cell down_to);
EAPI int              embryo_program_recursion_get(Embryo_Program *ep);

#ifdef _CHIBIOS_VM_
 	// moved in embrio.h
	// EAPI Embryo_Status    embryo_program_run(EmbrioVM *vmp, Embryo_Function fn);
#else
	EAPI Embryo_Status    embryo_program_run(Embryo_Program *ep, Embryo_Function func);
#endif
EAPI Embryo_Cell      embryo_program_return_value_get(Embryo_Program *ep);
EAPI void             embryo_program_max_cycle_run_set(Embryo_Program *ep, int max);
EAPI int              embryo_program_max_cycle_run_get(Embryo_Program *ep);

#if !(defined EMBRYO_PARAMS) && (defined _CHIBIOS_VM_)

#else
	EAPI int          embryo_parameter_cell_push(Embryo_Program *ep, Embryo_Cell cell);
	EAPI int          embryo_parameter_string_push(Embryo_Program *ep, const char *str);
	EAPI int          embryo_parameter_cell_array_push(Embryo_Program *ep, Embryo_Cell *cells, int num);
#endif

// Memory Pool for Embryo_Program
// static MemoryPool *embryo_program_MP;


/*
#ifdef  __cplusplus
}
#endif
*/

#endif
