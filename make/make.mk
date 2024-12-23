include make/color.mk
BUILD_DIR = build
PRINT = printf

WARNINGFLAGS = -Wall -Wpedantic -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-error=maybe-uninitialized \
-Wno-error=maybe-uninitialized -Wno-error=sign-compare -Wno-error=implicit-fallthrough -Wno-error=missing-field-initializers -Wno-pedantic
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