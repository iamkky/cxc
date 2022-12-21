#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//#include "abjson/jsonvalue.h"
#include "cx_component.h"
#include "cx_parser.h"
#include "cx_parser.tokens.h"

static int vvflag = 0;

char *readToBuffer(int fd, int inisize, int extrasize, int *readsize)
{
int bsize, r, rsize;
char *buffer, *tmp;

	bsize=inisize;
	if((buffer = malloc(bsize + extrasize))==NULL) return NULL;

	rsize=0;

	do{
		if(rsize>=bsize){
			bsize += bsize;
			if((tmp = realloc(buffer, bsize + extrasize))==NULL){
				*readsize = 0;
				free(buffer);
				return NULL;
			}
			buffer = tmp;
		} 
		r = read(fd, buffer+rsize, bsize - rsize);
		rsize += r;
	}while(r>0);

	*readsize = rsize;
	return buffer;
}

int printLine(FILE *fp, char *line)
{
	while(*line!=0 && *line!='\n'){
		fputc(*line++, fp);
	}
	fputc('\n', fp);
}

int unexpected_handler(CxParser self, int token, int nonterminal)
{
int size;

	fprintf(stderr,"Error: Expected: token: %d\n", token);
}

int backtrackFail_handler(CxParser self, int nonterminal)
{
int size;

	fprintf(stderr,"Error: Backtracking failed:  %d (%s)\n", nonterminal, Rdpp_CxParserNonterminals_Names[nonterminal-10000]);
}


int main(int argc, char **argv)
{
CxParser		cx;
CxParserExtraDataType	extra;
CxComponentList		list;
char			*buffer, *p;
int			bytes, result, lcount;

	//bytes = read(0, buffer, 32767);
	buffer = readToBuffer(0, 32768, 1, &bytes);
	buffer[bytes] = 0;

	p = buffer;

	extra.lnumber = 1;
	extra.line_start = buffer;

	while(*p!=0){

		if(*p!='{' || (*p=='{' && *(p+1)!='%')){
			if(*p=='\n') {
				extra.lnumber++;
				extra.line_start = p + 1;
			}
			fputc(*p++, stdout);
			continue;
		}

		p += 2;

		if (vvflag) fprintf(stderr,"Fragment found %d: >%-20.20s<\n", extra.lnumber, p);
		cx = CxParserNew(p, &extra);	

		CxParserSetUnexpected(cx, unexpected_handler);
		CxParserSetBacktrackFail(cx, backtrackFail_handler);
	
		result = CxParserParse(cx);
	
		//fprintf(stdout,"Input Size: %d\n", bytes);
		//fprintf(stdout,"cursor: %d\n", cx->cursor);
		//fprintf(stdout,"Last Token: %d\n", cx->currToken);
	
		if(result>0){
  			//fprintf(stdout,"\nResult:\n");
			//fprintf(stdout,"\nPrint:\n");
			//cxComponentListPrint(list, stdout, 0);
			//fprintf(stdout,"\nCode:\n");
			list = cx->value[0].complist;
			cxComponentListGenCode(list, stdout, 0);
			p += cx->cursor;
		}else{
			fprintf(stderr,"Failed to parse line %d\n", extra.lnumber);
		 	printLine(stderr, extra.line_start);
			exit(-1);
		}
		
		if(*p!='%' || (*p=='%' && *(p+1)!='}')){
			fprintf(stderr,"Missing %} at %d\n", extra.lnumber);
		 	printLine(stderr, extra.line_start);
			exit(-1);
		}

		p += 2;

		CxParserFree(cx);
	}	
	free(buffer);
}

