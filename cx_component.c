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

void cxAttributeSetSourceFile(CxAttribute self, char *file)
{
	if(self==NULL) return;
	self->source_file = file;
}

void cxAttributeSetSourceLine(CxAttribute self, int line)
{
	if(self==NULL) return;
	self->source_line = line;
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


CxComponent cxComponentNew(int type, char *face, char *code, CxComponentList child)
{
CxComponent self;

	if(type==CX_COMPONENT_TAG && face==NULL) return NULL;
	if(type!=CX_COMPONENT_TAG && code==NULL) return NULL;

	if((self=malloc(sizeof(*(CxComponent)0)))==NULL) return NULL;

	self->type  = type;
	self->face  = face ? strdup(face) : NULL;
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

void cxComponentSetSourceFile(CxComponent self, char *file)
{
	if(self==NULL) return;
	self->source_file = file;
}

void cxComponentSetSourceLine(CxComponent self, int line)
{
	if(self==NULL) return;
	self->source_line = line;
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

	fprintf(fp, "#line %d\n", self->source_line);

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
	fprintf(fp, "\n");
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

	fprintf(fp, "#line %d\n", self->source_line);
	//fprintf(fp, "\n");
	//printIndent(fp, indent, " ");
	//
	// A CODERAW	is expected to be a expression resolving to a char *
	// A CODELEMENT is expected to be a expression resolving to a He
	// A TEXTF	is expected to be a parameters list that can be supplied to a strf call
	// A TEXT	is expected to be a expression resolving to a char *
	//
	if(self->type == CX_COMPONENT_CODE_RAW){
		fprintf(fp, "heText(%s)", self->code);
	}else if(self->type == CX_COMPONENT_CODE_ELEMENT){
		fprintf(fp, " %s ", self->code);
	}else if(self->type == CX_COMPONENT_TEXTF){
		fprintf(fp, "heTextf(%s)", self->code);
	}else if(self->type == CX_COMPONENT_TEXTRAW){
		fprintf(fp, "heText(\n");
		cxComponentGenCodeTextRaw(fp, self->code);
		fprintf(fp, ")");
	}else if(self->type == CX_COMPONENT_FUNCTIONCALL){
		fprintf(fp, "%s\n", self->code);
		cxAttributeListGenCode(self->attrlist, fp, indent);
		if(self->child) {
			fprintf(fp, ",\n");
			cxComponentListGenCode(self->child, fp, indent + 1);
			fprintf(fp, ",NULL)");
		}else{
			fprintf(fp, ",NULL)");
		}
	}else{
		fprintf(fp, "heNew(\"%s\"\n", self->face);
		cxAttributeListGenCode(self->attrlist, fp, indent);
		if(self->child) {
			fprintf(fp, ",\n");
			cxComponentListGenCode(self->child, fp, indent + 1);
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
	fprintf(fp, "\n");
	if(self->next) {
		fprintf(fp, ",\n");
		cxComponentListGenCode(self->next, fp, indent);
	}
}

