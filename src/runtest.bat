@echo off
call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d E:\code\github\MarkdownEditor\src\tests
cl /nologo /EHsc /W3 /utf-8 /D_UNICODE /DUNICODE MdAnchorsTest.cpp ..\MdAnchors.cpp /Fe:MdAnchorsTest.exe
if errorlevel 1 exit /b 1
MdAnchorsTest.exe
