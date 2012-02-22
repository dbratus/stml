CC   = g++
SRCS = $(wildcard src/*.cpp) \
       $(wildcard src/generators/*.cpp) \
       $(wildcard src/languages/*.cpp) \
       $(wildcard src/languages/russian/*.cpp)
OBJS = $(subst src/, lib/, $(subst .cpp,.o, $(SRCS)))
OBJS_DEV = $(subst src/, lib-dev/, $(subst .cpp,.o, $(SRCS)))
LIB  = lib/libstml.a
LIB_DEV = lib-dev/libstml.a

all: $(LIB) $(LIB_DEV)

$(LIB) : $(OBJS)
	ar -r $(LIB) $(OBJS)

$(LIB_DEV) : $(OBJS_DEV)
	ar -r $(LIB_DEV) $(OBJS_DEV)
	
$(OBJS) : | lib	lib/generators lib/languages/russian

$(OBJS_DEV) : | lib-dev	lib-dev/generators lib-dev/languages/russian

lib/%.o : src/%.cpp
	$(CC) -c -o"$@" "$<"

lib-dev/%.o : src/%.cpp
	$(CC) -c -g3 -fno-inline -O0 -o"$@" "$<"

lib/generators: | lib
	mkdir lib/generators

lib/languages: | lib
	mkdir lib/languages

lib/languages/russian: | lib/languages
	mkdir lib/languages/russian

lib-dev/generators: | lib-dev
	mkdir lib-dev/generators

lib-dev/languages: | lib-dev
	mkdir lib-dev/languages

lib-dev/languages/russian: | lib-dev/languages
	mkdir lib-dev/languages/russian

lib:
	mkdir lib

lib-dev:
	mkdir lib-dev

clean:
	rm -rf lib
	rm -rf lib-dev

.PHONY: all clean