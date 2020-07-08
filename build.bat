@echo off
where /q cl
if ERRORLEVEL 1 (
    call  "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)


set application_name=win32_game.exe
set build_options= -DBUILD_WIN32=1
set compile_flags= -nologo /W0 /Zi /EHsc
set link_flags= gdi32.lib opengl32.lib user32.lib

if not exist build mkdir build
pushd build
start /b /wait "" "cl.exe"  %build_options% %compile_flags% ../src/win32_main.cpp /link %link_flags% /out:%application_name%
popd