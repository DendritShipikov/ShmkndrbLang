call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
cl /c /D DEBUG /EHsc /W4 /O2 /sdl /permissive- /std:c++17 /Fo"./build/" "./src/main.cpp"
link /out:"./build/prog.exe" "./build/main.obj"