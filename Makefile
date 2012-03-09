CPPCOMPILER=g++
FLAGS=-p -g
LIBS=`pkg-config --libs opencv`
UNITS=obj/Camera.o obj/CheckersBoard.o obj/main.o obj/Window.o \
	obj/CheckersServer/Server.o obj/CheckersServer/SocketUtils.o\
	obj/Arm.o obj/game/Game.o obj/game/Checkers.o obj/game/play.o\
	obj/game/tables.o

all: xbotAll

xbotAll: $(UNITS)
	$(CPPCOMPILER) $(FLAGS) $(UNITS) $(LIBS) -o xbot

obj/game/%.o : game/%.cpp
	@mkdir -p obj/game
	$(CPPCOMPILER) $(FLAGS)  -c $< -o $@

obj/main.o: main.cpp
	@mkdir -p obj
	$(CPPCOMPILER) $(FLAGS)  -c $< -o $@

obj/%.o: %.cpp %.h
	@mkdir -p obj/game
	$(CPPCOMPILER) $(FLAGS)  -c $< -o $@


clean:
	rm -rf $(UNITS) xbot