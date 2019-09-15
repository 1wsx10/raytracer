SOURCES=main.cpp libdraw.cpp logger.cpp timer.cpp mutex.cpp matrix.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DFILES=$(SOURCES:.cpp=.d)
PROGRAM=a.out
DFILERULES=-MMD -MP
# remove pedantic. this uses FB code, portability is already out the window
#CFLAGS= -O0 -std=c++2a -Wall $(DFILERULES)
#CFLAGS= -O1 -std=c++2a -Wall $(DFILERULES)
#CFLAGS= -O2 -std=c++2a -Wall $(DFILERULES)
CFLAGS= -O3 -std=c++2a -Wall $(DFILERULES)
#CFLAGS= -g -std=c++2a -Wall $(DFILERULES)
LDFLAGS=$(CFLAGS) -lncurses -lm -pthread
CC=g++
DOT_A_FILES= lib/vector/libvector.a lib/write_screen/libdraw.a
INC= lib/

# "lib/vector/libvector.a" becomes "-Llib/vector/"
L=$(addprefix -L,$(dir $(DOT_A_FILES)))
# "lib/vector/libvector.a" becomes "-lvector"
l=$(addprefix -l,$(notdir $(basename $(subst /lib,/,$(DOT_A_FILES)))))

.PHONY: all clean locate_headers print-% SAY_COMPILING SAY_LINKING
.SUFFIXES:#remove all suffixes
.SUFFIXES: .cpp .h .o .a #define our own

all: $(PROGRAM)

#SAY_LINKING:
#	@echo '===================='
#	@echo '      linking       '
#	@echo '===================='

$(PROGRAM): $(OBJECTS) $(DOT_A_FILES) #SAY_LINKING
	$(CC) $(OBJECTS) -o $@ $(L) $(l) $(LDFLAGS)
	@echo


#SAY_COMPILING:
#	@echo '===================='
#	@echo '     compiling      '
#	@echo '===================='

%.o: %.cpp #SAY_COMPILING
	$(CC) -c $< -o $@ -I'$(INC)' $(CFLAGS)
	@echo

$(DOT_A_FILES):
	@echo
	@echo '===================='
	@printf '     %s' $(addprefix -l,$(notdir $(basename $(subst /lib,/,$@))))
	@echo
	@echo '===================='
	make $(notdir $@) -C $(dir $@)

-include $(DFILES)

locate_headers: $(PROGRAM)
locate_headers: CFLAGS += -H -fsyntax-only

clean:
	-rm $(PROGRAM) *.o *.d
	-for dir in $(dir $(DOT_A_FILES)); do make clean -C $$dir; done

rebuild:clean all

print-%: ; @echo $*=$($*)
