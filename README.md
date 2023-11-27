CXC - A pseudo compiler for embedded HTML into C source code.
=============================================================

Generated code dependes on libhelium and libabd to compile

By convention any attribute starting with "on" and styled in camel case will be treated as and event name.

In backend mode any event should be attributed a string, just like any other attribute. Be it a hardcoded string, or any form of embbeded code.

In WASM mode if a string is assigned to an event attribute it will pass thru javascript as a javascript code. But in WASM mode a C event handler function can be assigned to a event attribute. In this case the final render will be a coding to be used by AWTk in order to call back this C function.
Note: by now passing thru is not working... but will.

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
		return {%
			<mkDiv(class)>
				<mkImg(src) />
			</mkDiv>
		%}
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
		return mkDiv(class, mkImg(src,NULL),NULL);
	}
