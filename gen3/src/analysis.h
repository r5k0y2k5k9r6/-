/*
 * analysis.h
 *
 *  Created on: Nov 18, 2019
 *      Author: b7122021
 */
#ifndef ANALYSIS_H_
#define ANALYSIS_H_


/*analysis.c*/
extern int parse_program(void);
int block(void);
int var_decl(void);
int type(void);
int subprg_decl(void);
int cmp_statement(void);
int statement(void);
int ass_statement(void);
int cnd_statement(void);
int itr_statement(void);
int call_statement(void);
int inpt_statement(void);
int outpt_statement(void);
int var(void);
int expression(void);
int simple_expression(void);
int term(void);
int fact(void);
#endif /* ANALYSIS_H_ */
