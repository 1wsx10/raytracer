SOURCES=main.cpp libdraw.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DFILES=$(SOURCES:.cpp=.d)
PROGRAM=a.out
DFILERULES=-MMD -MP
CFLAGS= -g -std=c++2a -pedantic -Wall $(DFILERULES)
LDFLAGS=$(CFLAGS) -lncurses -lm
CC=g++
DOT_A_FILES= lib/vector/libvector.a lib/write_screen/libdraw.a
INC= lib/

# "lib/vector/libvector.a" becomes "-Llib/vector/"
L=$(addprefix -L,$(dir $(DOT_A_FILES)))
# "lib/vector/libvector.a" becomes "-lvector"
l=$(addprefix -l,$(notdir $(basename $(subst /lib,/,$(DOT_A_FILES)))))

.PHONY: all clean SAY_COMPILING SAY_LINKING
.SUFFIXES:#remove all suffixes
.SUFFIXES: .cpp .h .o .a #define our own

all: $(PROGRAM)

SAY_LINKING:
	@echo '===================='
	@echo '      linking       '
	@echo '===================='

$(PROGRAM): $(OBJECTS) $(DOT_A_FILES) SAY_LINKING
	$(CC) $(OBJECTS) -o $@ $(L) $(l) $(LDFLAGS)
	@echo


SAY_COMPILING:
	@echo '===================='
	@echo '     compiling      '
	@echo '===================='

%.o: %.cpp SAY_COMPILING
	$(CC) -c $< -o $@ -I'$(INC)' $(CFLAGS)
	@echo

$(DOT_A_FILES):
	@echo '===================='
	@printf '     %s' $(addprefix -l,$(notdir $(basename $(subst /lib,/,$@))))
	@echo
	@echo '===================='
	make $(notdir $@) -C $(dir $@)

-include $(DFILES)

clean:
	-rm $(PROGRAM) *.o *.d
	-make clean -C $(dir $(DOT_A_FILES))

rebuild:clean all
