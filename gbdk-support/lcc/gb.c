/* Unix */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#ifdef __WIN32__
#include <windows.h>
#endif

#include "gb.h"

#ifndef GBDKLIBDIR
#define GBDKLIBDIR "\\gbdk\\"
#endif

extern char *progname;
extern char * strsave(const char *);

#define ARRAY_LEN(A) (sizeof(A) / sizeof(A[0]))

typedef struct {
	const char *port;
	const char *plat;
	const char *default_plat;
	const char *rom_extension;
	const char *cpp;
	const char *include;
	const char *com;
	const char *as;
	const char *bankpack;
	const char *ld;
	const char *ihxcheck;
	const char *mkbin;
} CLASS;

static struct {
	const char *name;
	const char *val;
} _tokens[] = {
		// expandable string tokens used in "CLASS" command strings
		{ "port",		"gbz80" },
		{ "plat",		"gb" },
		{ "sdccdir", "%bindir%"},
		{ "cpp",		"%sdccdir%sdcpp" },
		{ "cppdefault", 	"-Wall -DSDCC=1 -DSDCC_PORT=%port% "
			"-DSDCC_PLAT=%plat% -D%cppmodel%"
		},
		{ "cppmodel",	"SDCC_MODEL_SMALL" },
		{ "includedefault",	"-I%includedir%" },
		{ "includedir", 	"%prefix%include" },
		{ "prefix",		GBDKLIBDIR },
		{ "comopt",		"--noinvariant --noinduction" },
		{ "commodel", 	"small" },
		{ "com",		"%sdccdir%sdcc" },
		{ "comflag",	"-c"},
		{ "comdefault",	"-mgbz80 --no-std-crt0 --fsigned-char --use-stdout "
					    " -D__PORT_%port% -D__TARGET_%plat% "},
		/* asdsgb assembler defaults:
			-p: disable pagination
			-o: create object file
			-g: make undef symbols global
			-n: defer symbol resolving to link time (autobanking relies on this) [requires sdcc 12238+]
		*/
		{ "asdefault",	"-pogn -I%libdir%%plat%" },
		{ "as_gb",		"%sdccdir%sdasgb" },
		{ "as_z80",		"%sdccdir%sdasz80" },
		{ "bankpack", "%bindir%bankpack" },
		{ "ld_gb",		"%sdccdir%sdldgb" },
		{ "ld_z80",		"%sdccdir%sdldz80" },
		{ "libdir",		"%prefix%lib/%libmodel%/asxxxx/" },
		{ "libmodel",	"small" },
#ifndef GBDKBINDIR
		{ "bindir",		"%prefix%bin/" },
#else
		{ "bindir",		GBDKBINDIR },
#endif
		{ "ihxcheck", "%bindir%ihxcheck" },
		{ "mkbin", "%sdccdir%makebin" },
		{ "crt0dir", "%libdir%%plat%/crt0.o"},
		{ "libs_include", "-k %libdir%%port%/ -l %port%.lib -k %libdir%%plat%/ -l %plat%.lib"}
};

#define NUM_TOKENS	(sizeof(_tokens)/sizeof(_tokens[0]))

static char *getTokenVal(const char *key)
{
	int i;
	for (i = 0; i < NUM_TOKENS; i++) {
		if (!strcmp(_tokens[i].name, key))
			return strdup(_tokens[i].val);
	}
	assert(0);
	return NULL;
}

static void setTokenVal(const char *key, const char *val)
{
	int i;
	for (i = 0; i < NUM_TOKENS; i++) {
		if (!strcmp(_tokens[i].name, key)) {
			_tokens[i].val = strdup(val);
			return;
		}
	}
	assert(0);
}

/*
$1 are extra parameters passed using -W
$2 is the list of objects passed as parameters
$3 is the output file
*/
static CLASS classes[] = {
		// GB
		{ "gbz80",		// port
			"gb",		// plat
			"gb",		// default_plat
			EXT_GB,		// ROM file extension
			"%cpp% %cppdefault% -DGB=1 -DGAMEBOY=1 -DINT_16_BITS $1 $2 $3",
			"%includedefault%",
			"%com% %comdefault% -Wa%asdefault% -DGB=1 -DGAMEBOY=1 -DINT_16_BITS $1 %comflag% $2 -o $3",
			"%as_gb% %asdefault% $1 $3 $2",
			"%bankpack% $1 $2",
			"%ld_gb% -n -i $1 %libs_include% $3 %crt0dir% $2",
			"%ihxcheck% $2 $1",
			"%mkbin% -Z $1 $2 $3"
		},
		// AP
		{ "gbz80",		// port
			"ap",		// plat
			"ap",		// default_plat
			EXT_AP,		// ROM file extension
			"%cpp% %cppdefault% -DGB=1 -DGAMEBOY=1 -DINT_16_BITS $1 $2 $3",
			"%includedefault%",
			"%com% %comdefault% -Wa%asdefault% -DGB=1 -DGAMEBOY=1 -DINT_16_BITS $1 %comflag% $2 -o $3",
			"%as_gb% %asdefault% $1 $3 $2",
			"%bankpack% $1 $2",
			"%ld_gb% -n -i $1 %libs_include% $3 %crt0dir% $2",
			"%ihxcheck% $2 $1",
			"%mkbin% -Z $1 $2 $3"
		},
		// SMS
		{ "z80",		// port
			"sms",		// plat
			"sms",		// default_plat
			EXT_SMS,	// ROM file extension
			"%cpp% %cppdefault% -DINT_16_BITS $1 $2 $3",
			"%includedefault%",
			"%com% %comdefault% -Wa%asdefault% -DINT_16_BITS $1 %comflag% $2 -o $3",
			"%as_z80% %asdefault% $1 $3 $2",
			"",			// bankpack command: turned off for this port:platform
			"%ld_z80% -n -i $1 %libs_include% $3 %crt0dir% $2",
			"%ihxcheck% $2 $1",
			"%mkbin% -S $1 $2 $3"
		},
		// GG
		{ "z80",		// port
			"gg",		// plat
			"gg",		// default_plat
			EXT_GG,		// ROM file extension
			"%cpp% %cppdefault% -DINT_16_BITS $1 $2 $3",
			"%includedefault%",
			"%com% %comdefault% -Wa%asdefault% -DINT_16_BITS $1 %comflag% $2 -o $3",
			"%as_z80% %asdefault% $1 $3 $2",
			"",			// bankpack command: turned off for this port:platform
			"%ld_z80% -n -i $1 %libs_include% $3 %crt0dir% $2",
			"%ihxcheck% $2 $1",
			"%mkbin% -S $1 $2 $3"
		}
};

static CLASS *_class = &classes[0];

#define NUM_CLASSES 	(sizeof(classes)/sizeof(classes[0]))

static int setClass(const char *port, const char *plat)
{
	int i;
	for (i = 0; i < NUM_CLASSES; i++) {
		if (!strcmp(classes[i].port, port)) {
			if (plat && classes[i].plat && !strcmp(classes[i].plat, plat)) {
				_class = classes + i;
				return 1;
			}
			else if (!classes[i].plat || !plat) {
				_class = classes + i;
				return 1;
			}
		}
	}
	return 0;
}

/* Algorithim
	 while (chars in string)
		if space, concat on end
	if %
		Copy off what we have sofar
		Call ourself on value of token
		Continue scanning
*/

/* src is destroyed */
static char **subBuildArgs(char **args, char *template)
{
	char *src = template;
	char *last = src;
	static int quoting = 0;

	/* Shared buffer between calls of this function. */
	static char buffer[128];
	static int indent = 0;

	indent++;
	while (*src) {
		if (isspace(*src) && !quoting) {
			/* End of set - add in the command */
			*src = '\0';
			strcat(buffer, last);
			*args = strdup(buffer);
			buffer[0] = '\0';
			args++;
			last = src + 1;
		}
		else if (*src == '%') {
			/* Again copy off what we already have */
			*src = '\0';
			strcat(buffer, last);
			*src = '%';
			src++;
			last = src;
			while (*src != '%') {
				if (!*src) {
					/* End of string without closing % */
					assert(0);
				}
				src++;
			}
			*src = '\0';
			/* And recurse :) */
			args = subBuildArgs(args, getTokenVal(last));
			*src = '%';
			last = src + 1;
		}
		else if (*src == '\"') {
			quoting = !quoting;
		}
		src++;
	}
	strcat(buffer, last);
	if (indent == 1) {
		*args = strdup(buffer);
		args++;
		buffer[0] = '\0';
	}

	indent--;
	return args;
}

static void buildArgs(char **args, const char *template)
{
	char *s = strdup(template);
	char **last;
	last = subBuildArgs(args, s);
	*last = NULL;
}

// If order is changed here, file type handling MUST be updated
// in lcc.c: "switch (suffix(name, suffixes, 5)) {"
char *suffixes[] = {
    EXT_C,               // 0
    EXT_I,               // 1
    EXT_ASM ";" EXT_S,   // 2
    EXT_O   ";" EXT_OBJ, // 3
    EXT_IHX,             // 4
    EXT_GB,              // 5
    0
};

char inputs[256] = "";

char *cpp[256];
char *include[256];
char *com[256] = { "", "", "" };
char *as[256];
char *ihxcheck[256];
char *ld[256];
char *bankpack[256];
char *mkbin[256];
char *rom_extension;

const char *starts_with(const char *s1, const char *s2)
{
	if (!strncmp(s1, s2, strlen(s2))) {
		return s1 + strlen(s2);
	}
	return NULL;
}

int option(char *arg) {
	const char *tail;
	if ((tail = starts_with(arg, "--prefix="))) {
		/* Go through and set all of the paths */
		setTokenVal("prefix", tail);
		return 1;
	}
	else if ((tail = starts_with(arg, "--gbdklibdir="))) {
		setTokenVal("libdir", tail);
		return 1;
	}
	else if ((tail = starts_with(arg, "--gbdkincludedir="))) {
		setTokenVal("includedir", tail);
		return 1;
	}
	else if ((tail = starts_with(arg, "--sdccbindir="))) {
		// Allow to easily run with external SDCC snapshot / release
		setTokenVal("sdccdir", tail);
		return 1;
	}
	else if ((tail = starts_with(arg, "-S"))) {
		// -S is compile to ASM only
		// When composing the compile stage, swap in of -S instead of default -c
		setTokenVal("comflag", "-S");
	}
	else if ((tail = starts_with(arg, "-no-crt"))) {
		// When composing link stage, clear out crt0dir path
		setTokenVal("crt0dir", "");
	}
	else if ((tail = starts_with(arg, "-no-libs"))) {
		// When composing link stage, clear out crt0dir path
		setTokenVal("libs_include", "");
	}
	else if ((tail = starts_with(arg, "-m"))) {
		char word_count = 0;
		char * p_str = strtok( strsave(tail),":"); // Copy arg str so it doesn't get unmodified by strtok()
		char * words[3]; // +1 in size of expected number of entries to detect excess

		// Split string into words separated by ':' chars (expecting PORT and PLAT)
		while (p_str != NULL) {
			words[word_count++] = p_str;
			p_str = strtok(NULL, ":");
			if (word_count >= ARRAY_LEN(words)) break;
		}

		// Requires both PORT and PLAT, must match a valid setClass entry.
		if (word_count == 2) {
			setTokenVal("port", words[0]);
			setTokenVal("plat", words[1]);
			if (!setClass(words[0], words[1])) {
				fprintf(stderr, "Error: %s: unrecognised PORT:PLATFORM from %s:%s\n", progname, words[0], words[1]);
				exit(-1);
			}
		} else {
			fprintf(stderr, "Error: -m requires both/only PORT and PLATFORM values (ex: -mgbz80:gb) : %s\n", arg);
			exit(-1);
		}

		return 1;
	}
	else if ((tail = starts_with(arg, "--model-"))) {
		if (!strcmp(tail, "small")) {
			setTokenVal("commodel", "small");
			setTokenVal("libmodel", "small");
			setTokenVal("cppmodel", "SDCC_MODEL_SMALL");
			return 1;
		}
		else if (!strcmp(tail, "medium")) {
			setTokenVal("commodel", "medium");
			setTokenVal("libmodel", "medium");
			setTokenVal("cppmodel", "SDCC_MODEL_MEDIUM");
			return 1;
		}
	}
	return 0;
}

void finalise(void)
{
	if (!_class->plat) {
		setTokenVal("plat", _class->default_plat);
	}
	buildArgs(cpp, _class->cpp);
	buildArgs(include, _class->include);
	buildArgs(com, _class->com);
	buildArgs(as, _class->as);
	buildArgs(bankpack, _class->bankpack);
	buildArgs(ld, _class->ld);
	buildArgs(ihxcheck, _class->ihxcheck);
	buildArgs(mkbin, _class->mkbin);
	rom_extension = strdup(_class->rom_extension);
}

void set_gbdk_dir(char* argv_0)
{
	char buf[1024 - 2]; // Path will get quoted below, so reserve two characters for them
#ifdef __WIN32__
	char slash = '\\';
	if (GetModuleFileName(NULL,buf, sizeof(buf)) == 0)
	{
		return;
	}
#else
	char slash = '/';
	strncpy(buf, argv_0, sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';
#endif

	// Strip of the trailing GBDKDIR/bin/lcc.exe and use it as the prefix.
	char *p = strrchr(buf, slash);
	if (p) {
		while(p != buf && *(p - 1) == slash) //Fixes https://github.com/Zal0/gbdk-2020/issues/29
			-- p;
		*p = '\0';

		p = strrchr(buf, slash);
		if (p) {
			*++p = '\0';
			char quotedBuf[1024];
			snprintf(quotedBuf, sizeof(quotedBuf), "\"%s\"", buf);
			setTokenVal("prefix", quotedBuf);
		}
	}
}
