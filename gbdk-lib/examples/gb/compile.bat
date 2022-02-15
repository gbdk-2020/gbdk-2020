@echo off

REM Automatically build all GB examples in this directory.

for /d %%i in (.\*) do (
	pushd "%%i"
	.\compile.bat
	popd
)
