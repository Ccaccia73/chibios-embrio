#ifndef _EMBRYO_CC_PREFIX_H_
#define _EMBRYO_CC_PREFIX_H_

int         e_prefix_determine(char *argv0);
void        e_prefix_shutdown(void);
void        e_prefix_fallback(void);
const char *e_prefix_get(void);
const char *e_prefix_bin_get(void);
const char *e_prefix_data_get(void);
const char *e_prefix_lib_get(void);


#define PACKAGE_BIN_DIR "./"
#define PACKAGE_DATA_DIR "./"
#define PACKAGE_LIB_DIR "../embryo-1.0.0-lib"

// #define PATH_MAX 60

#endif
