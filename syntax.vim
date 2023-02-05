syn region cxcDoubleCode start="{{" end="}}" contained contains=cString
syn region cxcCode start="{[^%{]" end="}" contained contains=cString
syn region cxcCodeElement start="\[" end="\]" contained
syn region cxcTagCode start="{[^{]" end="}" extend contained
syn region cxcTagDoubleCode start="{{" end="}}" contained contains=cString
syn region cxcTag start="<" end=">" contains=cString,cxcTagDoubleCode,cxcTagCode extend contained
syn region cxcFragment start="{%" end="%}" contains=cString,cxcTag,cxcDoubleCode,cxcCode,cxcCodeElement extend

hi def link cxcCode		Operator
hi def link cxcTagCode		Operator
hi def link cxcDoubleCode	Operator
hi def link cxcTagDoubleCode	Operator
hi def link cxcCodeElement	Type
hi def link cxcFragment		Code
hi def link cxcTag		Special
