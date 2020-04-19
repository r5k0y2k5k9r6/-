/*
 * cross-reference.c
 *
 *  Created on: 2020/01/06
 *      Author: rimikawasaki
 */
#include "cross-reference.h"
#include "scan.h"

ID *globalidroot, *localidroot;
int init_idtab(){ /*initialise the table*/
	if((globalidroot=(ID*)malloc(sizeof(ID)))==NULL) return(error("can not malloc.(ID)"));
	if((localidroot=(ID*)malloc(sizeof(ID)))==NULL) return(error("can not malloc.(ID)"));
	globalidroot->nextp=NULL;
	localidroot->nextp=NULL;
	return(NORMAL);
}

ID *search_globalidtab(char *np){ /*search the name pointed by np*/
	ID *p;

	for(p=globalidroot;p->nextp!=NULL;p=p->nextp){
		if(strcmp(np,p->nextp->name)==0) return(p);
	}
	return(p);
}

int globalvar_register(VARNAME *vp,TYPE *tp) { /*register the variable pointed by vp*/
	ID *idp,*tail;
	VARNAME *varp;

	for(varp=vp;varp!=NULL;varp=varp->nextvarp){
		tail=search_globalidtab(varp->varname);
		if(tail->nextp!=NULL) return(error("already declared variable."));
		if((idp=(ID*)malloc(sizeof(ID)))==NULL) return(error("can not malloc.(ID)"));
		if((idp->itp=(TYPE*)malloc(sizeof(TYPE)))==NULL) return(error("can not malloc.(TYPE)"));
		if((idp->irefp=(LINE*)malloc(sizeof(LINE)))==NULL) return(error("can not malloc.(LINE)"));
		if((idp->name=(char*)malloc(strlen(varp->varname)+1))==NULL) return(error("can not malloc.(char)"));
		strcpy(idp->name,varp->varname);
		idp->itp=tp;
		idp->deflinenum=get_linenum();
		idp->irefp->nextlinep=NULL;
		idp->nextp=NULL;
		tail->nextp=idp;
	}
	return(NORMAL);
}

int globalproc_register(char *np,PARA *pp){ /*register the program pointed by np*/
	ID *idp,*tail;

	tail=search_globalidtab(np);
	if(tail->nextp!=NULL) return(error("already declared program."));
	if((idp=(ID*)malloc(sizeof(ID)))==NULL) return(error("can not malloc.(ID)"));
	if((idp->itp=(TYPE*)malloc(sizeof(TYPE)))==NULL) return(error("can not malloc.(TYPE)"));
	if((idp->itp->paratp=(PARA*)malloc(sizeof(PARA)))==NULL) return(error("can not malloc.(PARA)"));
	if((idp->irefp=(LINE*)malloc(sizeof(LINE)))==NULL) return(error("can not malloc.(LINE)"));
	if((idp->name=(char*)malloc(strlen(np)+1))==NULL) return(error("can not malloc.(char)"));
	strcpy(idp->name,np);
	idp->itp->ttype=TPPROC;
	if(pp!=NULL){
		idp->itp->paratp=pp;
		idp->ispara=1;
	}else{
		idp->ispara=0;
		idp->itp->paratp=NULL;
	}
	idp->deflinenum=get_linenum();
	idp->irefp->nextlinep=NULL;
	idp->nextp=NULL;
	tail->nextp=idp;
	return(NORMAL);
}

int globalid_countup(char *np){
	ID *p;
	LINE *lp,*newline;
	p=search_globalidtab(np);
	if(p->nextp== NULL) return(ERROR);
	lp=p->nextp->irefp;
	while(lp->nextlinep!=NULL) lp=lp->nextlinep;
	if((newline=(LINE*)malloc(sizeof(LINE)))==NULL) return(error("can not malloc.(LINE)"));
	newline->reflinenum=get_linenum();
	newline->nextlinep=NULL;
	lp->nextlinep=newline;
	return(NORMAL);
}

ID *search_localidtab(char *np,char *procnp){
	ID *p;

	for(p=localidroot;p->nextp!=NULL;p=p->nextp){
		if((strcmp(np,p->nextp->name)==0)&&(strcmp(procnp,p->nextp->procname)==0)) break;
	}
	return(p);
}

int localpara_register(char *np,PARA *pp){ /*register np's parameter pointed by pp*/
	ID *idp,*tail;
	PARA *parap;
	VARNAME *varp;

	for(parap=pp;parap!=NULL;parap=pp->nextparap){
		for(varp=parap->paraname;varp!=NULL;varp=varp->nextvarp){
			tail=search_localidtab(varp->varname,np);
			if(tail->nextp!=NULL) return(error("already declared parameter."));
			if((idp=(ID*)malloc(sizeof(ID)))==NULL) return(error("can not malloc.(ID)"));
			if((idp->itp=(TYPE*)malloc(sizeof(TYPE)))==NULL) return(error("can not malloc.(TYPE)"));
			if((idp->irefp=(LINE*)malloc(sizeof(LINE)))==NULL) return(error("can not malloc.(LINE)"));
			if((idp->name=(char*)malloc(strlen(varp->varname)+1))==NULL) return(error("can not malloc.(char)"));
			if((idp->procname=(char*)malloc(strlen(np)+1))==NULL) return(error("can not malloc.(char)"));
			strcpy(idp->name,varp->varname);
			strcpy(idp->procname,np);
			idp->itp->ttype=parap->paratp;
			idp->deflinenum=get_linenum();
			idp->irefp->nextlinep=NULL;
			idp->nextp=NULL;
			tail->nextp=idp;
		}
	}
	return(NORMAL);
}

int localvar_register(char *np,VARNAME *vp,TYPE *tp){ /*register np's variable pointed by vp*/
	ID *idp,*tail;
	VARNAME *varp;

	for(varp=vp;varp!=NULL;varp=varp->nextvarp){
		tail=search_localidtab(varp->varname,np);
		if(tail->nextp!=NULL) return(error("already declared variable."));
		if((idp=(ID*)malloc(sizeof(ID)))==NULL) return(error("can not malloc.(ID)"));
		if((idp->itp=(TYPE*)malloc(sizeof(TYPE)))==NULL) return(error("can not malloc.(TYPE)"));
		if((idp->irefp=(LINE*)malloc(sizeof(LINE)))==NULL) return(error("can not malloc.(LINE)"));
		if((idp->name=(char*)malloc(strlen(varp->varname)+1))==NULL) return(error("can not malloc.(char)"));
		if((idp->procname=(char*)malloc(strlen(np)+1))==NULL) return(error("can not malloc.(char)"));
		strcpy(idp->name,varp->varname);
		strcpy(idp->procname,np);
		idp->itp=tp;
		idp->deflinenum=get_linenum();
		idp->irefp->nextlinep=NULL;
		idp->nextp=NULL;
		tail->nextp=idp;
	}
	return(NORMAL);
}

int localid_countup(char *np,char *procnp){
	ID *p;
	LINE *lp,*newline;

	p=search_localidtab(np,procnp);
	if(p->nextp==NULL)	return(ERROR);
	lp=p->nextp->irefp;
	while(lp->nextlinep!=NULL) lp=lp->nextlinep;
	if((newline=(LINE*)malloc(sizeof(LINE)))==NULL) return(error("can not malloc.(LINE)"));
	newline->reflinenum=get_linenum();
	newline->nextlinep=NULL;
	lp->nextlinep=newline;
	return(NORMAL);
}

VARNAME *addnamelist(VARNAME *list,char *name){
	VARNAME *top,*new;

	top=list;
	new=(VARNAME*)malloc(sizeof(VARNAME));
	new->varname=(char*)malloc(strlen(name)+1);
	strcpy(new->varname,name);
	new->nextvarp=NULL;

	if(list==NULL){
		list=new;
		return list;
	}
	while(list->nextvarp!=NULL) list=list->nextvarp;
	list->nextvarp=new;
	return top;
}

PARA *search_procpara(char *np){
	ID *p;

	p=search_globalidtab(np);
	return(p->nextp->itp->paratp);
}

TYPE *search_globalvartype(char *np){
	ID *p;

	p=search_globalidtab(np);
	return(p->nextp->itp);
}
TYPE *search_localvartype(char *np,char *procnp){
	ID *p;

	p=search_localidtab(np,procnp);
	return(p->nextp->itp);
}

void print_idtab(){ /* Output the registered data */
	struct ID *gip,*lip;
	struct PARA *pp;
	struct LINE *lp;
	char *type[7]={"integer","char","boolean","array[integer]","array[char]","array[boolean]","procedure"};

	printf("Name\tType\t\tDef.|Ref.\n");
	for(gip=globalidroot->nextp;gip!=NULL;gip=gip->nextp) {
		if(gip->itp->ttype==TPPROC){
			printf("%s\t%s", gip->name, type[(gip->itp->ttype)-1]);
			if(gip->ispara==1){
				printf("(");
				pp=gip->itp->paratp;
				printf("%s",type[(pp->paratp)-1]);
				while(pp->nextparap!=NULL){
					pp=pp->nextparap;
					printf(",%s",type[(pp->paratp)-1]);
				}
				printf(")");
			}
			printf("\t%d|",gip->deflinenum);
			if((lp=gip->irefp->nextlinep)!=NULL){
				printf("%d",lp->reflinenum);
				while(lp->nextlinep!=NULL){
					lp=lp->nextlinep;
					printf(",%d",lp->reflinenum);
				}
			}
			printf("\n");
			for(lip=localidroot->nextp;lip!=NULL;lip=lip->nextp){
				if(strcmp(lip->procname,gip->name)==0){
					printf("%s(%s)\t%s\t%d|", lip->name,lip->procname,type[(lip->itp->ttype)-1],lip->deflinenum);
					if((lp=lip->irefp->nextlinep)!=NULL){
						printf("%d",lp->reflinenum);
						while(lp->nextlinep!=NULL){
							lp=lp->nextlinep;
							printf(",%d",lp->reflinenum);
						}
					}
					printf("\n");
				}
			}
		}else{
			printf("%s\t%s\t%d|", gip->name,type[(gip->itp->ttype)-1],gip->deflinenum);
			if((lp=gip->irefp->nextlinep)!=NULL){
				printf("%d",lp->reflinenum);
				while(lp->nextlinep!=NULL){
					lp=lp->nextlinep;
					printf(",%d",lp->reflinenum);
				}
			}
			printf("\n");
		}
	}

}

void release_idtab(){ 	/* Release the data structure */
	struct ID *p,*q;

	q=globalidroot;
	for(p=globalidroot->nextp;p!=NULL;p=q->nextp) {
		q->nextp=p->nextp;
		free(p->name);
		free(p->itp);
		free(p->irefp);
		free(p);
		p=q->nextp;
	}
	q=localidroot;
	for(p=localidroot->nextp;p!=NULL;p=q->nextp) {
		q->nextp=p->nextp;
		free(p->name);
		free(p->itp);
		free(p->irefp);
		free(p);
	}

}
