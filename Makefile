WARNING = -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-all
CFLAGS = -std=c99 -O2 $(WARNING) -pipe -ggdb -Iinclude -I/usr/local/include

# LDFLAGS = -static
PREFIX = /usr/local/bin
NAME = dissasembler
OUTDIR = .build
OBJ = \
      $(OUTDIR)/main.o \
	  $(OUTDIR)/cpu.o \

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
