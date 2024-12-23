WARNING = -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-all
CFLAGS = -std=c99 -O2 $(WARNING) -pipe -ggdb -Iinclude -I/usr/local/include

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
	$(CC) -c $(CFLAGS) -o $@ $<

$(NAME): $(OBJ)
	$(CC) -o $(OUTDIR)/$@$(EXT) $^ $(LDLIBS) $(LDFLAGS)

tests: clean
	@mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)/tests -D TEST $(CFLAGS) $(LDLIBS) $(LDFLAGS) src/dissasembler.c src/cpu.c tests/emulator.c
	$(OUTDIR)/tests

release: $(NAME)
	strip $(OUTDIR)/$(NAME)

clean:
	rm -rf .build/ log core
