CPPCOMPILER=g++
LIBS=`pkg-config --libs opencv`
UNITS=obj/Camera.o obj/ChessBoard.o obj/Image.o obj/main.o obj/Transformations.o obj/Window.o

all: xbotAll

xbotAll: $(UNITS)
	$(CPPCOMPILER) $(UNITS) $(LIBS) -g -o xbot

obj/%.o: %.cpp %.h
	@mkdir -p obj
	$(CPPCOMPILER) -g -c $< -o $@

obj/main.o: main.cpp
	@mkdir -p obj
	$(CPPCOMPILER) -g -c $< -o $@

clean:
	rm -rf obj/*o xbot