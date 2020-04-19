/*
 * analysis.c
 *
 *  Created on: Nov 18, 2019
 *      Author: b7122021
 */
#include "analysis.h"
#include "cross-reference.h"
#include "scan.h"

#define GLOBAL 0
#define LOCAL 1

int token;
int scope=GLOBAL;
char* procname;
TYPE *tp;


int parse_program(){
	token=scan();
	if(token != TPROGRAM) return(error("Syntax error.\nKeyword 'program' is not found."));
	token = scan();
	if(token != TNAME) return(error("Syntax error.\nProgram name is not found."));
	token = scan();
	if(token != TSEMI) return(error("Syntax error.\nSemicolon is not found."));
	token = scan();
	if(block() == ERROR) return(ERROR);
	if(token != TDOT) return(error("Syntax error.\nPeriod is not found at the end of program."));
	token = scan();
	return(NORMAL);
}

int block(){
	while((token == TVAR)||(token == TPROCEDURE)){
		switch(token){
		case (TVAR):if(var_decl() == ERROR) return(ERROR);break;
		case (TPROCEDURE):if(subprg_decl() == ERROR) return(ERROR);break;
		}
		scope=GLOBAL;
	}
	if(cmp_statement() == ERROR) return(ERROR);
	return(NORMAL);
}

int var_decl(){
	VARNAME *vp;

	vp=NULL;

	token = scan();
	if(token != TNAME) return(error("Syntax error on variable declaration.\nVariable name is not found."));
	do{
		vp=addnamelist(vp,string_attr);
		token = scan();
		switch(token){
		case(TCOLON):
		case(TCOMMA):break;
		default:
			return(error("Syntax error on variable declaration.\nNeed colon or comma."));
		}
		token = scan();
	}while(token == TNAME);
	if(type() == ERROR) return(ERROR);
	switch(scope){
	case GLOBAL:if((globalvar_register(vp,tp))==ERROR) return(ERROR);break;
	case LOCAL:if((localvar_register(procname,vp,tp))==ERROR) return(ERROR);break;
	}
	token = scan();
	if(token != TSEMI) return(error("Syntax error on variable declaration.\nSemicolon is not found."));
	vp=NULL;
	token = scan();
	while(token == TNAME){
		do{
			vp=addnamelist(vp,string_attr);
			token = scan();
			switch(token){
			case(TCOLON):break;
			case(TCOMMA):break;
			default:
				return(error("Syntax error on variable declaration.\nNeed colon or comma."));
			}
			token = scan();
		}while(token == TNAME);
		if(type() == ERROR) return(ERROR);
		switch(scope){
		case GLOBAL:if((globalvar_register(vp,tp))==ERROR) return ERROR;break;
		case LOCAL:if((localvar_register(procname,vp,tp))==ERROR) return ERROR;break;
		}
		token = scan();
		if(token != TSEMI) return(error("Syntax error on variable declaration.\nSemicolon is not found."));
		vp=NULL;
		token = scan();
	}
	return(NORMAL);
}

int type(){
	if((tp = (TYPE*)malloc(sizeof(TYPE))) == NULL) return(error("can not malloc.(TYPE)"));
	switch(token){
	case(TINTEGER):tp->ttype=TPINT; return(NORMAL);
	case(TBOOLEAN):tp->ttype=TPBOOL; return(NORMAL);
	case(TCHAR):tp->ttype=TPCHAR; return(NORMAL);
	case(TARRAY):break;
	default:return(error("Syntax error on type declaration."));
	}
	token = scan();
	if(token != TLSQPAREN) return(error("Syntax error on array declaration.\n\"[\" is not found."));
	token = scan();
	if(token != TNUMBER) return(error("Syntax error on array declaration.\nArray number is not found."));
	if(num_attr == 0) return(error("Syntax error on array declaration.\nArray number must be more than 0."));
	tp->arraysize=num_attr;
	token = scan();
	if(token !=TRSQPAREN) return(error("Syntax error on array declaration.\n\"]\" is not found."));
	token = scan();
	if(token != TOF) return (error("Syntax error on array declaration.\nKeyword 'of' is not found."));
	token = scan();
	switch(token){
	case(TINTEGER):tp->ttype=TPARRAYINT; return(NORMAL);
	case(TBOOLEAN):tp->ttype=TPARRAYBOOL; return(NORMAL);
	case(TCHAR):tp->ttype=TPARRAYCHAR; return(NORMAL);
	default:return(error("Syntax error on array declaration.\nNeed standard type(integer/boolean/char)."));
	}
	return(NORMAL);
}

int subprg_decl(){
	PARA *pp;
	token = scan();
	if(token != TNAME) return(error("Syntax error on subprogram declaration.\nProcedure name is not found."));
	if((procname = (char*)malloc(strlen(string_attr)+1)) == NULL) return(error("can not malloc.(char)"));
	strcpy(procname,string_attr);
	token = scan();
	if((pp=(PARA*)malloc(sizeof(PARA)))==NULL) return(error("can not malloc.(PARA)"));
	if(token == TLPAREN){
		token = scan();
		if(token != TNAME) return(error("Syntax error on formal parameters.\nVariable name is not found."));
		do{
			pp->paraname=addnamelist(pp->paraname,string_attr);
			token = scan();
			switch(token){
			case(TCOLON):break;
			case(TCOMMA):break;
			default:return(error("Syntax error on formal parameters.\nNeed colon or comma."));
			}
			token = scan();
		}while(token == TNAME);
		if(type() == ERROR) return(ERROR);
		pp->paratp=tp->ttype;
		token = scan();
		while(token == TSEMI){
			pp=pp->nextparap;
			if((pp=(PARA*)malloc(sizeof(PARA)))==NULL) return(error("can not malloc.(PARA)"));
			token = scan();
			if(token != TNAME) return(error("Syntax error on formal parameters.\nVariable name is not found."));
			if((pp->paraname = (VARNAME*)malloc(sizeof(VARNAME))) == NULL) return(error("can not malloc.(VARNAME)"));
			do{
				pp->paraname=addnamelist(pp->paraname,string_attr);
				token = scan();
				switch(token){
				case(TCOLON):break;
				case(TCOMMA):break;
				default:return(error("Syntax error on formal parameters.\nNeed colon or comma."));
				}
				token = scan();
			}while(token == TNAME);
			if(type() != ERROR) return(ERROR);
			pp->paratp=tp->ttype;
			token = scan();
		}
		if(token != TRPAREN) return(error("Syntax error on formal parameters.\n\")\" is not found."));
		token = scan();
	}else pp=NULL;
	if(globalproc_register(procname,pp)==ERROR) return(ERROR);
	if(localpara_register(procname,pp)==ERROR) return(ERROR);
	if(token != TSEMI) return(error("Syntax error on subprogram declaration.\nSemicolon is not found."));
	token = scan();
	scope=LOCAL;
	if(token == TVAR){
		if(var_decl() == ERROR) return(ERROR);
	}
	if(cmp_statement() == ERROR) return(ERROR);
	if(token != TSEMI) return(error("Syntax error on subprogram declaration.\nSemicolon is not found."));
	free(procname);
	token = scan();
	return(NORMAL);
}

int cmp_statement(){
	if(token != TBEGIN) return(error("Syntax error on compound statement.\nKeyword 'begin' is not found."));
	if(statement() == ERROR) return(ERROR);
	while(token == TSEMI){
		if(statement() == ERROR) return(ERROR);
	}
	if(token != TEND) return(error("Syntax error on compound statement.\nKeyword 'begin' is not found."));
	token = scan();
	return(NORMAL);
}

int statement(){
	token = scan();
	switch(token){
	case(TNAME):if(ass_statement() == ERROR) return(ERROR);break;
	case(TIF):if(cnd_statement() == ERROR) return(ERROR);break;
	case(TWHILE):if(itr_statement() == ERROR) return(ERROR);break;
	case(TCALL):if(call_statement() == ERROR) return(ERROR);break;
	case(TBREAK):
	case(TRETURN):{
		token = scan();
		break;
	}
	case(TREAD):
	case(TREADLN):if(inpt_statement() == ERROR) return(ERROR);break;
	case(TWRITE):
	case(TWRITELN):if(outpt_statement() == ERROR) return(ERROR);break;
	case(TBEGIN):if(cmp_statement() == ERROR) return(ERROR);break;
	case(TEND):break;
	default:return(error("Syntax error on statement."));
	}
	return(NORMAL);
}

int ass_statement(){
	int ltype,rtype;
	if((ltype=var()) == ERROR) return(ERROR);
	if((ltype!=TPINT)&&(ltype!=TPCHAR)&&(ltype!=TPBOOL)) return(error("type error."));
	if(token != TASSIGN) return(error("Syntax error on assignment statement.\n\":=\" is not found."));
	token = scan();
	if((rtype=expression()) == ERROR) return(ERROR);
	if((rtype!=TPINT)&&(rtype!=TPCHAR)&&(rtype!=TPBOOL)) return(error("type error."));
	if(ltype!=rtype) return(error("type error."));
	return(NORMAL);
}

int cnd_statement(){
	int etype;
	token = scan();
	if((etype=expression()) == ERROR) return(ERROR);
	if(etype!=TPBOOL) return(error("type error."));
	if(token != TTHEN) return(error("Syntax error on condition statement.\nKeyword 'then' is not found."));
	if(statement() == ERROR) return(ERROR);
	if(token == TELSE){
		if(statement() == ERROR) return(ERROR);
	}
	return(NORMAL);
}

int itr_statement(){
	int etype;
	token = scan();
	if((etype=expression()) == ERROR) return(ERROR);
	if(etype!=TPBOOL) return(error("type error"));
	if(token != TDO) return(error("Syntax error on iteration statement.\nKeyword 'do' is not found."));
	if(statement() == ERROR) return(ERROR);
	return(NORMAL);
}

int call_statement(){
	PARA *pp;
	int etype;
	token = scan();
	if(token != TNAME) return(error("Syntax error on call statement.\nProcedure name is not found."));
	if(strcmp(string_attr,procname)==0) return(error("Recursive call not allowed."));
	if(globalid_countup(string_attr)==ERROR) return(error("undeclared program."));
	if((procname=(char*)malloc(strlen(string_attr)+1))==NULL) return(error("can not malloc.(char)"));
	strcpy(procname,string_attr);
	token = scan();
	pp=search_procpara(procname);
	if(token == TLPAREN){
		token = scan();
		if((etype=expression()) == ERROR) return(ERROR);
		if(pp==NULL) return(error("No-parameter subprogram."));
		if(etype!=pp->paratp) return(error("type error."));
		while(token == TCOMMA){
			pp=pp->nextparap;
			token = scan();
			if((etype=expression()) == ERROR) return(ERROR);
			if(pp==NULL) return(error("too many parameters."));
			if(etype!=pp->paratp) return(error("type error."));
		}
		if(token != TRPAREN) return(error("Syntax error on call statement.\n\")\" is not found."));
		if(pp->nextparap!=NULL) return(error("fewer parameters."));
		token = scan();
	}else if(pp!=NULL) return(error("fewer parameters."));
	return(NORMAL);
}

int inpt_statement(){
	int vtype;
	token = scan();
	if(token == TLPAREN){
		token = scan();
		if((vtype=var()) == ERROR) return(ERROR);
		if((vtype!=TPINT)&&(vtype!=TPCHAR)) return(error("type error."));
		while(token == TCOMMA){
			token = scan();
			if((vtype=var()) == ERROR) return(ERROR);
			if((vtype!=TPINT)||(vtype!=TPCHAR)) return(error("type error."));
		}
		if(token != TRPAREN) return(error("Syntax error on input statement.\n\")\" is not found."));
		token = scan();
	}
	return(NORMAL);
}

int outpt_statement(){
	int etype;
	token = scan();
	if(token == TLPAREN){
		token = scan();
		if(token == TSTRING){
			if((strlen(string_attr)==1)||(strlen(string_attr)<0)) return(error("string length is too small."));
			token = scan();
		}else if((etype=expression()) == ERROR) return(ERROR);
		else{
			if((etype!=TPINT)&&(etype!=TPCHAR)&&(etype!=TPBOOL)) return(error("type error."));
			if(token == TCOLON){
				token = scan();
				if(token != TNUMBER) return(error("Syntax error on input statement.\nNumber is not found."));
				token = scan();
			}
		}
		while(token == TCOMMA){
			token = scan();
			if(token == TSTRING){
				if(((strlen(string_attr)==1)&&(strcmp(string_attr," ")!=0))||(strlen(string_attr)<0)) return(error("string length is too small."));
				token = scan();
			}else if((etype=expression()) == ERROR) return(ERROR);
			else{
				if((etype!=TPINT)&&(etype!=TPCHAR)&&(etype!=TPBOOL)) return(error("type error."));
				if(token == TCOLON){
					token = scan();
					if(token != TNUMBER) return(error("Syntax error on input statement.\nNumber is not found."));
					token = scan();
				}
			}
		}
		if(token != TRPAREN) return(error("Syntax error on output statement.\n\")\" is not found."));
		token = scan();
	}
	return(NORMAL);
}

int var(){
	int vtype,etype;

	switch(scope){
	case GLOBAL:{
		if(globalid_countup(string_attr)==ERROR) return(error("undeclared variable."));
		tp=search_globalvartype(string_attr);
		break;
	}
	case LOCAL:{
		if(localid_countup(string_attr,procname)==ERROR){
			if(globalid_countup(string_attr)==ERROR) return(error("undeclared variable."));
			tp=search_globalvartype(string_attr);
		}else	tp=search_localvartype(string_attr,procname);
		break;
	}
	}
	token = scan();
	vtype=tp->ttype;
	if(token == TLSQPAREN){
		switch(vtype){
		case TPINT:
		case TPCHAR:
		case TPBOOL:
		case TPPROC:return(error("type error."));
		case TPARRAYINT:vtype=TPINT;break;
		case TPARRAYCHAR:vtype=TPCHAR;break;
		case TPARRAYBOOL:vtype=TPBOOL;break;
		}
		token = scan();
		if((etype=expression()) == ERROR) return(ERROR);
		if(etype!=TPINT) return(error("type error."));
		if(token != TRSQPAREN) return(error("Syntax error on variable.\n\"]\" is not found."));
		token = scan();
	}
	return(vtype);
}

int expression(){
	int ltype,rtype,etype;
	if((ltype=simple_expression()) == ERROR) return(ERROR);
	etype=ltype;
	while((token == TEQUAL)||(token == TNOTEQ)||(token == TLE)||(token == TLEEQ)||(token == TGR)||(token == TGREQ)){
		token = scan();
		if((rtype=simple_expression()) == ERROR) return(ERROR);
		if(ltype!=rtype) return(error("type error."));
		etype=TPBOOL;
	}
	return(etype);
}
int simple_expression(){
	int ltype,rtype,stype;
	if((token == TPLUS)||(token == TMINUS)){
		token = scan();
		if((ltype=term()) == ERROR) return(ERROR);
		if(ltype!=TPINT) return(error("type error."));
	}else{
		if((ltype=term()) == ERROR) return(ERROR);
	}
	stype=ltype;
	while((token == TPLUS)||(token == TMINUS)||(token == TOR)){
		if(token==TOR){
			if(ltype!=TPBOOL) return(error("type error."));
			token = scan();
			if((rtype=term()) == ERROR) return(ERROR);
			if(rtype!=TPBOOL) return(error("type error."));
			stype=TPBOOL;
		}else{
			if(ltype!=TPINT) return(error("type error."));
			token = scan();
			if((rtype=term()) == ERROR) return(ERROR);
			if(rtype!=TPINT) return(error("type error."));
			stype=TPINT;
		}
	}
	return(stype);
}

int term(){
	int ltype,rtype,ttype;
	if((ltype=fact()) == ERROR) return(ERROR);
	ttype=ltype;
	while((token == TSTAR)||(token == TDIV)||(token == TAND)){
		if(token == TAND){
			if(ltype!=TPBOOL) return(error("type error."));
			token = scan();
			if((rtype=fact()) == ERROR) return(ERROR);
			if(rtype!=TPBOOL) return(error("type error."));
			ttype=TPBOOL;
		}else{
			if(ltype!=TPINT) return(error("type error."));
			token = scan();
			if((rtype=fact()) == ERROR) return(ERROR);
			if(rtype!=TPINT) return(error("type error."));
			ttype=TPINT;
		}

	}
	return(ttype);
}

int fact(){
	int ftype,etype;
	switch(token){
	case(TNAME):if((ftype=var()) == ERROR) return(ERROR);break;
	case(TNUMBER):{
		ftype=TPINT;
		token = scan();
		break;
	}
	case(TFALSE):
	case(TTRUE):{
		ftype=TPBOOL;
		token = scan();
		break;
	}
	case(TSTRING):{
		if(strlen(string_attr)!=1) return(error("string length must be 1."));
		ftype=TPCHAR;
		token = scan();
		break;
	}
	case(TLPAREN):{
		token = scan();
		if((ftype=expression()) == ERROR) return(ERROR);
		if(token != TRPAREN) return(ERROR);
		token = scan();
		break;
	}
	case(TNOT):token = scan();return((ftype=fact())==TPBOOL? ftype:error("type error."));
	case(TINTEGER):{
		token = scan();
		if(token != TLPAREN) return(ERROR);
		token = scan();
		if((etype=expression()) == ERROR) return(ERROR);
		if((etype!=TPINT)&&(etype!=TPCHAR)&&(etype!=TPBOOL)) return(error("type error."));
		if(token != TRPAREN) return(ERROR);
		token = scan();
		ftype=TPINT;
		break;
	}
	case(TBOOLEAN):{
		token = scan();
		if(token != TLPAREN) return(ERROR);
		token = scan();
		if((etype=expression()) == ERROR) return(ERROR);
		if((etype!=TPINT)&&(etype!=TPCHAR)&&(etype!=TPBOOL)) return(error("type error."));
		if(token != TRPAREN) return(ERROR);
		token = scan();
		ftype=TPBOOL;
		break;
	}
	case(TCHAR):{
		token = scan();
		if(token != TLPAREN) return(ERROR);
		token = scan();
		if((etype=expression()) == ERROR) return(ERROR);
		if((etype!=TPINT)&&(etype!=TPCHAR)&&(etype!=TPBOOL)) return(error("type error."));
		if(token != TRPAREN) return(ERROR);
		token = scan();
		ftype=TPCHAR;
		break;
	}
	}
	return(ftype);
}
