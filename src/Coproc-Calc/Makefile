
CPP = g++
CC = gcc
LIBS = -lsystemc
LFLAGS = -Wno-deprecated -L$(SYSTEMC)lib-linux64
CFLAGS = -Wno-deprecated -I$(SYSTEMC)include -I .

#cfiles = $(wildcard *.c)
cppfiles = $(wildcard *.cpp)
ofiles = $(cppfiles:.cpp=.o)
#ofiles = $(cfiles:.c=.o) $(cppfiles:.cpp=.o)

TARGET = a.out

.PHONY : clean all

all : $(ofiles) $(TARGET)

$(TARGET) : $(ofiles)
	$(CPP) $(LFLAGS) $^ -o $@ $(LIBS)

#%.o : %.c %.h
#	$(CC) -c $(CFLAGS) $^

#%.o : .%c
#	$(CC) -c $(CFLAGS) $^

%.o : %.cpp %.h
	$(CPP) $(CFLAGS) -c $^

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $^


clean :
	rm *.o
	rm $(TARGET)
	rm out.txt
