WARNING = -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-all
CFLAGS = -std=c99 -O0 $(WARNING) -pipe -ggdb -Iinclude -I/usr/local/include
LDLIBS = -lSDL2
EMCCFLAGS = -s USE_SDL=2 -s USE_GLFW=3 --shell-file minshell.html -s ASYNCIFY --preload-file $(ROM)
PLATFORM ?= PLATFORM_DESKTOP

ifeq ($(PLATFORM),WEB)
	CC=emcc
	LDLIBS = $(EMCCFLAGS)
	EXT = .html
endif

ROM = space-invaders.rom
NAME = emulator
OUTDIR = .build
OBJ = \
      $(OUTDIR)/main.o \
	  $(OUTDIR)/cpu.o \
	  $(OUTDIR)/machine.o \
	  $(OUTDIR)/dissasembler.o \

all: $(NAME)

run: $(NAME)
	$(OUTDIR)/$(NAME) $(ROM)

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(OUTDIR)
	$(CC) -c $(CFLAGS) -o $@ $< -D$(PLATFORM) $(LDLIBS) -DROM=\"$(ROM)\"

$(NAME): $(OBJ)
	$(CC) -o $(OUTDIR)/$@$(EXT) $^ $(LDLIBS) $(LDFLAGS)

web-release: clean $(NAME)
	@rm -rf pub index.html
	@mkdir -p pub
	mv -f .build/$(NAME).* pub/
	mv pub/emulator.html pub/index.html
	cp res/favicon.png pub/favicon.png

tests: clean
	@mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)/tests  $(CFLAGS) $(LDLIBS) $(LDFLAGS) src/dissasembler.c src/cpu.c tests/emulator.c
	$(OUTDIR)/tests

release: $(NAME)
	strip $(OUTDIR)/$(NAME)

clean:
	rm -rf .build/ log core index.html $(NAME).data pub
