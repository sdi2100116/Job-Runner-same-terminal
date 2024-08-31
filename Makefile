# compile with gcc
CC=gcc

# Compiler options:
#

CXXFLAGS = -g -pthread 


# set the name of the executable file to compile here
PROGRAM1 = jobCommander
PROGRAM2 = jobExecutorServer
PROGRAM3 = progDelay

F_BIN=bin
F_BUILD = build


OBJS1 = build/jobCommander.o
OBJS2 = build/jobExecutorServer.o  build/jobExecutorServerImplementation.o build/buffer.o
OBJS3 = build/progDelay.o

$(F_BUILD)/%.o: src/%.c
	$(CC) $(CXXFLAGS) -c $< -o $@

$(PROGRAM1): $(OBJS1)
	$(CC) $(CXXFLAGS) $(OBJS1) -o $(F_BIN)/$(PROGRAM1)

$(PROGRAM2): $(OBJS2)
	$(CC) $(CXXFLAGS) $(OBJS2)  -o $(F_BIN)/$(PROGRAM2)

$(PROGRAM3): $(OBJS3)
	$(CC) $(CXXFLAGS) $(OBJS3)  -o $(F_BIN)/$(PROGRAM3)

clean:
	rm -f $(OBJS1) $(OBJS2) 
	rm -f $(F_BIN)/$(PROGRAM1) $(F_BIN)/$(PROGRAM2) $(F_BIN)/$(PROGRAM3)
	rm -f $(F_BUILD)/*.o