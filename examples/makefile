SRC = \
	main.c \
	../*.c
HEADERS = \
	main.h \
	../*.h

TARGET = classic_test
LIBS = -lz
OPTS = -Wall -Wextra -Wfatal-errors -Wno-ignored-qualifiers -g
FLAGS = $(LIBS) $(OPTS)
OUT = ./$(TARGET)
DEBUG = ./debug/$(TARGET)
CC = gcc

default: $(OUT)

debug: $(DEBUG)

$(OUT): $(SRC) $(HEADERS)
	$(CC) -o $(OUT) $(SRC) $(FLAGS)

$(DEBUG): $(SRC) $(HEADERS)
	$(CC) -g -o $(DEBUG) $(SRC) $(FLAGS)

clean:
	rm $(OUT) -f
	rm $(DEBUG) -f

