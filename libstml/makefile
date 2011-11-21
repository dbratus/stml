CC   = g++
SRCS = $(wildcard src/*.cpp) \
       $(wildcard src/generators/*.cpp) \
       $(wildcard src/languages/*.cpp) \
       $(wildcard src/languages/russian/*.cpp)
OBJS = $(subst src/, lib/, $(subst .cpp,.o, $(SRCS)))
LIB  = lib/libstml.a

all: $(LIB)

$(LIB) : $(OBJS)
	ar -r $(LIB) $(OBJS)

$(OBJS) : | lib	lib/generators lib/languages/russian

lib/%.o : src/%.cpp
	$(CC) -c -o"$@" "$<"

lib/generators: | lib
	mkdir lib/generators

lib/languages: | lib
	mkdir lib/languages

lib/languages/russian: | lib/languages
	mkdir lib/languages/russian

lib:
	mkdir lib

clean:
	rm -rf lib

.PHONY: all clean