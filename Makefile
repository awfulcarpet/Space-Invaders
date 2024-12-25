WARNING = -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-all
CFLAGS = -std=c99 -O0 $(WARNING) -pipe -ggdb -Iinclude -I/usr/local/include
LDLIBS = -lSDL2
EMCCFLAGS = -s USE_SDL=2 -s USE_GLFW=3 --shell-file minshell.html -s ASYNCIFY --preload-file space-invaders.rom
PLATFORM ?= PLATFORM_DESKTOP

ifeq ($(PLATFORM),WEB)
	CC=emcc
	LDLIBS = $(EMCCFLAGS)
	EXT = .html
endif

NAME = emulator
OUTDIR = .build
OBJ = \
      $(OUTDIR)/main.o \
	  $(OUTDIR)/cpu.o \
	  $(OUTDIR)/machine.o \
	  $(OUTDIR)/dissasembler.o \

all: $(NAME)

run: $(NAME)
	$(OUTDIR)/$(NAME) space-invaders.rom

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(OUTDIR)
	$(CC) -c $(CFLAGS) -o $@ $< -D$(PLATFORM) $(LDLIBS)

$(NAME): $(OBJ)
	$(CC) -o $(OUTDIR)/$@$(EXT) $^ $(LDLIBS) $(LDFLAGS)

web-release: clean $(NAME)
	@rm -rf pub index.html
	@mkdir -p pub
	mv -f .build/$(NAME).* pub/
	mv pub/emulator.html pub/index.html
	# cp pub/$(NAME).data .
	# sed 's/$(NAME).js/pub\/$(NAME).js/g' pub/$(NAME).html > index.html

tests: clean
	@mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)/tests  $(CFLAGS) $(LDLIBS) $(LDFLAGS) src/dissasembler.c src/cpu.c tests/emulator.c
	$(OUTDIR)/tests

release: $(NAME)
	strip $(OUTDIR)/$(NAME)

clean:
	rm -rf .build/ log core index.html $(NAME).data pub
