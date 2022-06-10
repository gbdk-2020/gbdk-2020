/*
 * lcc [ option ]... [ file | -llib ]...
 * front end for the ANSI C compiler
 */
static char rcsid[] = "$Id: lcc.c,v 2.0 " BUILDDATE " " BUILDTIME " gbdk-2020 Exp $";

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <signal.h>

#ifdef _WIN32
# include <io.h>
# include <process.h>
#else
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
#endif

#include "gb.h"
#include "list.h"
#include "targets.h"

#ifndef TEMPDIR
#define TEMPDIR "/tmp"
#endif

void *alloc(int);
extern char *basepath(char *);
extern char *path_stripext(char *);
extern char *path_newext(char *, char *);
static int callsys(char *[]);
extern char *concat(const char *, const char *);
static void compose(char *[], List, List, List);
static void error(char *, char *);
static char *exists(char *);
static char *first(char *);
static int filename(char *, char *);
static void help(void);
static void initinputs(void);
static void interrupt(int);
static void opt(char *);
extern int main(int, char *[]);
extern char *replace(const char *, int, int);
static void rm(List);
extern char *strsave(const char *);
extern char *stringf(const char *, ...);
extern int suffix(char *, char *[], int);
extern char *tempname(char *);

static bool arg_has_searchkey(char *, char *);

// Adds linker default required vars if not present (defined by user)
static void Fixllist();

static void handle_autobanking(void);


// These get populated from _class using finalise() in gb.c
extern char *cpp[], *include[], *com[], *as[], *bankpack[], *ld[], *ihxcheck[], *mkbin[], *postproc[], inputs[], *suffixes[], *rom_extension;
extern arg_entry *llist0_defaults;
extern int llist0_defaults_len;

extern int option(char *);
extern void set_gbdk_dir(char*);

void finalise(void);

static int errcnt;		/* number of errors */
static int Eflag;		/* -E specified */
static int Sflag;		/* -S specified */
static int cflag;		/* -c specified */
static int Kflag;		/* -K specified */
static int autobankflag;	/* -K specified */
int verbose;		/* incremented for each -v */
static List bankpack_flags;	/* bankpack flags */
static List ihxchecklist;	/* ihxcheck flags */
static List mkbinlist;		/* loader files, flags */

// Index entries for llist[]
#define L_ARGS 0
#define L_FILES 1
#define L_LKFILES 2
static List llist[3];       /* [2] = .lkfiles, [1] = linker object file list, [0] = linker flags */

static List alist;		/* assembler flags */
List clist;		/* compiler flags */
static List plist;		/* preprocessor flags */
static List ilist;		/* list of additional includes from LCCINPUTS */
static List rmlist;		/* list of files to remove */
static char *outfile;		/* ld output file or -[cS] object file */
static int ac;			/* argument count */
static char **av;		/* argument vector */
char *tempdir = TEMPDIR;	/* directory for temporary files */
char *progname;
static List lccinputs;		/* list of input directories */
char bankpack_newext[1024] = {'\0'};
static int ihx_inputs = 0;  // Number of ihx files present in input list
static char ihxFile[256] = {'\0'};
static char binFile[256] = {'\0'};


int main(int argc, char *argv[]) {
	int i, j, nf;

	progname = argv[0];
	ac = argc + 50;
	av = alloc(ac * sizeof(char *));
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, interrupt);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, interrupt);
#ifdef SIGHUP
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, interrupt);
#endif
	if (getenv("TMP"))
		tempdir = getenv("TMP");
	else if (getenv("TEMP"))
		tempdir = getenv("TEMP");
	else if (getenv("TMPDIR"))
		tempdir = getenv("TMPDIR");
	assert(tempdir);

	// Remove trailing slashes
	i = strlen(tempdir);
	for (; (i > 0) && ((tempdir[i - 1] == '/') || (tempdir[i - 1] == '\\')); i--)
		tempdir[i - 1] = '\0';
	if (argc <= 1) {
		help();
		exit(0);
	}
//	clist = append("-D__LCC__", 0);
	initinputs();
	if (getenv("GBDKDIR"))
		option(stringf("--prefix=%s", getenv("GBDKDIR")));
	else
		set_gbdk_dir(argv[0]);
	for (nf = 0, i = j = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0) {
			if (++i < argc) {
				// Don't allow output file to have ".c" or ".i" extension (first two in suffixes[])
				if (suffix(argv[i], suffixes, 2) != SUFX_NOMATCH) {
					error("-o would overwrite %s", argv[i]);
					exit(8);
				}
				// Valid output file found
				outfile = argv[i];
				continue;
			}
			else {
				error("unrecognized option `%s'", argv[i - 1]);
				exit(8);
			}
		}
		else if (strcmp(argv[i], "-target") == 0) {
			if (argv[i + 1] && *argv[i + 1] != '-')
				i++;
			continue;
		}
		else if (*argv[i] == '-' && argv[i][1] != 'l') {
			opt(argv[i]);
			continue;
		}
		else if (*argv[i] != '-') {
			// Count number of (.ihx) files
			if (suffix(argv[i], (char * []){EXT_IHX}, 1) != SUFX_NOMATCH)
				ihx_inputs++;
			// Count number of (.c, .i, .asm, .s) files
			else if (suffix(argv[i], suffixes, 3) != SUFX_NOMATCH)
				nf++;
		}
		argv[j++] = argv[i];
	}

	// Ignore -o output request if:
	// * compile only (-c) *OR* compile to ASM (-S) is specified
	// * and there are 2 or more source files (.c, .i, .asm, .s) in the input list (what "nf" seems to count)
	// Instead, it will generate output matching each input filename
	if ((cflag || Sflag) && outfile && nf != 1) {
		fprintf(stderr, "%s: -o %s ignored\n", progname, outfile);
		outfile = 0;
	}

	// When .ihx is an input only ihxcheck and makebin will be called.
	// Warn that all source files won't be processed
	if ((ihx_inputs > 0) && (nf > 0)) {
		fprintf(stderr, "%s: Warning: .ihx file present as input, all other input files ignored\n", progname);
	}

	// Add includes
	argv[j] = 0;

	// This copies settings from port:platform "class" structure
	// into command strings used for compose()
	finalise();

	for (i = 0; include[i]; i++)
		clist = append(include[i], clist);
	if (ilist) {
		List b = ilist;
		do {
			b = b->link;
			clist = append(b->str, clist);
		} while (b != ilist);
	}
	ilist = 0;
	for (i = 1; argv[i]; i++)
		// Process arguments
		if (*argv[i] == '-')
			opt(argv[i]);
		else {
			// Process filenames
			char *name = exists(argv[i]);
			if (name) {
				if (strcmp(name, argv[i]) != 0
					|| ((nf > 1) && (suffix(name, suffixes, 3) != SUFX_NOMATCH)) ) // Does it match: .c, .i, .asm, .s

					fprintf(stderr, "%s:\n", name);
				// Send input filename argument to "filename processor"
				// which will add them to llist[n] in some form most of the time
				filename(name, 0);
			}
			else
				error("can't find `%s'", argv[i]);
		}


	// Perform Link / ihxcheck / makebin stages (unless some conditions prevent it)
	if (errcnt == 0 && !Eflag && !cflag && !Sflag &&
		(llist[L_FILES] || llist[L_LKFILES] || ((ihxFile[0] != '\0') && ihx_inputs))) {

		int target_is_ihx = 0;

		// if outfile is not specified, set it to default rom extension for active port:platform
		if(!outfile)
			outfile = concat("a", rom_extension);

		// If an .ihx file is present as input skip link related stages
		if (ihx_inputs > 0) {

			// Only one .ihx can be used for input, warn that others will be ignored
			if (ihx_inputs > 1)
				fprintf(stderr, "%s: Warning: Multiple (%d) .ihx files present as input, only one (%s) will be used\n", progname, ihx_inputs, ihxFile);
		}
		else {

			//file.gb to file.ihx (don't use tmpfile because maps and other stuffs are created there)
			// Check to see if output target is a .ihx file
			target_is_ihx = (suffix(outfile, (char *[]){EXT_IHX}, 1) != SUFX_NOMATCH);

			// Build ihx file name from output name
			sprintf(ihxFile, "%s%s", path_stripext(outfile), EXT_IHX);

			// Only remove .ihx from the delete-list if it's not the final target
			if (!target_is_ihx)
				append(ihxFile, rmlist);

			// If auto bank assignment is enabled, modify obj files before linking
			// Will alter: llist[L_FILES] and llist[L_LKFILES]
			if (autobankflag)
				handle_autobanking();

			// Copy any pending linkerfiles into the linker list (with "-f" as preceding arg)
			llist[L_FILES] = list_add_to_another(llist[L_FILES], llist[L_LKFILES], NULL, "-f");
			// Call linker (add output ihxfile in compose $3)
			Fixllist();   // Fixlist adds required default linker vars if not added by user
			compose(ld, llist[L_ARGS], llist[L_FILES], append(ihxFile, 0));

			if (callsys(av))
				errcnt++;
		} // end: non-ihx input file handling

		// ihxcheck (test for multiple writes to the same ROM address)
		if (!Kflag) {
			compose(ihxcheck, ihxchecklist, append(ihxFile, 0), 0);
			if (callsys(av))
				errcnt++;
		}

		// No need to makebin (.ihx -> .gb [or other rom_extension]) if .ihx is final target
		if (!target_is_ihx)
		{
			if(errcnt == 0)
			{
                // makebin - use output filename unless there is a post-process step
                if (strlen(postproc) == 0)
                    sprintf(binFile, "%s", outfile);
                else
                    sprintf(binFile, "%s", path_newext(outfile, EXT_ROM));

                compose(mkbin, mkbinlist, append(ihxFile, 0), append(binFile, 0));
                if (callsys(av))
                    errcnt++;

                // post-process step (such as makecom), if applicable
                if ((strlen(postproc) != 0) && (errcnt == 0)) {
                    compose(postproc, append(binFile, 0), append(outfile, 0), 0);
                    if (callsys(av))
                        errcnt++;
                }
			}
		}
	}
	rm(rmlist);
    if (verbose > 0)
        fprintf(stderr, "\n");
	return errcnt ? EXIT_FAILURE : EXIT_SUCCESS;
}



// Check whether string "arg" has "searchkey" at the first possible
// occurrence of searchkey's starting character in arg (typically "." or "_")
static bool arg_has_searchkey(char * arg, char * searchkey) {
	char * str_start = strchr(arg, searchkey[0]);

	if (str_start)
		return (strncmp(str_start, searchkey, strlen(searchkey)) == 0);
	else
		return false;
}


// Adds linker default required vars if not present (defined by user)
// Uses data from targets.c for per port/platform defaults
static void Fixllist()
{
	int c;

	// Iterate through linker list entries
	if(llist[L_ARGS]) {
		List b = llist[L_ARGS];
		do {
			b = b->link;
			// Only -g and -b settings are supported at this time
			if(b->str[1] == 'g' || b->str[1] == 'b')
			{
				// '-g' and '-b' now have their values separated by a space.
				// That splits them into two consecutive llist items,
				// so try advancing to the next item to access it's value.
				// Example:  b = "-g", next = ".STACK=0xE000"
				if (b != llist[L_ARGS])
					b = b->link;
				else
					break; // end of list

				// Check current linker arg to see if any default settings are present
				// If they do, flag them as present so they don't need to be added later
				for (c = 0; c < llist0_defaults_len; c++)
					if (arg_has_searchkey(b->str, llist0_defaults[c].searchkey))
						llist0_defaults[c].found = true;
			}
		} while (b != llist[L_ARGS]);
	}

	// Add required default settings to the linker list if they weren't found
	for (c = 0; c < llist0_defaults_len; c++)
		if (llist0_defaults[c].found == false) {
			// Add the entry to the linker llist[L_ARGS], flag first then value
			llist[L_ARGS] = append(llist0_defaults[c].addflag,  llist[L_ARGS]);
			llist[L_ARGS] = append(llist0_defaults[c].addvalue, llist[L_ARGS]);
		}
}


/* alloc - allocate n bytes or die */
void *alloc(int n) {
	static char *avail, *limit;

	n = (n + sizeof(char *) - 1)&~(sizeof(char *) - 1);
	if (n >= limit - avail) {
		avail = malloc(n + 4 * 1024);
		assert(avail);
		limit = avail + n + 4 * 1024;
	}
	avail += n;
	return avail - n;
}


/* basepath - return base name for name, e.g. /usr/drh/foo.c => foo */
char *basepath(char *name) {
	char *s, *b, *t = 0;

	for (b = s = name; *s; s++)
		if (*s == '/' || *s == '\\') {
			b = s + 1;
			t = 0;
		}
		else if (*s == '.')
			t = s;
	s = strsave(b);
	if (t)
		s[t - b] = 0;
	return s;
}

// path_stripext - return a new string of path [name] with extension removed
//              e.g. /usr/drh/foo.c => /usr/drh/foo
char *path_stripext(char *name) {
	char * copy_str = strsave(name);
	char * end_str = copy_str + strlen(copy_str);

	// Work from end of string backward,
	// truncate string at first "." char
	while (end_str > copy_str) {
		if (*end_str == '.') {
			*end_str = '\0';
			break;
		}
		end_str--;
	}
	return copy_str;
}


// path_newext - return a new string of path [name] with extension replaced
//              e.g. /usr/drh/foo.c => /usr/drh/foo
char *path_newext(char *name, char *new_ext) {
     return stringf("%s%s", path_stripext(name), new_ext);
}


// Check if an extension matches the end of a filename
int matches_ext(const char * filename, const char * ext)
{
	// Only test match if filename is larger than extension
	// Then check the end of the filename for [ext] length of chars
	if (strlen(filename) >= strlen(ext))
		return strncmp(filename + strlen(filename) - strlen(ext), ext, strlen(ext)) == 0;
	else return 0;
}


#ifndef WIN32
#define _P_WAIT 0

static int _spawnvp(int mode, const char *cmdname, char *argv[]) {
	int status;
	pid_t pid, n;

	switch (pid = fork()) {
	case -1:
		fprintf(stderr, "%s: no more processes\n", progname);
		return 100;
	case 0:
		execv(cmdname, argv);
		fprintf(stderr, "%s: ", progname);
		perror(cmdname);
		fflush(stdout);
		exit(100);
	}
	while ((n = wait(&status)) != pid && n != -1)
		;
	if (n == -1)
		status = -1;
	if (status & 0377) {
		fprintf(stderr, "%s: fatal error in %s\n", progname, cmdname);
		status |= 0400;
	}
	return (status >> 8) & 0377;
}
#endif

/* removes quotes from src and stores it on dst */
void removeQuotes(char* src, char* dst)
{
	while(*src != '\0')
	{
		if(*src != '\"')
    {
      if(*dst != *src)
				*(dst) = *src;
      dst ++;
    }
		src ++;
	}
  if(*dst != '\0')
	  *dst = '\0';
}

/* turns "C:\Users\Zalo\Desktop\gb\gbdk 2020\build\gbdk\"bin/sdcpp
   into  "C:\Users\Zalo\Desktop\gb\gbdk 2020\build\gbdk\bin/sdcpp" */
void fixQuotes(char* str)
{
	while(*str != '\"' && *str != '\0')
		str ++;

	char* src = str;
	if(*src == '\"')
	{
		*(str ++) = '\"';
		while(*src != '\0')
		{
			if(*src != '\"')
				*(str ++) = *src;
			src ++;
		}
		*(str ++) = '\"';
		*(str ++) = '\0';
	}
}

/* callsys - execute the command described by av[0...], return status */
static int callsys(char **av) {
	int i, status = 0;
	static char **argv;
	static int argc;

	for (i = 0; av[i] != NULL; i++)
		;
	if (i + 1 > argc) {
		argc = i + 1;
		if (argv == NULL)
			argv = malloc(argc * sizeof *argv);
		else {
			char **argv0 = argv;
			argv = realloc(argv0, argc * sizeof *argv);
			if (argv == NULL)
				free(argv0);
		}
		assert(argv);
	}
	for (i = 0; status == 0 && av[i] != NULL; ) {
		int j = 0;
		char *s;
		for (; av[i] != NULL && (s = strchr(av[i], '\n')) == NULL; i++)
			argv[j++] = av[i];
		if (s != NULL) {
			if (s > av[i])
				argv[j++] = stringf("%.*s", s - av[i], av[i]);
			if (s[1] != '\0')
				av[i] = s + 1;
			else
				i++;
		}
		argv[j] = NULL;
		if (verbose > 0) {
			int k;
			fprintf(stderr, "%s", argv[0]);
			for (k = 1; argv[k] != NULL; k++)
				fprintf(stderr, " %s", argv[k]);
			fprintf(stderr, "\n");
		}
		if (verbose < 2)
		{
			char argv_0_no_quotes[256];
			removeQuotes(argv[0], argv_0_no_quotes);
			for(char** it = argv; *it != 0; it ++)
			{
#ifdef __WIN32__
				fixQuotes(*it); //On windows quotes must be kept, and fixed
#else
        removeQuotes(*it, *it); //On macos, quotes must be fully removed from args
#endif
			}
			//For future reference:
			//_spawnvp requires _FileName to not have quotes
			//_Arguments must have quotes on windows, but not in macos
			//Quoted strings must begin and end with quotes, no quotes in the middle
			status = _spawnvp(_P_WAIT, argv_0_no_quotes, argv);
		}
		if (status == -1) {
			fprintf(stderr, "%s: ", progname);
			perror(argv[0]);
		}
	}
	return status;
}

/* concat - return concatenation of strings s1 and s2 */
char *concat(const char *s1, const char *s2) {
	int n = strlen(s1);
	char *s = alloc(n + strlen(s2) + 1);

	strcpy(s, s1);
	strcpy(s + n, s2);
	return s;
}

/* compose - compose cmd into av substituting a, b, c for $1, $2, $3, resp. */
static void compose(char *cmd[], List a, List b, List c) {
	int i, j;
	List lists[3];

	lists[0] = a;
	lists[1] = b;
	lists[2] = c;
	for (i = j = 0; cmd[i]; i++) {
		char *s = strchr(cmd[i], '$');
		if (s && isdigit(s[1])) {
			int k = s[1] - '0';
			assert(k >= 1 && k <= 3);
			b = lists[k - 1];
			if (b) {
				b = b->link;
				av[j] = alloc(strlen(cmd[i]) + strlen(b->str) - 1);
				strncpy(av[j], cmd[i], s - cmd[i]);
				av[j][s - cmd[i]] = '\0';
				strcat(av[j], b->str);
				strcat(av[j++], s + 2);
				while (b != lists[k - 1]) {
					b = b->link;
					assert(j < ac);
					av[j++] = b->str;
				};
			}
		}
		else if (*cmd[i]) {
			assert(j < ac);
			av[j++] = cmd[i];
		}
	}
	av[j] = NULL;
}

/* error - issue error msg according to fmt, bump error count */
static void error(char *fmt, char *msg) {
	fprintf(stderr, "%s: ", progname);
	fprintf(stderr, fmt, msg);
	fprintf(stderr, "\n");
	errcnt++;
}

/* exists - if `name' readable return its path name or return null */
static char *exists(char *name) {
	List b;

	if ((name[0] == '/' || name[0] == '\\' || name[2] == ':')
		&& access(name, 4) == 0)
		return name;
	if (!(name[0] == '/' || name[0] == '\\' || name[2] == ':')
		&& (b = lccinputs))
		do {
			b = b->link;
			if (b->str[0]) {
				char buf[1024];
				sprintf(buf, "%s/%s", b->str, name);
				if (access(buf, 4) == 0)
					return strsave(buf);
			}
			else if (access(name, 4) == 0)
				return name;
		} while (b != lccinputs);
		if (verbose > 1)
			return name;
		return 0;
}

/* first - return first component in semicolon separated list */
static char *first(char *list) {
	char *s = strchr(list, ';');

	if (s) {
		char buf[1024];
		size_t len = s - list;
		if(len >= sizeof(buf)) len = sizeof(buf)-1;
		strncpy(buf, list, len);
		buf[len] = '\0';
		return strsave(buf);
	}
	else
		return list;
}

/* filename - process file name argument `name', return status */
static int filename(char *name, char *base) {
	int status = 0;
	static char *stemp, *itemp;

	if (base == 0)
		base = basepath(name);

	// Handle all available suffixes except .gb (last in list)
	switch (suffix(name, suffixes, 5)) {
	case 0:	/* C source files */
		{
			char *ofile;
			if ((cflag || Sflag) && outfile)
				ofile = outfile;
			else if (cflag)
				ofile = concat(base, EXT_O);
			else if (Sflag) {
				// When compiling to asm only, set outfile as .asm
				ofile = concat(base, EXT_ASM);
			}
			else
			{
				ofile = tempname(EXT_O);

				char* ofileBase = basepath(ofile);
				rmlist = append(stringf("%s/%s%s", tempdir, ofileBase, EXT_ASM), rmlist);
				rmlist = append(stringf("%s/%s%s", tempdir, ofileBase, ".lst"), rmlist);
				rmlist = append(stringf("%s/%s%s", tempdir, ofileBase, ".sym"), rmlist);
				rmlist = append(stringf("%s/%s%s", tempdir, ofileBase, ".adb"), rmlist);
			}

			compose(com, clist, append(name, 0), append(ofile, 0));
			status = callsys(av);
			if (!find(ofile, llist[L_FILES]))
				llist[L_FILES] = append(ofile, llist[L_FILES]);
		}
		break;
	case 2:	/* assembly language files */
		if (Eflag)
			break;
		if (!Sflag) {
			char *ofile;
			if (cflag && outfile)
				ofile = outfile;
			else if (cflag)
				ofile = concat(base, EXT_O);
			else
				ofile = tempname(EXT_O);
			compose(as, alist, append(name, 0), append(ofile, 0));
			status = callsys(av);
			if (!find(ofile, llist[L_FILES]))
				llist[L_FILES] = append(ofile, llist[L_FILES]);
		}
		break;
	case 3:	/* object files */
		if (!find(name, llist[L_FILES]))
			llist[L_FILES] = append(name, llist[L_FILES]);
		break;
	case 4: // .ihx files
		// Apply "name" as .ihx file (there can be only one as input)
		strncpy(ihxFile, name, sizeof(ihxFile) - 1);
		break;
	default:
		if (Eflag) {
			compose(cpp, plist, append(name, 0), 0);
			status = callsys(av);
		}
		llist[L_FILES] = append(name, llist[L_FILES]);
		break;
	}
	if (status)
		errcnt++;
	return status;
}

/* help - print help message */
static void help(void) {
	static char *msgs[] = {
"", " [ option | file ]...\n",
"	except for -l, options are processed left-to-right before files\n",
"	unrecognized options are taken to be linker options\n",
"-A	warn about nonANSI usage; 2nd -A warns more\n",
"-b	emit expression-level profiling code; see bprint(1)\n",
#ifdef sparc
"-Bstatic -Bdynamic	specify static or dynamic libraries\n",
#endif
"-Bdir/	use the compiler named `dir/rcc'\n",
"-c	compile only\n",
"-dn	set switch statement density to `n'\n",
"-debug	Turns on --debug for compiler, -y (.cdb) and -j (.noi) for linker\n",
"-Dname -Dname=def	define the preprocessor symbol `name'\n",
"-E	run only the preprocessor on the named C programs and unsuffixed files\n",
"-g	produce symbol table information for debuggers\n",
"-help or -?	print this message\n",
"-Idir	add `dir' to the beginning of the list of #include directories\n",
"-K don't run ihxcheck test on linker ihx output\n",
"-lx	search library `x'\n",
"-m	select port and platform: \"-m[port]:[plat]\" ports:sm83,z80,mos6502 plats:ap,duck,gb,sms,gg,nes\n",
"-N	do not search the standard directories for #include files\n",
"-n	emit code to check for dereferencing zero pointers\n",
"-no-crt do not auto-include the gbdk crt0.o runtime in linker list\n",
"-no-libs do not auto-include the gbdk libs in linker list\n",
"-O	is ignored\n",
"-o file	leave the output in `file'\n",
"-P	print ANSI-style declarations for globals\n",
"-p -pg	emit profiling code; see prof(1) and gprof(1)\n",
"-S	compile to assembly language\n",
"-autobank auto-assign banks set to 255 (bankpack)\n"
#ifdef linux
"-static	specify static libraries (default is dynamic)\n",
#endif
"-t -tname	emit function tracing calls to printf or to `name'\n",
"-target name	is ignored\n",
"-tempdir=dir	place temporary files in `dir/'", "\n"
"-Uname	undefine the preprocessor symbol `name'\n",
"-v	show commands as they are executed; 2nd -v suppresses execution\n",
"-w	suppress warnings\n",
"-Woarg	specify system-specific `arg'\n",
"-W[pfablim]arg	pass `arg' to the preprocessor, compiler, assembler, bankpack, linker, ihxcheck, or makebin\n",
	0 };
	int i;
	char *s;

	msgs[0] = progname;
	for (i = 0; msgs[i]; i++) {
		fprintf(stderr, "%s", msgs[i]);
		if (strncmp("-tempdir", msgs[i], 8) == 0 && tempdir)
			fprintf(stderr, "; default=%s", tempdir);
	}
#define xx(v) if ((s = getenv(#v))) fprintf(stderr, #v "=%s\n", s)
	xx(LCCINPUTS);
	xx(LCCDIR);
#ifdef WIN32
	xx(include);
	xx(lib);
#endif
#undef xx
}

/* initinputs - if LCCINPUTS or include is defined, use them to initialize various lists */
static void initinputs(void) {
	char *s = getenv("LCCINPUTS");
	List list, b;

	if (s == 0 || (s = inputs)[0] == 0)
		s = ".";
	if (s) {
		lccinputs = path2list(s);
		b = lccinputs;
		if (b)
			do {
				b = b->link;
				if (strcmp(b->str, ".") != 0) {
					ilist = append(concat("-I", b->str), ilist);
					if (strstr(com[1], "win32") == NULL)
						llist[L_ARGS] = append(concat("-L", b->str), llist[L_ARGS]);
				}
				else
					b->str = "";
			} while (b != lccinputs);
	}
#ifdef WIN32
	if (list = b = path2list(getenv("include")))
		do {
			b = b->link;
			ilist = append(stringf("-I\"%s\"", b->str), ilist);
		} while (b != list);
#endif
}

/* interrupt - catch interrupt signals */
static void interrupt(int n) {
	rm(rmlist);
	exit(n = 100);
}

/* opt - process option in arg */
static void opt(char *arg) {
	switch (arg[1]) {	/* multi-character options */
	case 'W':	/* -Wxarg */
		if (arg[2] && arg[3])
			switch (arg[2]) {
			case 'o':
				if (option(&arg[3]))
					return;
				break;
			case 'p': /* Preprocessor */
				clist = append(&arg[3], clist);
				return;
			case 'f': /* Compiler */
				if (strcmp(&arg[3], "-C") || option("-b")) {
					clist = append(&arg[3], clist);
					return;
				}
				break; /* and fall thru */
			case 'a': /* Assembler */
				alist = append(&arg[3], alist);
				return;
			case 'i': /* ihxcheck arg list */
				ihxchecklist = append(&arg[3], ihxchecklist);
				return;
			case 'b': /* auto bankpack_flags arg list */
				bankpack_flags = append(&arg[3], bankpack_flags);
				// -Wb-ext=[.some-extension]
				// If bankpack is going to rewrite input object files to a new extension
				// then save that extension for rewriting the linker list (llist[L_FILES])
				if (strstr(&arg[3], "-ext=") != NULL) {
					if (arg[8]) {
						sprintf(bankpack_newext, "%s", &arg[8]);
					}
				}
				return;
			case 'l': /* Linker */
				if(arg[4] == 'y' && (arg[5] == 't' || arg[5] == 'o' || arg[5] == 'a' || arg[5] == 'p') && (arg[6] != '\0' && arg[6] != ' '))
					goto makebinoption; //automatically pass -yo -ya -yt -yp options to makebin (backwards compatibility)
				{
					// If using linker file for sdldgb (-f file[.lk]).
					// Starting at arg[5] should be name of the linkerfile
					if ((arg[4] == 'f') && (arg[5])) {
						// Items in llist[L_LKFILES] get added to llist[L_FILES] right before the linker is called.
						// That avoids sending to bankpack with the "-f" flag mixed in with the names of the .o object files.
						llist[L_LKFILES] = append(stringf(&arg[5]), llist[L_LKFILES]);
					} else {
						//sdldgb requires spaces between -k and the path
						llist[L_ARGS] = append(stringf("%c%c", arg[3], arg[4]), llist[L_ARGS]);     //splitting the args into 2 works on Win and Linux
						if (arg[5]) {
							llist[L_ARGS] = append(&arg[5], llist[L_ARGS]);  // Add filename separately if present
						}
					}
				}
				return;
			case 'm': /* Makebin */
			makebinoption:{
				char *tmp = malloc(256);
				char *tmp2 = malloc(256);
				tmp2[0] = '\0'; // Zero out second arg by default
				if (arg[4] == 'y') {
					sprintf(tmp, "%c%c%c", arg[3], arg[4], arg[5]); //-yo -ya -yt -yl -yk -yn -yp
					if (!(arg[5] == 'c' || arg[5] == 'C' || arg[5] == 's'  || arg[5] == 'S' || arg[5] == 'j' || arg[5] == 'p')) // Don't add second arg for -yc -yC -ys -yS -yj
						sprintf(tmp2, "%s", &arg[6]);
					// -yp of SDCC 4.1.0's makebin erroneously does not use a space between flag and it's value
					// So append trailing values to first arg that would otherwise go in the second arg
					if (arg[5] == 'p')
						sprintf(tmp, "%s", &arg[3]);

					// If MBC option is present for makebin (-Wl-yt <n> or -Wm-yt <n>) then make a copy for bankpack to use
					if (arg[5] == 't')
						bankpack_flags = append(&arg[3], bankpack_flags);
				} else if ((arg[4] == 'x') && arg[5] && arg[6]) {
					// SMS options
					// Print "-" plus first two option chars into first arg
					// and any trailing option chars into a separate arg
					sprintf(tmp, "%c%c%c", arg[3], arg[4], arg[5]);  //-xo -xj -xv
					if(arg[6])
						sprintf(tmp2, "%s", &arg[6]);
				} else {
					sprintf(tmp, "%c%c", arg[3], arg[4]); //-s
					if(arg[5])
						sprintf(tmp2, "%s", &arg[5]);
				}
				mkbinlist = append(tmp, mkbinlist);
				if (tmp2[0] != '\0')  // Only append second argument if it's populated
					mkbinlist = append(tmp2, mkbinlist);
				}return;
			}
		fprintf(stderr, "%s: %s ignored\n", progname, arg);
		return;
	case 'd':	/* -dn */
		if (strcmp(arg, "-debug") == 0) {
			// Load default debug options
			clist    = append("--debug", clist);  // Debug for sdcc compiler
			llist[L_ARGS] = append("-y", llist[L_ARGS]);    // Enable .cdb output for sdldgb linker
			llist[L_ARGS] = append("-j", llist[L_ARGS]);    // Enable .noi output
			return;
		}

		arg[1] = 's';
		clist = append(arg, clist);
		return;
	case 't':	/* -t -tname -tempdir=dir */
		if (strncmp(arg, "-tempdir=", 9) == 0)
			tempdir = arg + 9;
		else
			clist = append(arg, clist);
		return;
	case 'p':	/* -p -pg */
		if (option(arg))
			clist = append(arg, clist);
		else
			fprintf(stderr, "%s: %s ignored\n", progname, arg);
		return;
	case 'D':	/* -Dname -Dname=def */
	case 'U':	/* -Uname */
	case 'I':	/* -Idir */
		clist = append(arg, clist);
		return;
	case 'K':
		Kflag++;
		return;
	case 'a':
		if (strcmp(arg, "-autobank") == 0) {
			autobankflag++;
			return;
		}
	case 'n':
		if (strcmp(arg, "-no-crt") == 0) {
			option(arg);  // Clear crt0 entry in linker compose string
			return;
		}
		else if (strcmp(arg, "-no-libs") == 0) {
			option(arg);  // Clear libs entry in linker compose string
			return;
		}
	case 'B':	/* -Bdir -Bstatic -Bdynamic */
#ifdef sparc
		if (strcmp(arg, "-Bstatic") == 0 || strcmp(arg, "-Bdynamic") == 0)
			llist[L_FILES] = append(arg, llist[L_FILES]);
		else
#endif
		{
			static char *path;
			if (path)
				error("-B overwrites earlier option", 0);
			path = arg + 2;
			if (strstr(com[1], "win32") != NULL)
				com[0] = concat(replace(path, '/', '\\'), concat("rcc", EXT_IHX));
			else
				com[0] = concat(path, "rcc");
			if (path[0] == 0)
				error("missing directory in -B option", 0);
		}
		return;
	case 'h':
		if (strcmp(arg, "-help") == 0) {
			static int printed = 0;
	case '?':
		if (!printed)
			help();
		printed = 1;
		return;
		}
#ifdef linux
	case 's':
		if (strcmp(arg, "-static") == 0) {
			if (!option(arg))
				fprintf(stderr, "%s: %s ignored\n", progname, arg);
			return;
		}
#endif
	}
	if (arg[2] == 0)
		switch (arg[1]) {	/* single-character options */
		case 'S':        // Requested compile to assembly only
			Sflag++;
			option(arg); // Update composing the compile stage, use of -S instead of -c
			return;
		case 'O':
			fprintf(stderr, "%s: %s ignored\n", progname, arg);
			return;
		case 'A': case 'n': case 'w': case 'P':
			clist = append(arg, clist);
			return;
		case 'g': case 'b':
			if (option(arg))
				clist = append(arg[1] == 'g' ? "-g2" : arg, clist);
			else
				fprintf(stderr, "%s: %s ignored\n", progname, arg);
			return;
		case 'G':
			if (option(arg)) {
				clist = append("-g3", clist);
				llist[L_ARGS] = append("-N", llist[L_ARGS]);
			}
			else
				fprintf(stderr, "%s: %s ignored\n", progname, arg);
			return;
		case 'E':
			Eflag++;
			return;
		case 'c':
			cflag++;
			return;
		case 'N':
			if (strcmp(basepath(cpp[0]), "gcc-cpp") == 0)
				clist = append("-nostdinc", clist);
			include[0] = 0;
			ilist = 0;
			return;
		case 'v':
			if (verbose++ == 0) {
#if 0
				/* GBDK removed */
				if (strcmp(basepath(cpp[0]), "gcc-cpp") == 0)
					clist = append(arg, clist);
				clist = append(arg, clist);
#endif
				fprintf(stderr, "%s %s\n", progname, rcsid);
			}
			return;
		}
	if (option(arg))
		return;
	if (cflag || Sflag || Eflag)
		fprintf(stderr, "%s: %s ignored\n", progname, arg);
	else
		llist[L_FILES] = append(arg, llist[L_FILES]);
}


/* replace - copy str, then replace occurrences of from with to, return the copy */
char *replace(const char *str, int from, int to) {
	char *s = strsave(str), *p = s;

	for (; (p = strchr(p, from)) != NULL; p++)
		*p = to;
	return s;
}

/* rm - remove files in list */
static void rm(List list) {
	if (list) {
		List b = list;
		if (verbose)
			fprintf(stderr, "rm");
		do {
			if (verbose)
				fprintf(stderr, " %s", b->str);
			if (verbose < 2)
				remove(b->str);
		} while ((b = b->link) != list);
		if (verbose)
			fprintf(stderr, "\n");
	}
}

/* strsave - return a saved copy of string str */
char *strsave(const char *str) {
	return strcpy(alloc(strlen(str) + 1), str);
}

/* stringf - format and return a string */
char *stringf(const char *fmt, ...) {
	char buf[1024];
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	return strsave(buf);
}

/* suffix - if one of tails[0..n-1] holds a proper suffix of name, return its index */
int suffix(char *name, char *tails[], int n) {
	int i, len = strlen(name);

	for (i = 0; i < n; i++) {
		char *s = tails[i], *t;
		for (; (t = strchr(s, ';')); s = t + 1) {
			int m = t - s;
			if (len > m && strncmp(&name[len - m], s, m) == 0)
				return i;
		}
		if (*s) {
			int m = strlen(s);
			if (len > m && strncmp(&name[len - m], s, m) == 0)
				return i;
		}
	}
	return SUFX_NOMATCH;
}

/* tempname - generate a temporary file name in tempdir with given suffix */
char *tempname(char *suffix) {
	static int n;
	char *name = stringf("%s/lcc%d%d%s", tempdir, getpid(), n++, suffix);

	if (strstr(com[1], "win32") != NULL)
		name = replace(name, '/', '\\');
	rmlist = append(name, rmlist);
	return name;
}


// Performs the autobanking stage
//
// Should be called prior to doing compose() for the linker
//
static void handle_autobanking(void) {

    // bankpack will be populated if supported by active port:platform
    if (bankpack[0][0] != '\0') {

        char * bankpack_linkerfile_name = tempname(EXT_LK);
        rmlist = append(bankpack_linkerfile_name, rmlist); // Delete the linkerfile when done
        // Always use a linkerfile when using bankpack through lcc
        // Writes all input object files out to [bankpack_linkerfile_name]
        bankpack_flags = append(stringf("%s%s","-lkout=", bankpack_linkerfile_name), bankpack_flags);

        // Add linkerfile entries (usually *.lk) to the bankpack arg list if any are present
        bankpack_flags = list_add_to_another(bankpack_flags, llist[L_LKFILES], "-lkin=", NULL);

        // Prepare the bankpack command line, then execute it
        compose(bankpack, bankpack_flags, llist[L_FILES], 0);
        if (callsys(av))
            errcnt++;

        // Clear out the objects file and linkerfiles from their lists
        // Then replace them with the filename passed to bankpack for "-lkout="
        llist[L_FILES]   = list_remove_all(llist[L_FILES]);
        llist[L_LKFILES] = list_remove_all(llist[L_LKFILES]);
        llist[L_LKFILES] = append(stringf("%s", bankpack_linkerfile_name), llist[L_LKFILES]);
    }
    else
        fprintf(stderr, "Warning: bankpack enabled but not supported by active port:platform\n");
}