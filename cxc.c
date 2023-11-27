#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include "abjson/jsonvalue.h"
#include "cx_component.h"
#include "cx_parser.h"
#include "cx_parser.tokens.h"

enum states {ST_SCANNING, ST_COMMENT_C, ST_COMMENT_CPP, ST_STRING};

static int optVerbose = 0;
static int optPreserveSuffixes = 0;
static int optWasmMode = 0;

void usage(char *argv0)
{
	fprintf(stdout, "%s [options] <file>\n", argv0);
	fprintf(stdout, "\n");
	fprintf(stdout, "  -p:\tPreserve original suffixes\n");
	fprintf(stdout, "  -v:\tVerbose\n");
	fprintf(stdout, "  -h:\thelp\n");
}

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

FILE *openOutputFile(char *source, char *sext, char *dext, int check_source_ext, int preserveSuffixes)
{
FILE *fout;
char *output;
int  slen, sextlen, dextlen;

	if(source==NULL) return NULL;
	if(sext==NULL) return NULL;
	if(dext==NULL) return NULL;

	slen = strlen(source);
	sextlen = strlen(sext);
	if(slen<=sextlen) return NULL;
	
	if(check_source_ext && strcmp(sext, source+slen-sextlen)) {
		fprintf(stderr,"Input file %s has no %s extension\n", source, sext);
		return NULL;
	}

	dextlen = strlen(dext);

	if(preserveSuffixes){
		if((output = malloc(slen + dextlen + 1))==NULL) return NULL;
		strcpy(output, source);
		strcpy(output + slen, dext);	
	}else{
		if((output = malloc(slen - sextlen + dextlen + 1))==NULL) return NULL;
		strncpy(output, source, slen - sextlen);
		strcpy(output + slen - sextlen, dext);	
	}
	
	fout=fopen(output,"w");
	if(fout==NULL) fprintf(stderr,"Could not open output file: %s\n", output);

	free(output);
	
	return fout;
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

	fprintf(stderr,"Error: line %d: Expected: token: %d\n", self->extra->lnumber, token);
	fprintf(stderr,"%s:%d: In rule for %s: Unexpected %s [%d], expected: %s [%d]\n",
			self->extra->source,
			self->extra->lnumber,
			Rdpp_CxParserNonterminals_Names[nonterminal-10000],
			Rdpp_CxParserTerminals_Names[self->currToken-1000], 
			self->currToken,
			Rdpp_CxParserTerminals_Names[token-1000], 
			token
	);
}

int backtrackFail_handler(CxParser self, int nonterminal)
{
int size;

	fprintf(stderr,"Error: Backtracking failed:  %d (%s)\n", nonterminal, Rdpp_CxParserNonterminals_Names[nonterminal-10000]);
}

int processFragment(FILE *fpout, char *p, CxParserExtraDataType *extra)
{
CxParser		cx;
CxComponentList		list;
int			result;
int			cursor;

	if (optVerbose) fprintf(stderr,"Fragment found %d: >%-20.20s<\n", extra->lnumber, p);
	cx = CxParserNew(p, extra);	

	CxParserSetUnexpected(cx, unexpected_handler);
	CxParserSetBacktrackFail(cx, backtrackFail_handler);
	
	result = CxParserParse(cx);
	cursor = cx->cursor;
		
	if(optVerbose){
		fprintf(stdout,"cursor: %d\n", cursor);
		fprintf(stdout,"Last Token: %d\n", cx->currToken);
	}
			
	if(result>0){
		//fprintf(stdout,"\nResult:\n");
		//cxComponentListPrint(list, stdout, 0);
		//fprintf(stdout,"\nCode:\n");
		list = cx->value[0].complist;
		cxComponentListGenCode(list, fpout, 0);
	}else{
		fprintf(stderr,"Failed to parse line %d\n", extra->lnumber);
	 	printLine(stderr, extra->line_start);
	}
	
	CxParserFree(cx);
		
	return cursor;
}

int processFile(char *filename)
{
CxParserExtraDataType	extra;
FILE			*fpout;
char			*buffer, *p;
int			state;
int			size, fdin;

	// Read the source
	if((fdin = open(filename, O_RDONLY))<0){
		fprintf(stderr,"Could not open input file: %s\n", filename);
		return -1;
	}
	if((buffer = readToBuffer(fdin, 32768, 1, &size))==NULL){
		fprintf(stderr,"Could not alloc memory for source buffer\n");
		return -1;
	};
	buffer[size]=0;
	close(fdin);
	
//	buffer = readToBuffer(0, 32768, 1, &bytes);
//	buffer[bytes] = 0;

	if((fpout=openOutputFile(filename, ".cx", ".c", 1, optPreserveSuffixes))==NULL) return -1;

	if(optVerbose){
		fprintf(stdout,"Input Size: %d\n", size);
	}

	p = buffer;

	extra.lnumber = 1;
	extra.line_start = buffer;
	extra.source = filename;
	extra.wasmMode = optWasmMode;

	state = ST_SCANNING;

	fprintf(fpout,"#line 1 \"%s\"\n", filename);

	while(*p!=0){
		switch(state){
		case ST_SCANNING:
			if(*p=='{' && *(p+1)=='%'){
				p += 2;

				fprintf(fpout,"\n#line %d \"%s\"\n", extra.lnumber, filename);

				p += processFragment(fpout, p, &extra);

				fprintf(fpout,"\n#line %d \"%s\"\n", extra.lnumber, filename);

				if(*p=='%' && *(p+1)=='}'){
					p += 2;
				}else{
					fprintf(stderr,"Missing %} at %d\n", extra.lnumber);
			 		printLine(stderr, extra.line_start);
				}

				continue;
			}

			if(*p=='/' && *(p+1)=='*'){
				fputc(*p++, fpout);
				state = ST_COMMENT_C;
			}

			if(*p=='/' && *(p+1)=='/'){
				fputc(*p++, fpout);
				state = ST_COMMENT_CPP;
			}
			
			if(*p=='"'){
				state = ST_STRING;
			}

			break;

		case ST_COMMENT_C:
			if(*p=='*' && *(p+1)=='/'){
				fputc(*p++, fpout);
				state = ST_SCANNING;
			}
			break;

		case ST_COMMENT_CPP:
			if(*p=='\n'){
				fputc(*p++, fpout);
				state = ST_SCANNING;
			}
			break;

		case ST_STRING:
			if(*p=='\\' && *(p+1)=='"'){
				fputc(*p++, fpout);
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

		fputc(*p++, fpout);
	}	
	free(buffer);

	return 0;
}

int main(int argc, char **argv)
{
int	argcount;


	if(argc<2){
		usage(argv[0]);
		exit(-1);
	}
	argcount=1;

	while(argcount<argc){
		if(!strcmp(argv[argcount],"-v")){
			argcount++;
			optVerbose = 1;
			continue;
		}
		if(!strcmp(argv[argcount],"-p")){
			argcount++;
			optPreserveSuffixes = 1;
			continue;
		}
		if(!strcmp(argv[argcount],"-w")){
			argcount++;
			optWasmMode = 1;
			continue;
		}
		if(!strcmp(argv[argcount],"-h")){
			argcount++;
			usage(argv[0]);
			exit(0);
		}
		if(!processFile(argv[argcount])){
			if (optVerbose) fprintf(stderr,"Process of %s file finished!\n", argv[argcount]);
			argcount++;
			continue;
		}else{
			fprintf(stderr,"Process of %s file has failed!\n", argv[argcount]);
			exit(-1);
		}
	}
}
