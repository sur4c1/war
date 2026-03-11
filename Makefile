include source.mk

NAME	 =	war
NAME_CLEAN =	war_clean

VIRGIN = $(ODIR)/$(NAME).virgin

ODIR	 =	objs/
IDIR	 =	incs/
SDIR	 =	srcs/

OBJS	 =	$(addsuffix .o, $(addprefix $(ODIR), $(SRCS)))

CFLAGS   =	-I$(IDIR)
CFLAGS	 +=	-Werror
CFLAGS	 +=	-nostdlib -fno-builtin
CFLAGS	 += -O0 -finline-functions
CFLAGS 	 += -fomit-frame-pointer
CFLAGS	 += -fno-asynchronous-unwind-tables
CFLAGS	 += -fno-merge-all-constants
CFLAGS	 += -fno-optimize-sibling-calls
CFLAGS	 += -fno-jump-tables
CFLAGS	 += -fno-pic
CFLAGS	 += -nodefaultlibs
CFLAGS	 += -fno-stack-protector
CFLAGS	 += -fno-asynchronous-unwind-tables
CFLAGS	 += -fno-unwind-tables
CFLAGS	 += -fno-semantic-interposition
CFLAGS	 += -fvisibility=hidden
NFLAGS	  =	-f elf64

CC		 =	clang
RM		 =	@rm -rf
MKDIR	 =	@mkdir -p
NASM	 =	@nasm


all: $(NAME)

clean:
	$(RM) $(ODIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

bonus: CFLAGS += -DBONUS
bonus: re

$(ODIR)%.o.virgin:  $(SDIR)%.c
	$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)%.o:  $(SDIR)%.c $(VIRGIN)
	$(MKDIR) $(dir $@)
	@FRENZY="0x$$(readelf -S  $(VIRGIN) | grep .text | awk '{print $$5}')"; \
	VARAX="0x$$(readelf -S  $(VIRGIN) | grep -A1 .text | tail -n1 | awk '{print $$1}')"; \
	CYANURE="0x$$(readelf -s  $(VIRGIN) | grep cyanure | awk '{print $$2}')"; \
	BUBONIK="0x$$(readelf -s  $(VIRGIN) | grep "\b_start\b" | awk '{print $$2}')"; \
	ECHIDNAE="0x$$(readelf -s  $(VIRGIN) | grep "\bstr4\b" | awk '{print $$2}')"; \
	$(CC) $(CFLAGS) \
		-DFRENZY="$$FRENZY" \
		-DVARAX="$$VARAX"\
		-DCYANURE="$$CYANURE" \
		-DECHIDNAE="$$ECHIDNAE" \
		-DBUBONIK="$$BUBONIK" -c $< -o $@

$(VIRGIN): $(addsuffix .virgin, $(OBJS))
	@$(CC) $(CFLAGS) -g -no-pie $^ -o $@

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -no-pie $^ -o $@
	@strip --strip-all $@
	@objcopy \
	  --remove-section .comment \
	  --remove-section .note \
	 $@

print_curare: curare
	@hexdump -e '16/1 "%02x " "\n"' curare | xargs | sed 's/ /, /g' | sed 's/^/ /g' | sed 's/ / 0x/g'

curare: test/curare.s
	@nasm test/curare.s -o curare

.PHONY: all clean fclean re print_curare
