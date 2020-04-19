/*
 * cross-reference.h
 *
 *  Created on: 2020/01/06
 *      Author: rimikawasaki
 */

#ifndef CROSS_REFERENCE_H_
#define CROSS_REFERENCE_H_

#define TPINT 1
#define TPCHAR 2
#define TPBOOL 3
#define TPARRAYINT 4
#define TPARRAYCHAR 5
#define TPARRAYBOOL 6
#define TPPROC 7

typedef struct VARNAME{
	char *varname;
	struct VARNAME *nextvarp;
} VARNAME;

typedef struct PARA{
	VARNAME *paraname;
	int paratp;
	struct PARA *nextparap;
}PARA;

typedef struct TYPE {
	int ttype; /* TPINT TPCHAR TPBOOL TPARRAYINT TPARRAYCHAR TPARRAYBOOL TPPROC */
	int arraysize; /* size of array, if type is array */
	PARA *paratp; /* pointer to parameter's type list if ttype is TPPROC */
}TYPE;

typedef struct LINE {
	int reflinenum;
	struct LINE *nextlinep;
}LINE;
typedef struct ID {
	char *name;
	char *procname; /* procedure name within which this name is defined */ /* NULL if global name */
	TYPE *itp;
	int ispara; /* 1:formal parameter, 0:else(variable) */
	int deflinenum;
	LINE *irefp;
	struct ID *nextp;
} ID; /* Pointers to root of global & local symbol tables */

extern int init_idtab(void);
extern ID *search_globalidtab(char *np);
extern int globalvar_register(VARNAME *vp,TYPE *tp);
extern int globalproc_register(char *np,PARA *pp);
extern int globalid_countup(char *np);
extern ID *search_localidtab(char *np,char *procnp);
extern int localpara_register(char *np,PARA *pp);
extern int localvar_register(char *np,VARNAME *vp,TYPE *tp);
extern int localid_countup(char *np,char *procnp);
extern VARNAME *addnamelist(VARNAME *list,char *name);
extern PARA *search_procpara(char *np);
extern TYPE *search_globalvartype(char *np);
extern TYPE *search_localvartype(char *np,char *procnp);
extern void print_idtab(void);
extern void release_idtab(void);

#endif /* CROSS_REFERENCE_H_ */
