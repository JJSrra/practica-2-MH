# Nombre del fichero ejecutable
TARGET = p2

# Definición del compilador
CC = g++ -std=c++11 -O2

# Orden que será ejecutada por defecto
all: bin/$(TARGET)

bin/$(TARGET): $(TARGET).o obj/utilidadesBD.o obj/algoritmos.o obj/geneticos.o obj/memeticos.o obj/evolutivos.o
	$(CC) -o bin/$(TARGET) obj/$(TARGET).o obj/utilidadesBD.o obj/algoritmos.o obj/geneticos.o obj/memeticos.o obj/evolutivos.o

$(TARGET).o: src/$(TARGET).cpp include/utilidadesBD.h
	$(CC) -c -I./include -o obj/$(TARGET).o src/$(TARGET).cpp

obj/utilidadesBD.o: src/utilidadesBD.cpp include/utilidadesBD.h
	$(CC) -c -I./include -o obj/utilidadesBD.o src/utilidadesBD.cpp

obj/algoritmos.o: src/algoritmos.cpp include/algoritmos.h
	$(CC) -c -I./include -o obj/algoritmos.o src/algoritmos.cpp

obj/geneticos.o: src/geneticos.cpp include/geneticos.h
	$(CC) -c -I./include -o obj/geneticos.o src/geneticos.cpp

obj/memeticos.o: src/memeticos.cpp include/memeticos.h
	$(CC) -c -I./include -o obj/memeticos.o src/memeticos.cpp

obj/evolutivos.o: src/evolutivos.cpp include/evolutivos.h
	$(CC) -c -I./include -o obj/evolutivos.o src/evolutivos.cpp

clean:
	rm -f *.o
	rm -f $(TARGET)
