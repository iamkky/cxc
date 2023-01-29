syn region cxcCode start="{" end="}" extend contained
syn region cxcTag start="<" end=">" contains=cString,cxcCode extend contained
syn region cxcFragment start="{%" end="%}" contains=cString,cxcTag extend

hi def link cxcFragment Code
hi def link cxcCodei    Code
hi def link cxcTag      Special

