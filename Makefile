OPT = -O3 -march=i686 -fomit-frame-pointer  -DNDEBUG 
#OPT += -fno-exceptions -fno-rtti
#OPT += -fstrict-aliasing
#OPT += -frename-registers

DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -ggdb3 -O3 -DNDEBUG -fno-inline 

CFLAGS = -Wall -Wextra -Wno-variadic-macros -Wno-long-long

GPP     = g++ $(CFLAGS) 

all: clean debug

debug:
	$(GPP) $(DEBUG) -o debug   main.cpp 

opt:
	$(GPP) $(OPT) -o opt   main.cpp 

.SUFFIXES: .cpp .o

.cpp.o:
	$(GPP) -c $<

clean:
	rm -f *.s *.o .depend gmon.out debug core opt
	rm -f *~
