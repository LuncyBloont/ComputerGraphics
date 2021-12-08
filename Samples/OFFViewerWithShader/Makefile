src = off.cpp halfmesh.h modify.h render.h
out = off.exe
opt = -lGLEW32 -lfreeglut -lopengl32 -lGLU32
cc = x86_64-w64-mingw32-g++ 
ALL : ${out}
${out} : ${src}
	${cc} -g ${src} ${opt} -o ${out}
