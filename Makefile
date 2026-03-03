include source.mk

NAME	 =	pestilence
NAME_CLEAN =	pestilence_clean

VIRGIN = $(NAME).virgin

ODIR	 =	objs/
IDIR	 =	incs/
SDIR	 =	srcs/

OBJS	 =	$(addsuffix .o, $(addprefix $(ODIR), $(SRCS)))

CFLAGS   =	-I$(IDIR)
CFLAGS	 +=	-Werror
CFLAGS	 +=	-nostdlib -fno-builtin
CFLAGS	 += -g # TODO: remove
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


# ASM_DIR	=	libasm/
# ASM_SRC	=	proc_detach.s proc_spawn.s fs_handle.s fs_enumerate.s fs_query.s \
# 			fs_release.s vm_release.s vm_reserve.s vm_flush.s io_query.s     \
# 			io_resize.s vm_resize.s proc_terminate.s io_send.s
# ASM_OBJ =	$(addprefix $(ASM_DIR), $(ASM_SRC:.s=.o))

bonus: CFLAGS += -DBONUS
bonus: re

all: $(NAME)

clean:
	$(RM) $(ODIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

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
# 	echo varax: $$VARAX; \
# 	echo FRENZY: $$FRENZY; \
# 	echo CYANURE: $$CYANURE; \
# 	echo BUBONIK: $$BUBONIK; \
# 	echo ECHIDNAE: $$ECHIDNAE; \
	$(CC) $(CFLAGS) \
		-DFRENZY="$$FRENZY" \
		-DVARAX="$$VARAX"\
		-DCYANURE="$$CYANURE" \
		-DECHIDNAE="$$ECHIDNAE" \
		-DBUBONIK="$$BUBONIK" -c $< -o $@

$(VIRGIN): $(addsuffix .virgin, $(OBJS))
	@$(CC) $(CFLAGS) -no-pie $^ -o $@

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -no-pie $^ -o $@
	@strip --strip-all $@
	@objcopy \
	  --remove-section .comment \
	  --remove-section .note \
	 $@

$(NAME_CLEAN):



.PHONY: all clean fclean re
