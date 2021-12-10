src = off.cpp load.h render.h bmp.h
out = off.exe
opt = -lglew32 -lfreeglut -lopengl32
cc = x86_64-w64-mingw32-g++ 
ALL : ${out}
${out} : ${src}
	${cc} -g ${src} ${opt} -o ${out}
