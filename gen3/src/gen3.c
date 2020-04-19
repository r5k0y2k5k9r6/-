/*
 ============================================================================
 Name        : gen3.c
 Author      : Rimi
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "scan.h"
#include "analysis.h"
#include "cross-reference.h"

int main(int nc, char *np[]) {
	if(nc < 2) {
			printf("File name id not given.\n");
			return 0;
		}
		if(init_scan(np[1]) < 0) {
			printf("File %s can not open.\n", np[1]);
			return 0;
		}
		if(init_idtab()==ERROR) return -1;
		if(parse_program()==ERROR)return -1;
		end_scan();
		print_idtab();
		release_idtab();
		return 0;
}
