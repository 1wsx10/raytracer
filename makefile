SOURCES=main.cpp libdraw.cpp logger.cpp timer.cpp mutex.cpp matrix.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DFILES=$(SOURCES:.cpp=.d)
PROGRAM=a.out
DFILERULES=-MMD -MP
RTTI=-fno-rtti
EXCEPTIONS=-fno-exceptions
#OPTIMISATIONS=-g
#OPTIMISATIONS=-O0
#OPTIMISATIONS=-O1
#OPTIMISATIONS=-O2
#OPTIMISATIONS=-O3
OPTIMISATIONS= -Ofast -march=native
# remove pedantic. this uses FB code, portability is already out the window
CFLAGS= $(OPTIMISATIONS) $(RTTI) $(EXCEPTIONS) -std=c++2a -Wall $(DFILERULES)
LDFLAGS=$(CFLAGS) -lncurses -lm -pthread
CC=g++
DOT_A_FILES= lib/vector/libvector.a lib/write_screen/libdraw.a

# "lib/vector/libvector.a" becomes "-Llib/vector/"
L=$(addprefix -L,$(dir $(DOT_A_FILES)))
INC=$(addprefix -I,$(dir $(DOT_A_FILES)))
# "lib/vector/libvector.a" becomes "-lvector"
l=$(addprefix -l,$(notdir $(basename $(subst /lib,/,$(DOT_A_FILES)))))

.PHONY: all clean test locate_headers print-% SAY_COMPILING SAY_LINKING
.SUFFIXES:#remove all suffixes
.SUFFIXES: .cpp .h .o .a #define our own

all: $(PROGRAM)

y_inner: VARFLAGS=-DFRAME_TIMES_NAME=x_inner_frame_times.csv -DDRAW_X_INNER
y_inner: PROGRAM=y_inner.out

x_inner: VARFLAGS=-DFRAME_TIMES_NAME=y_inner_frame_times.csv -DDRAW_Y_INNER
x_inner: PROGRAM=x_inner.out

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
	$(CC) -c $< -o $@ $(INC) $(CFLAGS) $(VARFLAGS)
	@echo

$(DOT_A_FILES):
	@echo
	@echo '===================='
	@printf '     %s' $(addprefix -l,$(notdir $(basename $(subst /lib,/,$@))))
	@echo
	@echo '===================='
	make $(notdir $@) -C $(dir $@)

test:
	make -C test_matrix/
	@echo
	@echo "=======test======="
	test_matrix/test_matrix

-include $(DFILES)

locate_headers: $(PROGRAM)
locate_headers: CFLAGS += -H -fsyntax-only

clean:
	-rm $(PROGRAM) *.o *.d
	-for dir in $(dir $(DOT_A_FILES)); do make clean -C $$dir; done

rebuild:clean all

print-%: ; @echo $*=$($*)
