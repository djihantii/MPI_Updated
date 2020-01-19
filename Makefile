CC=mpicc
CFLAGS= -Wall -g
OBJECTS= utils.o
NBMach= 7
Initiateurs= 1

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

exec: prog
	mpirun -n $(NBMach) xterm -hold -e ./prog $(Initiateurs)
	#mpirun --xterm -1\! -n $(NBMach) ./prog $(Initiateurs)

.SECONDARY: $(OBJECTS)
	
clean:
	rm *.o prog

%.exe: $(OBJECTS)
	$(CC) -o $*.o -c $*.c $(CFLAGS)
	$(CC) -o prog $(CFLAGS) $(OBJECTS) $*.o
