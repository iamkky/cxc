#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//#include "abjson/jsonvalue.h"
#include "cx_component.h"
#include "cx_parser.h"
#include "cx_parser.tokens.h"

static int vvflag = 1;

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

int processFragment(char *p, CxParserExtraDataType *extra)
{
CxParser		cx;
CxComponentList		list;
int			result;
int			count;

	if (vvflag) fprintf(stderr,"Fragment found %d: >%-20.20s<\n", extra->lnumber, p);
	cx = CxParserNew(p, extra);	

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
		p += (count = cx->cursor);
	}else{
		fprintf(stderr,"Failed to parse line %d\n", extra->lnumber);
	 	printLine(stderr, extra->line_start);
	}
	
	CxParserFree(cx);
		
	return count;
}

enum states {ST_SCANNING, ST_COMMENT_C, ST_COMMENT_CPP, ST_STRING};

int main(int argc, char **argv)
{
CxParserExtraDataType	extra;
char			*buffer, *p;
int			bytes;
int			state;

	buffer = readToBuffer(0, 32768, 1, &bytes);
	buffer[bytes] = 0;

	p = buffer;

	extra.lnumber = 1;
	extra.line_start = buffer;
	extra.source = NULL;

	state = ST_SCANNING;

	while(*p!=0){
		switch(state){
		case ST_SCANNING:
			if(*p=='{' && *(p+1)=='%'){
				p += 2;
				p += processFragment(p, &extra);

				if(*p=='%' && *(p+1)=='}'){
					p += 2;
				}else{
					fprintf(stderr,"Missing %} at %d\n", extra.lnumber);
			 		printLine(stderr, extra.line_start);
				}

				continue;
			}
/*
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
	
				CxParserFree(cx);
				
				if(*p!='%' || (*p=='%' && *(p+1)!='}')){
					fprintf(stderr,"Missing %} at %d\n", extra.lnumber);
				 	printLine(stderr, extra.line_start);
					exit(-1);
				}
		
				p += 2;
				continue;
			}
*/

			if(*p=='/' && *(p+1)=='*'){
				fputc(*p++, stdout);
				state = ST_COMMENT_C;
			}

			if(*p=='/' && *(p+1)=='/'){
				fputc(*p++, stdout);
				state = ST_COMMENT_CPP;
			}
			
			if(*p=='"'){
				state = ST_STRING;
			}

			break;

		case ST_COMMENT_C:
			if(*p=='*' && *(p+1)=='/'){
				fputc(*p++, stdout);
				state = ST_SCANNING;
			}
			break;

		case ST_COMMENT_CPP:
			if(*p=='\n'){
				fputc(*p++, stdout);
				state = ST_SCANNING;
			}
			break;

		case ST_STRING:
			if(*p=='\\' && *(p+1)=='"'){
				fputc(*p++, stdout);
			}
			if(*p=='"'){
				state = ST_SCANNING;
			}
			break;
		}

		if(*p=='\n') {
			extra.lnumber++;
			extra.line_start = p + 1;
		}

		fputc(*p++, stdout);
	}	
	free(buffer);
}

