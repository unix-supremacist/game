include make/color.mk
BUILD_DIR = build
PRINT = printf

CFLAGS += $(OPTFLAGS) $(WARNINGFLAGS)
LINE = __________________________________________________
SPACE = |                                                  |

CMAKE = cmake -G Ninja -B$(BUILD_DIR) -S. -DCMAKE_C_FLAGS="$(CFLAGS)"

default: all

.PHONY: all clean
clean:
	@rm -r $(BUILD_DIR)
	@$(CMAKE) --fresh

test: all
	@$(BUILD_DIR)/Game

all:
	@$(CMAKE)
	@$(BUILD_TOOL) -C $(BUILD_DIR)
	@$(PRINT) "$(YELLOW)_$(LINE)_\n"
	@$(PRINT) "$(SPACE)\n"
	@$(PRINT) "|                 $(FLASH)Compile Settings$(WHITE)                 $(YELLOW)|\n"
	@$(PRINT) "|$(LINE)|$(WHITE)\n"
	@$(PRINT) "$(RED)Compiler:      $(PURPLE)$(CC)$(WHITE)\n"
	@$(PRINT) "$(RED)Optimizations: $(PURPLE)$(OPTFLAGS)$(WHITE)\n"