src = off.cpp halfmesh.h modify.h
out = off.exe
opt = -lglew32 -lfreeglut -lopengl32 -lglu32
cc = g++ 
ALL : ${out}
${out} : ${src}
	${cc} -g ${src} ${opt} -o ${out}
