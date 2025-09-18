BINDIR=../bin
LIBDIR=../lib
COREDIR=../../Libraries/core_aix
COREINC=$(COREDIR)/../include
CORELIB=$(COREDIR)/lib
APPINC=../../Server_AIX/include
APPLIB=../../Server_AIX/lib
CFLAGS64= -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include 
CFLAGS= -g -w -I$(COREINC) -I../include \
         -I$(XIPCROOT)/include
LFLAGS=-q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
JAVACLASSNAME=FAIrcvHandler
JNISHAREDLIB=libFAIrcvHandler.so
SYSTEMLIBFOLDER=/usr/lib
#SYSTEM_JAVA_INCLUDE_FOLDER=/usr/java_11/jdk-11.0.5+10/include
SYSTEM_JAVA_INCLUDE_FOLDER=/usr/java8_64/include
C_COMPILER=cc
JAVA_COMPILER=javac
JAVALIBDIR =../../Libraries/core_aix/lib

faircv.jar: $(JNISHAREDLIB)
	cd faiconnector && mvn package -Dmaven.test.skip=true && cp ./target/faiconnector-1.0.0-SNAPSHOT.jar ../faircv.jar && cd ..
	cp ./faircv $(BINDIR)/
	cp ./faircv.jar $(BINDIR)/

$(JNISHAREDLIB): java_daemon.o $(JAVACLASSNAME).class $(JAVACLASSNAME).o 
	$(C_COMPILER) -q64 -qarch=auto -qmkshrobj -o ./$(JNISHAREDLIB) $(JAVACLASSNAME).o  -lpteipc -lptecom -lequit -lpte -lxcom -lm -lpthread -lnsl -L$(APPLIB) -L$(CORELIB)
	
java_daemon.o: java_daemon.c 
	$(C_COMPILER) $(CFLAGS64) java_daemon.c -o faircv

$(JAVACLASSNAME).o : $(JAVACLASSNAME).c $(JAVACLASSNAME).h 
	$(C_COMPILER) -g -q64 -fPIC -I $(SYSTEM_JAVA_INCLUDE_FOLDER)/ -I $(SYSTEM_JAVA_INCLUDE_FOLDER)/aix -I $(COREINC) -I $(APPINC) -I $(XIPCROOT)/include -c $(JAVACLASSNAME).c -o $(JAVACLASSNAME).o 

$(JAVACLASSNAME).class : $(JAVACLASSNAME).java
	$(JAVA_COMPILER) $(JAVACLASSNAME).java -h .

clean :
	-@rm -f *.o *.so *.class $(JAVACLASSNAME).h $(JAVALIBDIR)/java_daemon.o ./faircv ./faircv.jar
	-@rm -f faiconnector/target/faiconnector-1.0.0.jar

