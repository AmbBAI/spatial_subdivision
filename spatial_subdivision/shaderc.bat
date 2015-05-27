set shadercExe="../build/win32_vs2013/bin/shadercRelease.exe"
set includePath="../bgfx/src/"
set outDir="../runtime/shaders/glsl/"

%shadercExe% -f vs_default.sc -i %includePath% -o %outDir%vs_default.bin --platform linux --type v --varyingdef varying.def.sc
%shadercExe% -f fs_default.sc -i %includePath% -o %outDir%fs_default.bin --platform linux --type f --varyingdef varying.def.sc

%shadercExe% -f vs_line.sc -i %includePath% -o %outDir%vs_line.bin --platform linux --type v --varyingdef varying.def.sc
%shadercExe% -f fs_line.sc -i %includePath% -o %outDir%fs_line.bin --platform linux --type f --varyingdef varying.def.sc
