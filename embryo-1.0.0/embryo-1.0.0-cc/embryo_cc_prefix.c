#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
// #include <fnmatch.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
// #include <dirent.h>

#ifndef _MSC_VER
# include <unistd.h>
# include <sys/param.h>
#endif

#ifdef HAVE_EVIL
# include <Evil.h>      /* for realpath */
#else
//	#include <pwd.h>
//	#include <grp.h>
//	#include <glob.h>
#endif /* ! HAVE_EVIL */

#ifdef __MacOSX__
# include <mach-o/dyld.h>
#endif

#include "embryo_cc_prefix.h"
#include "embryo_cc_sc.h"

/* local subsystem functions */
static int _e_prefix_share_hunt(void);
static int _e_prefix_fallbacks(void);

#ifdef _ORIGINAL_
	static int _e_prefix_try_proc(void);
#endif

static int _e_prefix_try_argv(char *argv0);
#ifdef __MacOSX__
static int _e_prefix_try_dyld(void);
#endif

/* local subsystem globals */
static char *_exe_path = NULL;
static char *_prefix_path = NULL;
static char *_prefix_path_bin = NULL;
static char *_prefix_path_data = NULL;
static char *_prefix_path_lib = NULL;

#define E_FREE(p) { if (p) {free(p); p = NULL;} }

/*#define PREFIX_CACHE_FILE 1*/
#define SHARE_D "share"DIRSEP_STR"embryo"

#define MAGIC_FILE "include"DIRSEP_STR"default.inc"

#define MAGIC_DAT SHARE_D DIRSEP_STR MAGIC_FILE

/* externally accessible functions */
int e_prefix_determine(char *argv0)
{
	char *p, buf[4096];
	struct stat st;

	e_prefix_shutdown();

	/* if user provides E_PREFIX - then use that or also more specific sub
	 * dirs for bin, lib, data and locale */
	if (getenv("EMBRYO_PREFIX"))
	{
		/* we don't expect to pass here, no environment variable set
		 * but we leave the original code
		 */
		printf("We have an environment variable!");

		_prefix_path = strdup(getenv("EMBRYO_PREFIX"));
		if (getenv("EMBRYO_BIN_DIR"))
			snprintf(buf, sizeof(buf), "%s/bin", getenv("EMBRYO_BIN_DIR"));
		else
			snprintf(buf, sizeof(buf), "%s/bin", _prefix_path);
		_prefix_path_bin = strdup(buf);

		if (getenv("EMBRYO_LIB_DIR"))
			snprintf(buf, sizeof(buf), "%s/lib", getenv("EMBRYO_LIB_DIR"));
		else
			snprintf(buf, sizeof(buf), "%s/lib", _prefix_path);
		_prefix_path_lib = strdup(buf);

		if (getenv("EMBRYO_DATA_DIR"))
			snprintf(buf, sizeof(buf), "%s/"SHARE_D, getenv("EMBRYO_DATA_DIR"));
		else
			snprintf(buf, sizeof(buf), "%s/"SHARE_D, _prefix_path);
		_prefix_path_data = strdup(buf);
		return 1;
	}
	/* no env var - examine process and possible argv0 */
#ifdef _ORIGINAL_
	if (!_e_prefix_try_proc())
	{
#endif
		if (_e_prefix_try_argv(argv0) == 0)
		{
#ifdef __MacOSX__
			if (!_e_prefix_try_dyld())
			{
#endif

				printf("No environment variable, proc filesystem skipped.\n\n");
				printf("Starting fallback procedure...\n\n");


				_e_prefix_fallbacks();
				return 0;
#ifdef __MacOSX__
			}
#endif
#ifdef _ORIGINAL_
		}
#endif
	}
	/* _exe_path is now a full absolute path TO this exe - figure out rest */
	/*   if
	 * exe        = /blah/whatever/bin/exe
	 *   then
	 * prefix     = /blah/whatever
	 * bin_dir    = /blah/whatever/bin
	 * data_dir   = /blah/whatever/share/enlightenment
	 * lib_dir    = /blah/whatever/lib
	 */
	p = strrchr(_exe_path, DIRSEP_CHAR);
	if (p)
	{
		p--;
		while (p >= _exe_path)
		{
			if (*p == DIRSEP_CHAR)
			{
				_prefix_path = malloc(p - _exe_path + 1);

				if (_prefix_path)
				{
					strncpy(_prefix_path, _exe_path, p - _exe_path);

					_prefix_path[p - _exe_path] = 0;

					// printf("prefix path: %s\n",_prefix_path);

					/* bin and lib always together */

					snprintf(buf, sizeof(buf), "%s"DIRSEP_STR"embryo-1.0.0-cc", _prefix_path);

					_prefix_path_bin = strdup(buf);

					// printf("prefix path bin: %s\n",_prefix_path_bin);

					snprintf(buf, sizeof(buf), "%s"DIRSEP_STR"embryo-1.0.0-lib", _prefix_path);

					_prefix_path_lib = strdup(buf);

					// printf("prefix path lib: %s\n",_prefix_path_lib);

					/* check if AUTHORS file is there - then our guess is right */
					snprintf(buf, sizeof(buf), "%s"DIRSEP_STR MAGIC_FILE, _prefix_path_bin);
					// printf("magic file: %s\n",buf);

					if (stat(buf, &st) == 0){
						// printf("Look for default.inc OK!\n");
						// snprintf(buf, sizeof(buf), "%s/"SHARE_D, _prefix_path);
						snprintf(buf, sizeof(buf), "%s", _prefix_path_bin);

						_prefix_path_data = strdup(buf);

						// printf("prefix path data: %s\n",_prefix_path_data);

					}
					/* AUTHORS file not there. time to start hunting! */
					else
					{
						if (_e_prefix_share_hunt())
						{
							return 1;
						}
						else
						{
							e_prefix_fallback();
							return 0;
						}
					}
					return 1;
				}
				else
				{
					e_prefix_fallback();
					return 0;
				}
			}
			p--;
		}
	}
	e_prefix_fallback();
	return 0;
}

void e_prefix_shutdown(void)
{
	E_FREE(_exe_path);
	E_FREE(_prefix_path);
	E_FREE(_prefix_path_bin);
	E_FREE(_prefix_path_data);
	E_FREE(_prefix_path_lib);
}

void e_prefix_fallback(void)
{
	e_prefix_shutdown();
	_e_prefix_fallbacks();
}

const char* e_prefix_get(void)
{
	return _prefix_path;
}

const char* e_prefix_bin_get(void)
{
	return _prefix_path_bin;
}

const char* e_prefix_data_get(void)
{
	return _prefix_path_data;
}

const char* e_prefix_lib_get(void)
{
	return _prefix_path_lib;
}

/* local subsystem functions */
static int _e_prefix_share_hunt(void)
{
	char buf[4096], buf2[4096], *p;
	struct stat st;

	// printf("SHARE HUNT!!!!!!!!!\n\n");

	/* sometimes this isn't the case - so we need to do a more exhaustive search
	 * through more parent and subdirs. hre is an example i have seen:
	 *
	 * /blah/whatever/exec/bin/exe
	 * ->
	 * /blah/whatever/exec/bin
	 * /blah/whatever/common/share/enlightenment
	 * /blah/whatever/exec/lib
	 */

	/* this is pure black magic to try and find data shares */
	/* 2. cache file doesn't exist or is invalid - we need to search - this is
	 * where the real black magic begins */

	/* BLACK MAGIC 1:
	 * /blah/whatever/dir1/bin
	 * /blah/whatever/dir2/share/enlightenment
	 */
	if (!_prefix_path_data)
	{
		DIR                *dirp;
		struct dirent      *dp;

		snprintf(buf, sizeof(buf), "%s", _prefix_path);
		p = strrchr(buf, '/');
		if (p) *p = 0;
		dirp = opendir(buf);
		if (dirp)
		{
			char *file;

			while ((dp = readdir(dirp)))
			{
				if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, "..")))
				{
					file = dp->d_name;
					snprintf(buf2, sizeof(buf2), "%s/%s/"MAGIC_DAT, buf, file);
					if (stat(buf2, &st) == 0)
					{
						snprintf(buf2, sizeof(buf2), "%s/%s/"SHARE_D, buf, file);
						_prefix_path_data = strdup(buf2);
						break;
					}
				}
			}
			closedir(dirp);
		}
	}

	/* BLACK MAGIC 2:
	 * /blah/whatever/dir1/bin
	 * /blah/whatever/share/enlightenment
	 */
	if (!_prefix_path_data)
	{
		snprintf(buf, sizeof(buf), "%s", _prefix_path);
		p = strrchr(buf, '/');
		if (p) *p = 0;
		snprintf(buf2, sizeof(buf2), "%s/"MAGIC_DAT, buf);
		if (stat(buf, &st) == 0)
		{
			snprintf(buf2, sizeof(buf2), "%s/"SHARE_D, buf);
			_prefix_path_data = strdup(buf2);
		}
	}

	/* add more black magic as required as we discover weridnesss - remember
	 * this is to save users having to set environment variables to tell
	 * e where it lives, so e auto-adapts. so these code snippets are just
	 * logic to figure that out for the user
	 */

	/* done. we found it - write cache file */
	if (_prefix_path_data)
	{
		return 1;
	}
	/* fail. everything failed */
	return 0;
}

static int _e_prefix_fallbacks(void)
{
	char *p;

	_prefix_path = strdup(PACKAGE_BIN_DIR);
	p = strrchr(_prefix_path, '/');
	if (p){
		*p = 0;
	}
	_prefix_path_bin    = strdup(PACKAGE_BIN_DIR);
	_prefix_path_data   = strdup(PACKAGE_DATA_DIR);
	_prefix_path_lib    = strdup(PACKAGE_LIB_DIR);
	printf("WARNING: Embryo could not determine its installed prefix\n"
			"         and is falling back on the compiled in default:\n"
			"           %s\n"
			"         You might like to try setting the following environment variables:\n"
			"           EMBRYO_PREFIX  - points to the base prefix of install\n"
			"           EMBRYO_BIN_DIR - optional in addition to E_PREFIX to provide\n"
			"                          a more specific binary directory\n"
			"           EMBRYO_LIB_DIR - optional in addition to E_PREFIX to provide\n"
			"                          a more specific library dir\n"
			"           EMBRYO_DATA_DIR - optional in addition to E_PREFIX to provide\n"
			"                          a more specific location for shared data\n"
			,
			_prefix_path);
	return 1;
}

#ifdef __MacOSX__
static int
_e_prefix_try_dyld(void)
{
	char path[PATH_MAX];
	uint32_t size = sizeof (path);

	if (_NSGetExecutablePath(path, &size) != 0)
		return 0;

	_exe_path = strdup(path);
	return 1;
}
#endif


#ifdef _ORIGINAL_
static int _e_prefix_try_proc(void)
{
	FILE *f;
	char buf[4096];
	void *func = NULL;

	func = (void *)_e_prefix_try_proc;
	f = fopen("/proc/self/maps", "r");
	if (!f) return 0;
	while (fgets(buf, sizeof(buf), f))
	{
		int len;
		char *p, mode[5] = "";
		unsigned long ptr1 = 0, ptr2 = 0;

		len = strlen(buf);
		if (buf[len - 1] == '\n')
		{
			buf[len - 1] = 0;
			len--;
		}
		if (sscanf(buf, "%lx-%lx %4s", &ptr1, &ptr2, mode) == 3)
		{
			if (!strcmp(mode, "r-xp"))
			{
				if (((void *)ptr1 <= func) && (func < (void *)ptr2))
				{
					p = strchr(buf, '/');
					if (p)
					{
						if (len > 10)
						{
							if (!strcmp(buf + len - 10, " (deleted)"))
								buf[len - 10] = 0;
						}
						_exe_path = strdup(p);
						fclose(f);
						return 1;
					}
					else
						break;
				}
			}
		}
	}
	fclose(f);
	return 0;
}
#endif

static int _e_prefix_try_argv(char *argv0)
{
	char *path, *p, *cp, *s;
	int len, lenexe;
	char buf[4096]; // , buf2[4096], buf3[4096];

	// printf("Trying argv:\n");
	// printf("argv0 = %s\n",argv0);

	/* Edit: we try to find here the path of the file: we don't exactly know
	 * how the compiler is called. Wee look for the name and then try to find
	 * the path of the executable, by using realpath in Linux or _fullpath in
	 * Windows.
	 */

	// build the absolute path

	// printf("stringa argv0: %s\n",argv0);

#if (defined __MINGW32__) || (defined __MINGW64__)
	if( _fullpath(buf,argv0,PATH_MAX)){
		p = strrchr(buf,DIRSEP_CHAR);
#else
	if (realpath(argv0, buf)){
		p = strrchr(buf,DIRSEP_CHAR);
#endif
		// printf("Try argv - stringa: %s\n",buf);
		if(p){
			_exe_path = strdup(buf);
			if (access(_exe_path, X_OK) == 0){
				// printf("\nFile is executable.\n");
				return 1;
			}
			else{
				perror ("The following error occurred: ");
			}
			free(_exe_path);
			_exe_path = NULL;
			return 0;
		}else{
			fprintf(stderr,"No directory symbol found!\n");
		}
	}else{
		fprintf(stderr,"No real path found!\n");
	}

	/* 3. argv0 no path - look in PATH */
	fprintf(stderr,"Looking in PATH: tbd!!!\n");
	path = getenv("PATH");
	if (!path) return 0;
	p = path;
	cp = p;
	lenexe = strlen(argv0);
	while ((p = strchr(cp, ':')))
	{
		len = p - cp;
		s = malloc(len + 1 + lenexe + 1);
		if (s)
		{
			strncpy(s, cp, len);
			s[len] = '/';
			strcpy(s + len + 1, argv0);
#if (defined __MINGW32__) || (defined __MINGW64__)
			// printf("fullpath!!\n");
			if( _fullpath(buf,s,PATH_MAX))
#else
			// printf("realpath!!\n");
			if (realpath(s, buf))
#endif
			{
				if (access(buf, X_OK) == 0)
				{
					_exe_path = strdup(buf);
					free(s);
					return 1;
				}
			}
			free(s);
		}
		cp = p + 1;
	}
	len = strlen(cp);
	s = malloc(len + 1 + lenexe + 1);
	if (s)
	{
		strncpy(s, cp, len);
		s[len] = '/';
		strcpy(s + len + 1, argv0);
#if (defined __MINGW32__) || (defined __MINGW64__)
		// printf("fullpath!!\n");
		if( _fullpath(buf,s,PATH_MAX))
#else
		// printf("realpath!!\n");
 		if (realpath(s, buf))
#endif
		{
			if (access(buf, X_OK) == 0)
			{
				_exe_path = strdup(buf);
				free(s);
				return 1;
			}
		}
		free(s);
	}
	/* 4. big problems. arg[0] != executable - weird execution */
	return 0;
}
