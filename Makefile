default: rt

CFLAGS= -std=c++14 -c -I ../include/ -I ../include/external/    
OBJS=glad.o lodepng.o 3dtree.o aabb.o camera.o config.o intersection.o main.o math.o mesh.o model.o pathtracer.o ray.o renderer.o shader.o triangle.o window.o

.cpp.o: .cpp
	g++ ${CFLAGS} $<

glad.o: external/glad.c
	g++ ${CFLAGS} external/glad.c

lodepng.o: external/lodepng.o
	g++ ${CFLAGS} external/lodepng.cpp

rt: ${OBJS}
	g++ -o rt  ${OBJS} -lassimp -lGL -lglfw -lpthread -ldl

clean:
	rm *.o rt

