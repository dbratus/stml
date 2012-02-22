CC = g++
SRCS = $(wildcard *.cpp)
BIN = bin/stml_test_harness

all: $(BIN)

$(BIN): $(SRCS) | bin
	$(CC) -g3 -fno-inline -O0 -o "$(BIN)" -L"../libstml/lib-dev" -I"../libstml/include" $(SRCS) -l"stml"
	chmod a+x $(BIN)

bin:
	mkdir bin

clean:
	rm -fr bin

.PHONY: clean
