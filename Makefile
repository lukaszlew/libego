OPT = -O3 -march=i686 -fomit-frame-pointer  -DNDEBUG 
#OPT += -fno-exceptions -fno-rtti
#OPT += -fstrict-aliasing
#OPT += -frename-registers

DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -ggdb3 -O3 -DNDEBUG -fno-inline 

CFLAGS += -Wall -Wextra -Wno-variadic-macros -Wno-long-long -Wno-pragmas

GPP     = g++ $(CFLAGS) 

all: deb

deb: playout_test_debug

opt: playout_test_opt

prof: playout_test_prof

board_test_debug: board.cpp utils.cpp
	$(GPP) $(DEBUG) -DBOARD_TEST -o board_test_debug board.cpp 

board_test_opt: board.cpp utils.cpp
	$(GPP) $(OPT)   -DBOARD_TEST -o board_test_opt   board.cpp 

playout_test_debug: board.cpp utils.cpp playout.cpp
	$(GPP) $(DEBUG) -DPLAYOUT_TEST -o playout_test_debug playout.cpp 

playout_test_opt: board.cpp utils.cpp playout.cpp
	$(GPP) $(OPT)   -DPLAYOUT_TEST -o playout_test_opt   playout.cpp 

playout_test_prof: board.cpp utils.cpp playout.cpp
	$(GPP) $(PROF)  -DPLAYOUT_TEST -o playout_test_prof   playout.cpp 



.SUFFIXES: .cpp .o

.cpp.o:
	$(GPP) -c $<

clean:
	rm -f *.s *.o .depend gmon.out core
	rm -f   board_test_debug   board_test_opt   board_test_prof
	rm -f playout_test_debug playout_test_opt playout_test_prof
	rm -f *~
