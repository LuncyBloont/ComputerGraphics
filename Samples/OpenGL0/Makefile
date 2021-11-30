src = triangle.cpp 
out = triangle
opt = -g -lglew -lGL -lopengl32 -lglut

ALL : triangle
${out} : ${src}
	g++ ${src} /usr/local/lib/libgltools.a -o ${out} ${opt}
clean : 
	rm triangle.exe
run :
	startxwin ./triangle.exe
