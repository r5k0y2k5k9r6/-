/*
 * scan.c
 *
 *  Created on: Oct 7, 2019
 *      Author: b7122021
 */
#include "token-list.h"
#include <ctype.h>
#include <string.h>

FILE *fp;
char buf='\0',cbuf='\0'; /*今の文字と1文字先の文字*/
char buffer[MAXSTRSIZE]; /*調査中の文字列*/
int linenum=0;
int eno;
int num_attr;
char string_attr[MAXSTRSIZE];
char *etext[4]={
		"Number is too large.",
		"String or Name is too long.",
		"Newline Character is included in the String.",
		"End-of-File in the String."};


int init_scan(char *filename){
	if ((fp = fopen(filename, "r")) == NULL) {
		return -1;
	}
	return 0;
}

int scan(void){
	int i=0, a=0;
	eno=-1;

	for(i=0;i<MAXSTRSIZE;i++){
		buffer[i]='\0';
		string_attr[i]='\0';
	}
	while(1){
		if(linenum == 0){ /*start*/
			fscanf(fp,"%c%c",&buf,&cbuf);
			linenum++;
		}else{ /*other*/
			buf=cbuf;
			if((fscanf(fp,"%c",&cbuf)) == EOF){
				printf("File scan finished.\n");
				printf("linenum:%d\n",linenum);
				return -1;
			}
		}
		if(iscntrl(buf)||buf ==' '){
			if((buf == '\n' && cbuf != '\r') || (buf == '\r' && cbuf != '\n') || (buf == '\n' && cbuf == '\r') || (buf == '\r' && cbuf == '\n')){
				if((buf == '\n' && cbuf == '\r') || (buf == '\r' && cbuf == '\n')){
					buf=cbuf;
					if((fscanf(fp,"%c",&cbuf)) == EOF){
						printf("File scan finished.\n");
						printf("linenum:%d\n",linenum);
						return -1;
					}
				}
				linenum++;
			}
		}else 	if(isdigit(buf)){/*number*/
			printf("check number\n");
			i=0;
			buffer[i]=buf;
			for(i=1;isdigit(cbuf);i++){
				buf=cbuf;
				buffer[i]=buf;
				fscanf(fp,"%c",&cbuf);
			}
			num_attr=atoi(buffer);
			printf("number:%d\n",num_attr);
			if(num_attr>32767){
				eno=0;
				error(eno);
				return -1;
			}
			return TNUMBER;
		}else if(isalpha(buf)){/*keyword,name*/
			printf("check key or name\n");
			i=0;
			buffer[i]=buf;
			for(i=1;isalpha(cbuf);i++){
				buf=cbuf;
				buffer[i]=buf;
				fscanf(fp,"%c",&cbuf);
			}
			printf("key:%s\n",buffer);
			for(a=0;a<KEYWORDSIZE;a++){ /*keyword*/
				if(strcmp(buffer,key[a].keyword) == 0){
					printf("match! %s\n",key[a].keyword);
					return key[a].keytoken;
				}
			}
			printf("check name\n");
			while(isalpha(cbuf)||isdigit(cbuf)){
				if(i>=MAXSTRSIZE-1){
					eno=1;
					error(eno);
					return -1;
				}
				buf=cbuf;
				buffer[i]=buf;
				i++;
				fscanf(fp,"%c",&cbuf);
			}
			strcpy(string_attr,buffer);
			printf("name:%s\n",string_attr);
			return TNAME;
		}else{
			printf("check others(buf:%c)\n",buf);
			switch(buf){
			case '+':return TPLUS;
			case '-':return TMINUS;
			case '*':return TSTAR;
			case '=':return TEQUAL;
			case '<':
				if(cbuf =='>'||cbuf =='='){
					buf=cbuf;
					fscanf(fp,"%c",&cbuf);
					return (cbuf == '>')? TNOTEQ:TLEEQ;
				}else return TLE;
			case '>':
				if(cbuf =='='){
					buf=cbuf;
					fscanf(fp,"%c",&cbuf);
					return TGREQ;
				}else return TGR;
			case '(':return TLPAREN;
			case ')':return TRPAREN;
			case '[':return TLSQPAREN;
			case ']':return TRSQPAREN;
			case ':':
				if(cbuf =='='){
					buf=cbuf;
					fscanf(fp,"%c",&cbuf);
					return TASSIGN;
				}else return TCOLON;
			case ',':return TCOMMA;
			case ';':return TSEMI;
			case '.':return TDOT;
			case '\'':
				string();
				if(eno!=-1){
					error(eno);
					return -1;
				}
				return TSTRING;
			case '{':
				comment();
				break;
			}
		}
	}
}

int get_linenum(void){
	return linenum;
}
void error(int eno){
	printf("\nERROR:%s (line:%d)\n",etext[eno],linenum);
	return;
}

void end_scan(void){
	fclose(fp);
}

void string(void){
	printf("check string\n");
	int i=0;
	while(cbuf !='\''){
		if(i>=MAXSTRSIZE-1){
			eno=1;
			return;
		}
		buf = cbuf;
		if(cbuf =='\n'|| cbuf =='\r'){
			eno=2;
			return;
		}
		fscanf(fp,"%c",&cbuf);
		string_attr[i]=buf;
		i++;
	}
	buf = cbuf;
	fscanf(fp,"%c",&cbuf);
	printf("string:%s\n",string_attr);
}
void comment(void){
	printf("check comment\n");
	switch(buf){
	case '/':
		while(buf != '*' && cbuf != '/'){
			fscanf(fp,"%c%c",&buf,&cbuf);
		}
		break;
	case '{':
		while(cbuf != '}'){
			buf = cbuf;
			fscanf(fp,"%c",&cbuf);
		}
		break;
	}
	printf("comment end\n");
	buf = cbuf;
	fscanf(fp,"%c",&cbuf);

}
