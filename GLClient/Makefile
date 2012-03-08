CPPCOMPILER=g++
#CPPCOMPILER=i586-mingw32msvc-g++
CCOMPILER=gcc
#CCOMPILER=i586-mingw32msvc-gcc
FLAGS=-Wall -g -O3
#LIBRARIES=-lGL -lglut -lGLU -lm -lpthread
LIBRARIES=-lGLU -lGL -lglut
MODULES=obj/GLObject.o obj/main.o obj/TextureManager.o obj/GLCube.o obj/Client.o \
	obj/SocketUtils.o 
SOIL_DEPENDS=soil/image_DXT.c  soil/image_helper.c  soil/SOIL.c  soil/stbi_DDS_aug_c.h  soil/stb_image_aug.c \
		soil/image_DXT.h  soil/image_helper.h  soil/SOIL.h  soil/stbi_DDS_aug.h  soil/stb_image_aug.h
SOIL_MODULES=obj/soil/image_DXT.o obj/soil/image_helper.o obj/soil/SOIL.o obj/soil/stb_image_aug.o

all: $(SOIL_MODULES) $(MODULES)
	$(CPPCOMPILER) $(SOIL_MODULES) $(MODULES) $(LIBRARIES) -o glcheckers

obj/soil/%.o : soil/%.c
	@mkdir -p obj/soil
	$(CCOMPILER) -c $< -o $@

obj/main.o : main.cpp
	@mkdir -p obj
	$(CPPCOMPILER) -c $(FLAGS) $< -o $@ 
obj/%.o : %.cpp %.h
	@mkdir -p obj
	$(CPPCOMPILER) -c $(FLAGS) $< -o $@ 
clean:
	rm -rf $(MODULES) $(SOIL_MODULES) fps
