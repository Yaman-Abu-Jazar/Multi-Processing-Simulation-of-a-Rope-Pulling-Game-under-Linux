LIBS = -lglut -lGLU -lGL -lm
G = gcc -g
O = -o
names = main player
OBJECTS = drawer

files:
	$(G) main.c $(O) main
	$(G) team1.c $(O) team1
	$(G) team2.c $(O) team2
	$(G) drawer.c $(O) drawer $(LIBS)

clean:
	rm -f $(names)