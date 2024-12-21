WARNING = -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-all -ggdb
CFLAGS = -std=c99 -O2 $(WARNING) -pipe -ggdb -Iinclude -I/usr/local/include
LDLIBS = -lraylib -lGL -lm -lX11 -lpthread -ldl -lrt
EMCCFLAGS = lib/libraylib.a -s USE_GLFW=3 --shell-file minshell.html -s ASYNCIFY -s FORCE_FILESYSTEM=1 --preload-file .
PLATFORM ?= PLATFORM_DESKTOP

ifeq ($(PLATFORM),WEB)
	CC=emcc
	LDLIBS = $(EMCCFLAGS)
	EXT = .html
endif

# LDFLAGS = -static
PREFIX = /usr/local/bin
NAME = space_invaders_emu
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
	$(CC) -c $(CFLAGS) -o $@ $< -D$(PLATFORM)

$(NAME): $(OBJ)
	$(CC) -o $(OUTDIR)/$@$(EXT) $^ $(LDLIBS) $(LDFLAGS)

release: $(NAME)
	strip $(OUTDIR)/$(NAME)

web-release: clean $(NAME)
	@rm -rf pub index.html
	@mkdir -p pub
	mv -f .build/$(NAME).* pub/
	sed "s/$(NAME).js/pub\/$(NAME).js/g" pub/$(NAME).html > index.html
	cp pub/$(NAME).data $(NAME).data

clean:
	rm -rf .build/ log core
