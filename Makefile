OPT   = -O3 -march=i686 -fomit-frame-pointer -DDEBUG -ffast-math -frename-registers
DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -O2 -march=i686 -DDEBUG -ggdb3 -fno-inline 

CFLAGS += -Wall #-static #-Wno-long-long -Wextra -Wno-variadic-macros

GPP    = g++ $(CFLAGS) 

FILES  = Makefile config.cpp utils.cpp basic_go_types.cpp board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp uct.cpp main.cpp gtp_board.cpp gtp_genmove.cpp

all: ego_opt


ego_debug: $(FILES)
	$(GPP) $(DEBUG) -o ego_debug main.cpp

ego_opt:   $(FILES)
	$(GPP) $(OPT)   -o ego_opt   main.cpp

ego_asm:   $(FILES)
	$(GPP) $(OPT)   -S -c        main.cpp

ego_prof:  $(FILES)
	$(GPP) $(PROF)  -o ego_prof  main.cpp



.SUFFIXES: .cpp .o

.cpp.o:
	$(GPP) -c $<

clean:
	rm -f *.s *.o .depend gmon.out core
	rm -f ego_debug ego_opt ego_prof
	rm -f *~
	rm -f *.orig
