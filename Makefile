# Makefile for Procedural Music Generation
NAME = proc_music

DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iincludes -g
LDFLAGS = -lasound -lm

SRC_DIR = src/
OBJ_DIR = obj/
BIN_DIR = bin/

NOTES_DIR = notes_utils/
NOTES_SRC = notes

WAVES_DIR = wave_utils/
WAVES_SRC = generate_wave add_waves stereo

SRC_FILES += main
SRC_FILES += $(addprefix $(NOTES_DIR), $(NOTES_SRC))
SRC_FILES += $(addprefix $(WAVES_DIR), $(WAVES_SRC))

SOURCES = $(addprefix $(SRC_DIR), $(addsuffix .c, $(SRC_FILES)))
OBJECTS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))
EXECUTABLE = $(BIN_DIR)$(NAME)

# Get unique subdirectories needed in obj/
OBJ_SUBDIRS = $(sort $(dir $(OBJECTS)))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	@echo "$(MAGENTA)Linking $(NAME)$(DEF_COLOR)"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "$(MAGENTA)Build complete$(DEF_COLOR)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.c | $(OBJ_SUBDIRS)
	@echo "$(GREEN)Compiling $<$(DEF_COLOR)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_SUBDIRS):
	@echo "$(CYAN)Creating object subdirectory: $@$(DEF_COLOR)"
	@mkdir -p $@

$(BIN_DIR):
	@echo "$(CYAN)Creating binary directory: $@$(DEF_COLOR)"
	@mkdir -p $(BIN_DIR)

clean:
	@echo "$(RED)Cleaning up...$(DEF_COLOR)"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "$(MAGENTA)Cleanup complete$(DEF_COLOR)"

.PHONY: all clean