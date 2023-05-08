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
#include "targets.h"

#ifndef GBDKLIBDIR
#define GBDKLIBDIR "\\gbdk\\"
#endif

extern char *progname;
extern char * strsave(const char *);

// Set default class as the first entry in classes (Game Boy)
static CLASS *_class = &classes[0];

static struct {
	const char *name;
	const char *val;
} _tokens[] = {
		// Expandable string tokens used in "CLASS" command strings
		{ "port",		"sm83" },  // if default class is ever changed from Game Boy, this default (and plat) may need to be changed to match
		{ "plat",		"gb" },
		{ "sdccdir", 	"%bindir%"},

		// In order for things such as "__SDCC_VERSION_MAJOR" to work with -E preprocess only,
		// the preprocessor needs to be invoked via SDCC instead since that's where those
		// settings are generated. (see sdcc --verbose -E ...)
		// { "cpp",		"%sdccdir%sdcpp" },
		// { "cppdefault", "-Wall -D__PORT_%port% -D__TARGET_%plat%"},
		{ "cpp",		"%com%" },
		{ "cppdefault",	"-E -D__PORT_%port% -D__TARGET_%plat% "},

		{ "includedefault",	"-I%includedir%" },
		{ "includedir", 	"%prefix%include" },
		{ "prefix",		GBDKLIBDIR },
		{ "comopt",		"--noinvariant --noinduction" },
		{ "com",		"%sdccdir%sdcc" },
		{ "comflag",	"-c"},
		{ "comdefault",	"-m%port% --no-std-crt0 --fsigned-char --use-stdout -D__PORT_%port% -D__TARGET_%plat% "},
		/* asdsgb assembler defaults:
			-p: disable pagination
			-o: create object file
			-g: make undef symbols global
			-n: defer symbol resolving to link time (autobanking relies on this) [requires sdcc 12238+]
		*/
		{ "asdefault",	"-pogn -I%libdir%%plat%" },
		{ "as_gb",		"%sdccdir%sdasgb" },
		{ "as_z80",		"%sdccdir%sdasz80" },
		{ "as_6500",	"%sdccdir%sdas6500" },
		{ "bankpack",	"%bindir%bankpack" },
		{ "ld_gb",		"%sdccdir%sdldgb" },
		{ "ld_z80",		"%sdccdir%sdldz80" },
		{ "ld_6808",	"%sdccdir%sdld6808" },
		{ "ld",			"%sdccdir%sdld" },
		{ "libdir",		"%prefix%lib/" },
#ifndef GBDKBINDIR
		{ "bindir",		"%prefix%bin/" },
#else
		{ "bindir",		GBDKBINDIR },
#endif
		{ "ihxcheck",	"%bindir%ihxcheck" },
		{ "mkbin",		"%sdccdir%makebin" },
		{ "crt0dir",	"%libdir%%plat%/crt0.o"},
		{ "libs_include", "-k %libdir%%port%/ -l %port%.lib -k %libdir%%plat%/ -l %plat%.lib"},
				{ "mkcom", "%sdccdir%makecom"}
};

static char *getTokenVal(const char *key)
{
	int i;
	for (i = 0; i < ARRAY_LEN(_tokens); i++) {
		if (!strcmp(_tokens[i].name, key))
			return strdup(_tokens[i].val);
	}
	assert(0);
	return NULL;
}

static void setTokenVal(const char *key, const char *val)
{
	int i;
	for (i = 0; i < ARRAY_LEN(_tokens); i++) {
		if (!strcmp(_tokens[i].name, key)) {
			_tokens[i].val = strdup(val);
			return;
		}
	}
	assert(0);
}

// Sets the local _class to a given Port/Platform from classes[]
static int setClass(const char *port, const char *plat)
{
	int i;
	for (i = 0; i < classes_count; i++) {
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

// TODO: This + suffix() is brittle and hard to read elsewhere. Rewrite it.
//
// If order is changed here, file type handling MUST be updated
// in lcc.c: "switch (suffix(name, suffixes, 5)) {"
//
// suffix() interpretes this as each additional array item being
// part of an inclusively larger set.
char *suffixes[] = {
	EXT_C,					// 0
	EXT_I,			 		// 1
	EXT_ASM ";" EXT_S,		// 2
	EXT_O   ";" EXT_OBJ,	// 3
	EXT_IHX,				// 4
	EXT_GB,					// 5
	0						// 6
};

char inputs[256] = "";

// Todo: Move these into a struct
// (Could use "CLASS" aside from const typing? It's mainly dupe of that anyway)
char *cpp[256];
char *include[256];
char *com[256] = { "", "", "" };
char *as[256];
char *ihxcheck[256];
char *ld[256];
char *bankpack[256];
char *mkbin[256];
char *postproc[256];
char *rom_extension;
arg_entry *llist0_defaults;
int llist0_defaults_len = 0;


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
			// Error out and warn user when old gbz80 PORT name is used instead of sm83
			if (!strcmp("gbz80", words[0])) {
				fprintf(stderr, "Error: %s: old \"gbz80\" SDCC PORT name specified (in \"%s\"). Use \"sm83\" instead. "
								"You must update your build settings.\n", progname, arg);
				exit(-1);
			}

			setTokenVal("port", words[0]);
			setTokenVal("plat", words[1]);
			if (!setClass(words[0], words[1])) {
				fprintf(stderr, "Error: %s: unrecognised PORT:PLATFORM from %s:%s\n", progname, words[0], words[1]);
				exit(-1);
			}
		} else {
			fprintf(stderr, "Error: -m requires both/only PORT and PLATFORM values (ex: -msm83:gb) : %s\n", arg);
			exit(-1);
		}

		return 1;
	}
	return 0;
}

// Build the port/platform specific toolchain command strings
// and apply any related settings
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
		if (strlen(_class->postproc) != 0) buildArgs(postproc, _class->postproc); else postproc[0] = '\0';
	rom_extension = strdup(_class->rom_extension);
	llist0_defaults = _class->llist0_defaults;
	llist0_defaults_len = _class->llist0_defaults_len;
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
