syn region cxcCode start="{[^%{]" end="}" contained contains=cString
syn region cxcDoubleCode start="{{" end="}}" contained contains=cString
syn region cxcCodeElement start="\[" end="\]" contained
syn region cxcTag start="<" end=">" contained contains=cString,cxcTagDoubleCode,cxcTagCode 
syn region cxcTagCode start="{[^{]" end="}" contained
syn region cxcTagDoubleCode start="{{" end="}}" contained contains=cString
syn region cxcFragment start="{%" end="%}" contains=cString,cxcTag,cxcDoubleCode,cxcCode,cxcCodeElement 

syn cluster cParenGroup add=cxcCode
syn cluster cParenGroup add=cxcDoubleCode
syn cluster cParenGroup add=cxcCodeElement
syn cluster cParenGroup add=cxcTag
syn cluster cParenGroup add=cxcTagCode
syn cluster cParenGroup add=cxcTagDoubleCode

hi def link cxcCode		Operator
hi def link cxcDoubleCode	Operator
hi def link cxcCodeElement	Type
hi def link cxcTag		Special
hi def link cxcTagCode		Operator
hi def link cxcTagDoubleCode	Operator
hi def link cxcFragment		Identifier
