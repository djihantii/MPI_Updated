CC=mpicc
CFLAGS= -Wall -g
OBJECTS= procRecovering.o utils.o
NBMach= 7
Initiateur= 1 5

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o prog

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

exec: prog
	mpirun -n $(NBMach) --xterm -1\! ./prog $(Initiateur)
	
