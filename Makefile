LIBS = -lpcap
OPTS = -Iinc -g
SRCS = $(notdir $(wildcard src/*.c))
OBJS = $(SRCS:.c=.o)
TARGET = auto-router2

all: build/$(TARGET)

build:
	mkdir build/

build/$(TARGET): $(addprefix build/,$(OBJS))
	gcc -o $@ $(OPTS) $^ $(LIBS)

build/%.o: src/%.c | build
	gcc -c -o $@ $(OPTS) $< $(LIBS)

clean:
	rm -rf build/

.PHONY: clean