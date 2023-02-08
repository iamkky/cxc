#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cx_component.h"

static void printIndent(FILE *fp, int indent, char *tab)
{
	while(indent--){
		fprintf(fp, "%s", tab);
	}
}

CxAttribute cxAttributeNew(char *name, int type, char *value)
{
CxAttribute self;

	if(name==NULL) return NULL;

	if((self=malloc(sizeof(*(CxAttribute)0)))==NULL) return NULL;

	self->name  = strdup(name);
	self->type  = type;
	self->value = value ? strdup(value) : NULL;

	return self;
}

void cxAttributeFree(CxAttribute self)
{
	if(self==NULL) return;
	if(self->name) free(self->name);
	if(self->value) free(self->value);
	free(self);
}

void cxAttributePrint(CxAttribute self, FILE *fp, int indent)
{
	if(self==NULL) return;
	printIndent(fp, indent, " ");
	fprintf(fp, "Attr: %p %s %s\n", self, self->name, self->value);
}

CxAttributeList cxAttributeListNew()
{
CxAttributeList self;

	if((self=malloc(sizeof(*(CxAttributeList)0)))==NULL) return NULL;

	self->attribute	= NULL;
	self->next	= NULL;

	return self;
}

void cxAttributeListSetAttribute(CxAttributeList self, CxAttribute attribute)
{
	if(self==NULL) return;
	self->attribute = attribute;
}

void cxAttributeListFree(CxAttributeList self)
{
	if(self==NULL) return;
	if(self->attribute) cxAttributeFree(self->attribute);
	if(self->next) cxAttributeListFree(self->next);
	free(self);
}

void cxAttributeListPrint(CxAttributeList self, FILE *fp, int indent)
{
	if(self==NULL) return;
	cxAttributePrint(self->attribute, fp, indent + 1);
	if(self->next) cxAttributeListPrint(self->next, fp, indent);
}


CxComponent cxComponentNew(char *face, char *code, CxComponentList child)
{
CxComponent self;

	if(face==NULL) return NULL;

	if((self=malloc(sizeof(*(CxComponent)0)))==NULL) return NULL;

	self->face  = strdup(face);
	self->child = child;
	self->attrlist = NULL;
	self->code = code ? strdup(code) : NULL;

	return self;
}

void cxComponentFree(CxComponent self)
{
	if(self==NULL) return;
	if(self->face) free(self->face);
	if(self->child) cxComponentListFree(self->child);
	free(self);
}

void cxComponentPrint(CxComponent self, FILE *fp, int indent)
{
	if(self==NULL){
		printIndent(fp, indent, " ");
		fprintf(fp, "Comp: %p\n", self);
		return;
	}

	printIndent(fp, indent, " ");
	fprintf(fp, "Comp: %p %s\n", self, self->face);
	cxAttributeListPrint(self->attrlist, fp, indent);
	if(self->child) cxComponentListPrint(self->child, fp, indent + 1);
}

CxComponentList cxComponentListNew()
{
CxComponentList self;

	if((self=malloc(sizeof(*(CxComponentList)0)))==NULL) return NULL;

	self->component	= NULL;
	self->next	= NULL;

	return self;
}

void cxComponentListSetComponent(CxComponentList self, CxComponent component)
{
	if(self==NULL) return;
	self->component = component;
}

void cxComponentListFree(CxComponentList self)
{
	if(self==NULL) return;
	if(self->component) cxComponentFree(self->component);
	if(self->next) cxComponentListFree(self->next);
	free(self);
}

void cxComponentListPrint(CxComponentList self, FILE *fp, int indent)
{
	if(self==NULL) return;

	cxComponentPrint(self->component, fp, indent);
	if(self->next)  cxComponentListPrint(self->next, fp, indent);
}

// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------

void cxComponentGenCodeTextRaw(FILE *fp, char *text)
{
int ch;

	fputc('"', fp);

	while(ch=*text){
		if(ch=='\n'){
			fputs("\\n", fp);
		}else{
			fputc(ch, fp);
		}
		text++;
	}

	fputc('"', fp);
	
}

void cxAttributeGenCode(CxAttribute self, FILE *fp, int indent)
{
	if(self==NULL) return;
	//printIndent(fp, indent, " ");

	if(self->type == CX_ATTR_EXPR_TYPE_STRING){
		fprintf(fp, ",heAttrNew(\"%s\",%s)", self->name, self->value);
	}else if(self->type == CX_ATTR_EXPR_TYPE_CODERAW){
		fprintf(fp, ",heAttrNew(\"%s\",%s)", self->name, self->value);
	}else if(self->type == CX_ATTR_EXPR_TYPE_CODETEXTF){
		fprintf(fp, ",heAttrNewf(\"%s\",%s)", self->name, self->value);
	}else if(self->type == CX_ATTR_EXPR_TYPE_NULL){
		fprintf(fp, ",heAttrNew(\"%s\",%s)", self->name, self->value);
	}
}

void cxAttributeListGenCode(CxAttributeList self, FILE *fp, int indent)
{
	if(self==NULL) return;
	cxAttributeGenCode(self->attribute, fp, indent + 1);
	if(self->next) {
		cxAttributeListGenCode(self->next, fp, indent);
	}
}

void cxComponentGenCode(CxComponent self, FILE *fp, int indent)
{
	if(self==NULL){
		//printIndent(fp, indent, " ");
		///fprintf(fp, "Comp: %p\n", self);
		return;
	}

	fprintf(fp, "\n");
	printIndent(fp, indent, " ");
	//
	// A CODE	is expected to be a expression resolving to a char *
	// A CODELEMENT is expected to be a expression resolving to a He
	// A TEXTF	is expected to be a parameters list that can be supplied to a strf call
	//
	if(!strcmp(self->face,"CODERAW")){
		fprintf(fp, "heText(%s)\n", self->code);
	}else if(!strcmp(self->face,"CODEELEMENT")){
		fprintf(fp, " %s \n", self->code);
	}else if(!strcmp(self->face,"TEXTF")){
		fprintf(fp, "heTextf(%s)\n", self->code);
	}else if(!strcmp(self->face,"TEXTRAW")){
		fprintf(fp, "heText(\n");
		cxComponentGenCodeTextRaw(fp, self->code);
		fprintf(fp, ")\n");
	}else if(!strcmp(self->face,"FUNCTIONCALL")){
		fprintf(fp, "%s", self->code);
		cxAttributeListGenCode(self->attrlist, fp, indent);
		if(self->child) {
			fprintf(fp, ",");
			cxComponentListGenCode(self->child, fp, indent + 1);
			fprintf(fp, "\n");
			printIndent(fp, indent, " ");
			fprintf(fp, ",NULL)");
		}else{
			fprintf(fp, ",NULL)");
		}
	}else{
		fprintf(fp, "heNew(\"%s\"", self->face);
		cxAttributeListGenCode(self->attrlist, fp, indent);
		if(self->child) {
			fprintf(fp, ",");
			cxComponentListGenCode(self->child, fp, indent + 1);
			fprintf(fp, "\n");
			printIndent(fp, indent, " ");
			fprintf(fp, ",NULL)");
		}else{
			fprintf(fp, ",NULL)");
		}
	}
}

void cxComponentListGenCode(CxComponentList self, FILE *fp, int indent)
{
	if(self==NULL) return;

	cxComponentGenCode(self->component, fp, indent);
	if(self->next) {
		fprintf(fp, ",");
		cxComponentListGenCode(self->next, fp, indent);
	}
}

