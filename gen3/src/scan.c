/*
 * scan.c
 *
 *  Created on: Oct 7, 2019
 *      Author: b7122021
 */
#include "scan.h"

#include <ctype.h>

FILE *fp;
char buf='\0',cbuf='\0'; /*今の文字と1文字先の文字*/
char buffer[MAXSTRSIZE]; /*調査中の文字列*/
int linenum=0;
int eno;
int num_attr;
char string_attr[MAXSTRSIZE];
char *scanerror[4]={
		"Number is too large.",
		"String or Name is too long.",
		"Newline Character is included in the String.",
		"End-of-File in the String."};

struct KEY key[KEYWORDSIZE] = {
		{"and", 	TAND	},
		{"array",	TARRAY	},
		{"begin",	TBEGIN	},
		{"boolean",	TBOOLEAN},
		{"break",	TBREAK  },
		{"call",	TCALL	},
		{"char",	TCHAR	},
		{"div",		TDIV	},
		{"do",		TDO	},
		{"else",	TELSE	},
		{"end",		TEND	},
		{"false",	TFALSE	},
		{"if",		TIF	},
		{"integer",	TINTEGER},
		{"not",		TNOT	},
		{"of",		TOF	},
		{"or",		TOR	},
		{"procedure", TPROCEDURE},
		{"program",	TPROGRAM},
		{"read",	TREAD	},
		{"readln",	TREADLN },
		{"return", 	TRETURN },
		{"then",	TTHEN	},
		{"true",	TTRUE	},
		{"var",		TVAR	},
		{"while",	TWHILE	},
		{"write",	TWRITE  },
		{"writeln",	TWRITELN}
};


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
				return 0;
			}
		}
		if(iscntrl(buf)||buf ==' '){
			if((buf == '\n' && cbuf != '\r') || (buf == '\r' && cbuf != '\n') || (buf == '\n' && cbuf == '\r') || (buf == '\r' && cbuf == '\n')){
				if((buf == '\n' && cbuf == '\r') || (buf == '\r' && cbuf == '\n')){
					buf=cbuf;
					if((fscanf(fp,"%c",&cbuf)) == EOF){
						return 0;
					}
				}
				linenum++;
			}
		}else 	if(isdigit(buf)){/*number*/
			i=0;
			buffer[i]=buf;
			for(i=1;isdigit(cbuf);i++){
				buf=cbuf;
				buffer[i]=buf;
				fscanf(fp,"%c",&cbuf);
			}
			strcpy(string_attr,buffer);
			num_attr=atoi(buffer);
			if(num_attr>32767){
				eno=0;
				return(error(scanerror[eno]));
			}
			return TNUMBER;
		}else if(isalpha(buf)){/*keyword,name*/
			i=0;
			buffer[i]=buf;
			for(i=1;isalpha(cbuf);i++){
				buf=cbuf;
				buffer[i]=buf;
				fscanf(fp,"%c",&cbuf);
			}
			for(a=0;a<KEYWORDSIZE;a++){ /*keyword*/
				if(strcmp(buffer,key[a].keyword) == 0){
					return key[a].keytoken;
				}
			}
			while(isalpha(cbuf)||isdigit(cbuf)){
				if(i>=MAXSTRSIZE-1){
					eno=1;
					return(error(scanerror[eno]));
				}
				buf=cbuf;
				buffer[i]=buf;
				i++;
				fscanf(fp,"%c",&cbuf);
			}
			strcpy(string_attr,buffer);
			return TNAME;
		}else{
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
					return(error(scanerror[eno]));
				}
				return TSTRING;
			case '{':
				comment();
				break;
			case '/':
					comment();
					break;
			}
		}
	}
}

int get_linenum(void){
	return linenum;
}

int error(char* message){
	printf("\nERROR:%s (line:%d)\n",message,linenum);
	end_scan();
	return -1;
}

void end_scan(void){
	fclose(fp);
}

void string(void){
	int i=0;
	while(cbuf !='\''){
		if(i>=MAXSTRSIZE-1){
			eno=1;
			return;
		}
		buf = cbuf;
		if(buf =='\n'|| buf =='\r'){
			eno=2;
			return;
		}
		string_attr[i++]=buf;
		if(fscanf(fp,"%c",&cbuf) == EOF){
			eno=3;
			return;
		}
	}
	buf = cbuf;
	fscanf(fp,"%c",&cbuf);
	if((isdigit(cbuf))||(isalpha(cbuf))){
		string_attr[i]=buf;
		i++;
		while(cbuf !='\''){
			if(i>=MAXSTRSIZE-1){
				eno=1;
				return;
			}
			buf = cbuf;
			if(buf =='\n'|| buf =='\r'){
				eno=2;
				return;
			}
			string_attr[i]=buf;
			if(fscanf(fp,"%c",&cbuf) == EOF){
				eno=3;
				return;
			}
			i++;
		}
		buf = cbuf;
		fscanf(fp,"%c",&cbuf);
	}
	while(cbuf =='\''){
		fscanf(fp,"%c",&cbuf);
		while(cbuf !='\''){
			if(i>=MAXSTRSIZE-1){
				eno=1;
				return;
			}
			buf = cbuf;
			if(buf =='\n'|| buf =='\r'){
				eno=2;
				return;
			}
			string_attr[i++]=buf;
			if(fscanf(fp,"%c",&cbuf) == EOF){
				eno=3;
				return;
			}
		}
		buf = cbuf;
		fscanf(fp,"%c",&cbuf);
		if((isdigit(cbuf))||(isalpha(cbuf))){
			string_attr[i]=buf;
			i++;
			while(cbuf !='\''){
				if(i>=MAXSTRSIZE-1){
					eno=1;
					return;
				}
				buf = cbuf;
				if(buf =='\n'|| buf =='\r'){
					eno=2;
					return;
				}
				string_attr[i]=buf;
				if(fscanf(fp,"%c",&cbuf) == EOF){
					eno=3;
					return;
				}
				i++;
			}
			buf = cbuf;
			fscanf(fp,"%c",&cbuf);
		}
	}
}
void comment(void){
	switch(buf){
	case '/':
		fscanf(fp,"%c%c",&buf,&cbuf);
		while(buf != '*'){
			buf = cbuf;
			fscanf(fp,"%c",&cbuf);
		}
		 if(cbuf != '/'){
			 buf=cbuf;
			 while(buf != '*'){
			 			buf = cbuf;
			 			fscanf(fp,"%c",&cbuf);
			 		}
		 }
		break;
	case '{':
		while(cbuf != '}'){
			buf = cbuf;
			fscanf(fp,"%c",&cbuf);
		}
		break;
	}
	buf = cbuf;
	fscanf(fp,"%c",&cbuf);

}
