SOURCES=main.cpp
#HEADERS=main.h
OBJECTS=$(SOURCES:.cpp=.o)
DFILES=$(SOURCES:.cpp=.d)
PROGRAM=a.out
DFILERULES=-MMD -MP
CFLAGS= -pedantic -Wall $(DFILERULES)
LDFLAGS=$(CFLAGS) -lncurses -lm
CC=g++
DOT_A_FILES= lib/vector/libvector.a lib/write_screen/libdraw.a
INC= lib/

# "lib/vector/libvector.a" becomes "-Llib/vector/"
L=$(addprefix -L,$(dir $(DOT_A_FILES)))
# "lib/vector/libvector.a" becomes "-lvector"
l=$(addprefix -l,$(notdir $(basename $(subst /lib,/,$(DOT_A_FILES)))))

.PHONY: all clean
.SUFFIXES:#remove all suffixes
.SUFFIXES: .cpp .h .o #define our own

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	@echo '===================='
	@echo '      linking       '
	@echo '===================='
	$(CC) $^ -o $@ $(L) $(l) $(LDFLAGS)

%.o: %.cpp
	@echo '===================='
	@echo '     compiling      '
	@echo '===================='
	$(CC) -c $< -o $@ -I'$(INC)' $(CFLAGS)

-include $(DFILES)

clean:
	-rm $(PROGRAM) *.o *.d

rebuild:clean all
