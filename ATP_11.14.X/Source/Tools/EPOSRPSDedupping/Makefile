BINDIR=../bin

EposFileGeneration: EposFileGeneration.o
	cc -o $(BINDIR)/EposFileGeneration EposFileGeneration.o 

EposFileGeneration.o: EposFileGeneration.c
	cc -g -q64 EposFileGeneration.c

clean:
	-@rm -f *.o $(BINDIR)/EposFileGeneration
