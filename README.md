CXC - A pseudo compiler for embedded HTML into C source code.
=============================================================

Generated code dependes on libhelium and libabd to compile

example:

	He mkImg(char *src)
	{
		return {% <img src={src}/> %};
	}

	He mkDiv(char *class, He content)
	{
		return {%
			<div class={class}>
				{content}
			</div>
		%};
	}

	He DivWithImage(char *class, char *src)
	{
		return mkDiv(class, mkImg(src));
	}

compiles to:

	He mkImg(char *src)
	{
		return
	heNEw("img",heAttrNew("src",src),NULL);
	}

	He mkDiv(char *class, He content)
	{
	return
	heNew("div",heAttrNew("class",class),
	content

	,NULL);
	}

	He DivWithImage(char *class, char *src)
	{
		return mkDiv(class, mkImg(src));
	}
