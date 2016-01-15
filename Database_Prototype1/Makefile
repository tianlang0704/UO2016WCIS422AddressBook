PP	:= g++
CC	:= gcc
CFLAGS	:= -g
OFLAGS	:= -g -std=c++0x
LIBS	:= -pthread -ldl
SRCS	:= $(wildcard *.cpp)
OBJS	:= $(SRCS:.cpp=.o) sqlite/sqlite3.o
EXEC	:= Database_P1
DATA	:= test.db


$(EXEC): $(OBJS)
	$(PP) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)
	rm -f *.o sqlite/*.o

sqlite/sqlite3.o: sqlite/sqlite3.c
	$(CC) $(OFLAGS) -o $@ -c $<

%.o: %.cpp
	$(PP) $(OFLAGS) -o $@ -c $<

.Phony: clean

clean:
	rm -f $(OBJS) $(EXEC) $(DATA)
