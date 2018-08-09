OBJ=obj/
LIB=lib/
REL=release/

CC=i686-w64-mingw32-gcc-win32
#CC=gcc
CFLAGS=-O3 -std=c99

ntkcalc: $(OBJ)ntkcalc.o $(OBJ)leio.o
	$(CC) -o $(REL)ntkcalc.exe $(OBJ)ntkcalc.o $(OBJ)leio.o
	zip $(REL)checksum.zip $(REL)ntkcalc.exe

$(OBJ)ntkcalc.o: ntkcalc.c
	$(CC) -c ntkcalc.c -o $(OBJ)ntkcalc.o $(CFLAGS)	

checksum: $(OBJ)checksum.o $(OBJ)leio.o
	$(CC) -o $(REL)checksum.exe $(OBJ)checksum.o $(OBJ)leio.o
	zip $(REL)checksum.zip $(REL)checksum.exe
		
$(OBJ)checksum.o: checksum.c
	$(CC) -c checksum.c -o $(OBJ)checksum.o $(CFLAGS)
		
baseval: $(OBJ)baseval.o $(OBJ)leio.o
	$(CC) -o $(REL)baseval.exe $(OBJ)baseval.o $(OBJ)leio.o
	zip $(REL)checksum.zip $(REL)baseval.exe
	
$(OBJ)baseval.o: baseval.c
	$(CC) -c baseval.c -o $(OBJ)baseval.o $(CFLAGS)

fwinfo: $(OBJ)fwinfo.o $(OBJ)leio.o
	$(CC) -o $(REL)fwinfo.exe $(OBJ)fwinfo.o $(OBJ)leio.o
	
$(OBJ)fwinfo.o: fwinfo.c
	$(CC) -c fwinfo.c -o $(OBJ)fwinfo.o $(CFLAGS)

$(OBJ)leio.o: $(LIB)leio.c
	$(CC) -c $(LIB)leio.c -o $(OBJ)leio.o $(CFLAGS)
	
	
ntkfwinfo: $(OBJ)ntkfwinfo.o
	$(CC) -o $(REL)ntkfwinfo.exe $(OBJ)ntkfwinfo.o
	
$(OBJ)ntkfwinfo.o: ntkfwinfo.c
	$(CC) -c ntkfwinfo.c -o $(OBJ)ntkfwinfo.o $(CFLAGS)

	
clean:
	rm $(REL)*
	rm $(OBJ)*
	
objclean:
	rm $(OBJ)*
