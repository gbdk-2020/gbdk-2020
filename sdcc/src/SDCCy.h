typedef union {
    symbol     *sym ;      /* symbol table pointer       */
    structdef  *sdef;      /* structure definition       */
    char       yychar[SDCC_NAME_MAX+1];
    sym_link       *lnk ;      /* declarator  or specifier   */
    int        yyint;      /* integer value returned     */
    value      *val ;      /* for integer constant       */
    initList   *ilist;     /* initial list               */
    char       *yyinline; /* inlined assembler code */
    ast       *asts;     /* expression tree            */
} YYSTYPE;
#define	IDENTIFIER	257
#define	TYPE_NAME	258
#define	CONSTANT	259
#define	STRING_LITERAL	260
#define	SIZEOF	261
#define	PTR_OP	262
#define	INC_OP	263
#define	DEC_OP	264
#define	LEFT_OP	265
#define	RIGHT_OP	266
#define	LE_OP	267
#define	GE_OP	268
#define	EQ_OP	269
#define	NE_OP	270
#define	AND_OP	271
#define	OR_OP	272
#define	MUL_ASSIGN	273
#define	DIV_ASSIGN	274
#define	MOD_ASSIGN	275
#define	ADD_ASSIGN	276
#define	SUB_ASSIGN	277
#define	LEFT_ASSIGN	278
#define	RIGHT_ASSIGN	279
#define	AND_ASSIGN	280
#define	XOR_ASSIGN	281
#define	OR_ASSIGN	282
#define	TYPEDEF	283
#define	EXTERN	284
#define	STATIC	285
#define	AUTO	286
#define	REGISTER	287
#define	CODE	288
#define	EEPROM	289
#define	INTERRUPT	290
#define	SFR	291
#define	AT	292
#define	SBIT	293
#define	REENTRANT	294
#define	USING	295
#define	XDATA	296
#define	DATA	297
#define	IDATA	298
#define	PDATA	299
#define	VAR_ARGS	300
#define	CRITICAL	301
#define	NONBANKED	302
#define	BANKED	303
#define	CHAR	304
#define	SHORT	305
#define	INT	306
#define	LONG	307
#define	SIGNED	308
#define	UNSIGNED	309
#define	FLOAT	310
#define	DOUBLE	311
#define	CONST	312
#define	VOLATILE	313
#define	VOID	314
#define	BIT	315
#define	STRUCT	316
#define	UNION	317
#define	ENUM	318
#define	ELIPSIS	319
#define	RANGE	320
#define	FAR	321
#define	CASE	322
#define	DEFAULT	323
#define	IF	324
#define	ELSE	325
#define	SWITCH	326
#define	WHILE	327
#define	DO	328
#define	FOR	329
#define	GOTO	330
#define	CONTINUE	331
#define	BREAK	332
#define	RETURN	333
#define	NAKED	334
#define	INLINEASM	335
#define	IFX	336
#define	ADDRESS_OF	337
#define	GET_VALUE_AT_ADDRESS	338
#define	SPIL	339
#define	UNSPIL	340
#define	GETHBIT	341
#define	BITWISEAND	342
#define	UNARYMINUS	343
#define	IPUSH	344
#define	IPOP	345
#define	PCALL	346
#define	ENDFUNCTION	347
#define	JUMPTABLE	348
#define	RRC	349
#define	RLC	350
#define	CAST	351
#define	CALL	352
#define	PARAM	353
#define	NULLOP	354
#define	BLOCK	355
#define	LABEL	356
#define	RECEIVE	357
#define	SEND	358
#define	ARRAYINIT	359


extern YYSTYPE yylval;
