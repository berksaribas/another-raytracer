call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set CommonCompilerFlags= -diagnostics:column -WL -O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999 -MD -EHsc /openmp
set CommonLinkerFlags= -incremental:no -opt:ref gdi32.lib kernel32.lib user32.lib shell32.lib OpenGL32.lib

pushd .\build
cl %CommonCompilerFlags% ..\src\*.cpp ..\src\glad\glad.c /I..\include /Fetracer.exe /link %CommonLinkerFlags%
popd

