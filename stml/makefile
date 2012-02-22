CC = g++
SRCS = main.cpp error_message.cpp args.cpp
HTMLS = $(subst .stml,.html,$(wildcard tests/*.stml))
HTML_DIFFS = $(subst .stml,.html.diff,$(wildcard tests/*.stml))
INST = /usr/local/bin/stml
BIN = bin/stml
BIN_DEV = bin-dev/stml

all: $(BIN) $(BIN_DEV)

$(BIN): $(SRCS) | bin
	$(CC) -o "$(BIN)" -L"../libstml/lib" -I"../libstml/include" $(SRCS) -l"stml"
	chmod a+x $(BIN)

$(BIN_DEV): $(SRCS) | bin-dev
	$(CC) -o "$(BIN_DEV)" -g3 -fno-inline -O0 -L"../libstml/lib-dev" -I"../libstml/include" $(SRCS) -l"stml"
	chmod a+x $(BIN_DEV)

bin-dev:
	mkdir bin-dev

bin:
	mkdir bin

$(INST): $(BIN)
	cp $(BIN) /usr/local/bin

install: $(INST)

clean:
	rm -fr bin
	rm -fr bin-dev
	rm -f tests/*.html
	rm -f tests/*.html.diff

test: $(BIN) $(HTML_DIFFS) $(HTMLS)

tests/%.html.diff: tests/%.html
	diff $< $<.etalon > $<.diff

tests/%.html: tests/%.stml
	./bin/stml -g html < $< > $@

.PHONY: clean