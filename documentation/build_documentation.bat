@echo off

:: Ensure local scope
setlocal

:: Ensure current dir
pushd %~dp0

:: CD to root
cd ..

doxygen %~dp0Doxyfile

cd documentation
python postprocess_doc.py

