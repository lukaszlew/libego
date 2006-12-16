OPT = -O3 -march=i686 -fomit-frame-pointer  -DNDEBUG 
#OPT += -fno-exceptions -fno-rtti
#OPT += -fstrict-aliasing
#OPT += -frename-registers

DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -ggdb3 -O3 -DNDEBUG -fno-inline 

CFLAGS = -Wall -Wextra -Wno-variadic-macros -Wno-long-long

GPP     = g++ $(CFLAGS) 

all: deb

deb: board_test_debug

opt: board_test_opt

board_test_debug: board.cpp
	$(GPP) $(DEBUG) -DBOARD_TEST -o board_test_debug board.cpp 

board_test_opt: board.cpp
	$(GPP) $(OPT)   -DBOARD_TEST -o board_test_opt   board.cpp 

.SUFFIXES: .cpp .o

.cpp.o:
	$(GPP) -c $<

clean:
	rm -f *.s *.o .depend gmon.out board_test_debug board_test_opt
	rm -f *~
