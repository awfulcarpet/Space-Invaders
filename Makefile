WARNING = -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-all
CFLAGS = -std=c99 -O2 $(WARNING) -pipe -ggdb -Iinclude -I/usr/local/include
LDLIBS = -lraylib -lGL -lm -lX11 -lpthread -ldl -lrt
EMCCFLAGS = lib/libraylib.a -s USE_GLFW=3 --shell-file minshell.html -s ASYNCIFY
PLATFORM ?= PLATFORM_DESKTOP

ifeq ($(PLATFORM),WEB)
	CC=emcc
	LDLIBS = $(EMCCFLAGS)
	EXT = .html
endif

# LDFLAGS = -static
PREFIX = /usr/local/bin
NAME = dissasembler
OUTDIR = .build
OBJ = \
      $(OUTDIR)/main.o \
	  $(OUTDIR)/cpu.o \
	  $(OUTDIR)/dissasembler.o \
	  $(OUTDIR)/machine.o \

all: $(NAME)

run: $(NAME)
	$(OUTDIR)/$(NAME) space-invaders.rom

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(OUTDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(NAME): $(OBJ)
	$(CC) -o $(OUTDIR)/$@$(EXT) $^ $(LDLIBS) $(LDFLAGS)

release: $(NAME)
	strip $(OUTDIR)/$(NAME)

clean:
	rm -rf .build/ log core
