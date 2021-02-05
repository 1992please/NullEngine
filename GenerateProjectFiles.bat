@echo off
call Engine\Programs\Premake\premake5.exe vs2017

@REM really weird error check that I don't udnerstand
set foundErr=1
if errorlevel 0 if not errorlevel 1 set "foundErr="
if defined foundErr pause