CC = g++
SRCS = main.cpp error_message.cpp args.cpp
HTMLS = $(subst .stml,.html,$(wildcard tests/*.stml))
HTML_DIFFS = $(subst .stml,.html.diff,$(wildcard tests/*.stml))
INST = /usr/local/bin/stml
BIN = bin/stml

all: $(BIN)

$(BIN): $(SRCS) | bin
	$(CC) -o "$(BIN)" -L"../libstml/lib" -I"../libstml/include" $(SRCS) -l"stml"
	chmod a+x $(BIN)

bin:
	mkdir bin

$(INST): $(BIN)
	cp $(BIN) /usr/local/bin

install: $(INST)

clean:
	rm -fr bin
	rm -f tests/*.html
	rm -f tests/*.html.diff

test: $(BIN) $(HTML_DIFFS) $(HTMLS)

tests/%.html.diff: tests/%.html
	diff $< $<.etalon > $<.diff

tests/%.html: tests/%.stml
	./bin/stml -g html < $< > $@

.PHONY: clean