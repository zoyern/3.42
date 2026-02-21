# *************************************************************** #
#                    Build configuration                            #
#                                                                   #
#    C11 strict, tous les warnings, zero tolerance.                 #
#    -DS3_ARENA_BACKEND=0 par defaut (malloc).                      #
#    -DS3_ARENA_BACKEND=1 pour mmap (Linux/Mac).                    #
#    -DS3_ARENA_BACKEND=2 pour embedded (buffer statique).          #
# *************************************************************** #

CC			= cc
CFLAGS		= -std=c11 -Wall -Wextra -Werror -Wpedantic \
			  -Wconversion -Wformat=2 -Wnull-dereference -Wvla \
			  -Wcast-qual -Wpointer-arith -Wshadow \
			  -Wstrict-prototypes \
			  -I./src -g3 -gdwarf-4 -O0
BUILD_DIR	= build

# --- Sources (modules actifs) ---
SRCS		= src/main.c \
			  src/arena/s3_arena.c \
			  src/trit/s3_trit.c

# --- Objects ---
OBJS		= $(SRCS:src/%.c=$(BUILD_DIR)/%.o)

# --- Build rule ---
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
