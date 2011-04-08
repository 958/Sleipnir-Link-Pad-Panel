@echo off
cl /nologo /MT /W3 /O2 /LD /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fe.\LinkPad.spx OleDragDrop.c LinkPad.c user32.lib gdi32.lib ole32.lib shell32.lib comctl32.lib Shlwapi.lib
del .\*.obj
del .\LinkPad.lib
del .\LinkPad.exp
