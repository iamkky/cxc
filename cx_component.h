#ifndef _CXCOMPONENT_H_
#define _CXCOMPONENT_H_

enum CxObjects		{CXCOMPONENT=10, CXCOMPONENTLIST, CXATTRIBUTE, CXATTRIBUTELIST, CXCOMPONENT_FUNCTIONCALL};
enum CxAttrExprTypes	{
			 CX_ATTR_EXPR_TYPE_CODERAW=1, CX_ATTR_EXPR_TYPE_STRING, CX_ATTR_EXPR_TYPE_CODETEXTF, CX_ATTR_EXPR_TYPE_NULL,
			 CX_COMPONENT_FUNCTIONCALL, CX_COMPONENT_TAG, CX_COMPONENT_CODE_ELEMENT, CX_COMPONENT_CODE_RAW, CX_COMPONENT_TEXTF, CX_COMPONENT_TEXTRAW,
			 CX_ATTR_EVENT
			};

typedef struct CxAttribute_struct *CxAttribute;
typedef struct CxAttributeList_struct *CxAttributeList;
typedef struct CxComponent_struct *CxComponent;
typedef struct CxComponentList_struct *CxComponentList;

struct CxAttribute_struct{
	int		type;
	char		*name;
	char		*value;
	char		*source_file;
	int		source_line;
};

struct CxAttributeList_struct{
	CxAttribute	attribute;
	CxAttributeList	next;
};

struct CxComponent_struct{
	int		type;
	char		*face;
	CxAttributeList	attrlist;
	CxComponentList	child;
	char		*code;
	char		*source_file;
	int		source_line;
};

struct CxComponentList_struct{
	CxComponent	component;
	CxComponentList	next;
};

CxAttribute cxAttributeNew(char *name, int type, char *value);
void cxAttributeFree(CxAttribute self);
void cxAttributePrint(CxAttribute self, FILE *fp, int indent);
void cxAttributeSetSourceFile(CxAttribute self, char *file);
void cxAttributeSetSourceLine(CxAttribute self, int line);

CxAttributeList cxAttributeListNew();
void cxAttributeListFree(CxAttributeList self);
void cxAttributeListPrint(CxAttributeList self, FILE *fp, int indent);
void cxAttributeListSetAttribute(CxAttributeList self, CxAttribute attribute);

CxComponent cxComponentNew(int type, char *face, char *coce, CxComponentList child);
void cxComponentFree(CxComponent self);
void cxComponentPrint(CxComponent self, FILE *fp, int indent);
void cxComponentSetSourceFile(CxComponent self, char *file);
void cxComponentSetSourceLine(CxComponent self, int line);

CxComponentList cxComponentListNew();
void cxComponentListFree(CxComponentList self);
void cxComponentListPrint(CxComponentList self, FILE *fp, int indent);
void cxComponentListSetComponent(CxComponentList self, CxComponent component);

void cxAttributeGenCode(CxAttribute self, FILE *fp, int indent);
void cxAttributeListGenCode(CxAttributeList self, FILE *fp, int indent);
void cxComponentGenCode(CxComponent self, FILE *fp, int indent);
void cxComponentListGenCode(CxComponentList self, FILE *fp, int indent);

#endif

