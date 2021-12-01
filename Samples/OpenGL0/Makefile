src = triangle.cpp 
out = triangle
opt = -lGLEW32 -lGLU32 -lopengl32 -lglut

ALL : triangle
${out} : ${src}
	x86_64-w64-mingw32-g++ ${src} -o ${out} ${opt}
clean : 
	rm triangle.exe
run :
	./triangle.exe
