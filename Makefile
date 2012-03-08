CPPCOMPILER=g++
LIBS=`pkg-config --libs opencv`
UNITS=obj/Camera.o obj/CheckersBoard.o obj/main.o obj/Window.o \
	obj/CheckersServer/Server.o obj/CheckersServer/SocketUtils.o\
	obj/Arm.o obj/game/Game.o obj/game/Checkers.o obj/game/play.o\
	obj/game/tables.o

all: xbotAll

xbotAll: $(UNITS)
	$(CPPCOMPILER) $(UNITS) $(LIBS) -g -o xbot

obj/game/%.o : game/%.cpp
	@mkdir -p obj/game
	$(CPPCOMPILER) -g -c $< -o $@

obj/main.o: main.cpp
	@mkdir -p obj
	$(CPPCOMPILER) -g -c $< -o $@

obj/%.o: %.cpp %.h
	@mkdir -p obj/game
	$(CPPCOMPILER) -g -c $< -o $@


clean:
	rm -rf obj/*o xbot