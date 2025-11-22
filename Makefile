# 主程序
./KeyBonk.exe: ./src/obj/main.o ./resource/resources.o
	g++ ./src/obj/main.o ./resource/resources.o \
	-std=c++17 -o ./KeyBonk.exe \
	-mwindows -municode  \
	-luser32 -lgdi32 -lole32 -lgdiplus -lwinmm

# main.o
./src/obj/main.o: ./src/main.cpp
	g++ -o ./src/obj/main.o ./src/main.cpp -c -std=c++17

# 资源文件
./resource/resources.o: ./resource/resources.rc
	windres .\resource\resources.rc -F pe-x86-64 -o .\resource\resources.o