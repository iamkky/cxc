#ifndef _CXCOMPONENT_H_
#define _CXCOMPONENT_H_

enum CxObjects		{CXCOMPONENT=10, CXCOMPONENTLIST, CXATTRIBUTE, CXATTRIBUTELIST};
enum CxAttrExprTypes	{CX_ATTR_EXPR_TYPE_CODERAW=1, CX_ATTR_EXPR_TYPE_STRING, CX_ATTR_EXPR_TYPE_CODETEXTF, CX_ATTR_EXPR_TYPE_NULL};

typedef struct CxAttribute_struct *CxAttribute;
typedef struct CxAttributeList_struct *CxAttributeList;
typedef struct CxComponent_struct *CxComponent;
typedef struct CxComponentList_struct *CxComponentList;

struct CxAttribute_struct{
	int		magic;
	int		type;
	char		*name;
	char		*value;
};

struct CxAttributeList_struct{
	int		magic;
	CxAttribute	attribute;
	CxAttributeList	next;
};

struct CxComponent_struct{
	int		magic;
	char		*face;
	CxAttributeList	attrlist;
	CxComponentList	child;
	char		*code;
};

struct CxComponentList_struct{
	int		magic;
	CxComponent	component;
	CxComponentList	next;
};

CxAttribute cxAttributeNew(char *name, int type, char *value);
void cxAttributeFree(CxAttribute self);
void cxAttributePrint(CxAttribute self, FILE *fp, int indent);

CxAttributeList cxAttributeListNew();
void cxAttributeListFree(CxAttributeList self);
void cxAttributeListPrint(CxAttributeList self, FILE *fp, int indent);
void cxAttributeListSetAttribute(CxAttributeList self, CxAttribute attribute);

CxComponent cxComponentNew(char *face, char *coce, CxComponentList child);
void cxComponentFree(CxComponent self);
void cxComponentPrint(CxComponent self, FILE *fp, int indent);

CxComponentList cxComponentListNew();
void cxComponentListFree(CxComponentList self);
void cxComponentListPrint(CxComponentList self, FILE *fp, int indent);
void cxComponentListSetComponent(CxComponentList self, CxComponent component);

void cxAttributeGenCode(CxAttribute self, FILE *fp, int indent);
void cxAttributeListGenCode(CxAttributeList self, FILE *fp, int indent);
void cxComponentGenCode(CxComponent self, FILE *fp, int indent);
void cxComponentListGenCode(CxComponentList self, FILE *fp, int indent);

#endif

