OPT   = -O3 -march=native -fomit-frame-pointer -DDEBUG -ffast-math -frename-registers -ggdb3 
DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -O2 -march=native -DDEBUG -ggdb3 -fno-inline 

CFLAGS += -Wall -Wextra -Wswitch-enum #-static #-Wno-long-long -Wno-variadic-macros

GPP    = g++-4.2 $(CFLAGS) 

FILES  = Makefile utils.cpp config.cpp utils.cpp \
         fast_timer.cpp fast_random.cpp fast_stack.cpp fast_map.cpp \
         player.cpp color.cpp vertex.cpp move.cpp \
         board.cpp playout.cpp uct.cpp sgf.cpp \
         gtp.cpp gtp_board.cpp gtp_sgf.cpp gtp_genmove.cpp \
         experiments.cpp \
         main.cpp 

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
