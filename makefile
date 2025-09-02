CC						= gcc
CFLAGS					= -ansi -pedantic -Wall
LDFLAGS 				= -lm
PROG_NAME 				= assembly_compiler
BUILD_DIR 				= build
OBJECT_DIR				= $(BUILD_DIR)/obj
EXE_DIR					= $(BUILD_DIR)/exe
ZIP_NAME				= mmn14.zip

ifdef DEBUG
CFLAGS 					= -ansi -pedantic -Wall -g -fsanitize=address
endif

TEST_DIR				= tests
TEST_DIR_IN_ERROR		= $(TEST_DIR)/error_test
TEST_DIR_IN_WARNING		= $(TEST_DIR)/warning_test
TEST_DIR_IN_GOOD		= $(TEST_DIR)/good_test
TEST_DIR_IN_MIX			= $(TEST_DIR)/mix_test

.PHONY: clean build_prog all

all: build_prog $(PROG_NAME)

build_prog:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJECT_DIR)
	mkdir -p $(EXE_DIR)

clean: clean_files
	rm -rf $(BUILD_DIR)	

$(PROG_NAME): build_prog assembler.o lexical_analysis.o preprocessor.o trie.o vector.o output_files.o main.o
	$(CC) $(CFLAGS) $(OBJECT_DIR)/*.o -o$(EXE_DIR)/$@ $(LDFLAGS)

clean_files:
	find $(TEST_DIR) \( -name '*.am' -o -name '*.ob' -o -name '*.ext' -o -name '*.ent' \) -exec rm -f {} +

run_test_error: $(PROG_NAME)
	$(EXE_DIR)/$(PROG_NAME) $(TEST_DIR_IN_ERROR)/error_1/error_1 -o$(TEST_DIR_IN_ERROR)/error_1 $(TEST_DIR_IN_ERROR)/error_2/error_2 -o$(TEST_DIR_IN_ERROR)/error_2
	
run_test_good: $(PROG_NAME)
	$(EXE_DIR)/$(PROG_NAME) $(TEST_DIR_IN_GOOD)/good_1/good_1 -o$(TEST_DIR_IN_GOOD)/good_1	$(TEST_DIR_IN_GOOD)/good_2/good_2 -o$(TEST_DIR_IN_GOOD)/good_2	$(TEST_DIR_IN_GOOD)/good_3/good_3 -o$(TEST_DIR_IN_GOOD)/good_3

run_test_mix: $(PROG_NAME)
	$(EXE_DIR)/$(PROG_NAME) $(TEST_DIR_IN_MIX)/mix_1/mix_1 -o$(TEST_DIR_IN_MIX)/mix_1	$(TEST_DIR_IN_MIX)/mix_2/mix_2 -o$(TEST_DIR_IN_MIX)/mix_2

run_test_warning: $(PROG_NAME)
	$(EXE_DIR)/$(PROG_NAME) $(TEST_DIR_IN_WARNING)/warning_1/warning_1 -o$(TEST_DIR_IN_WARNING)/warning_1

zip: clean
	rm	-f	$(ZIP_NAME)
	zip	-r	$(ZIP_NAME) *



assembler.o: Middle_end/assembler.c Middle_end/assembler.h \
 Middle_end/../Data_structures/Vector/vector.h \
 Middle_end/../Data_structures/Trie/trie.h \
 Middle_end/../preprocessor/preprocessor.h \
 Middle_end/../preprocessor/../Data_structures/Trie/trie.h \
 Middle_end/../preprocessor/../Data_structures/Vector/vector.h \
 Middle_end/../Front_end/lexical_analysis.h \
 Middle_end/../Front_end/../Data_structures/Trie/trie.h \
 Middle_end/../Front_end/../Data_structures/Vector/vector.h \
 Middle_end/../Front_end/../preprocessor/preprocessor.h \
 Middle_end/../Back_end/output_files.h \
 Middle_end/../Back_end/../Middle_end/assembler.h

trie.o: Data_structures/Trie/trie.c Data_structures/Trie/trie.h

vector.o: Data_structures/Vector/vector.c Data_structures/Vector/vector.h

lexical_analysis.o: Front_end/lexical_analysis.c \
 Front_end/lexical_analysis.h Front_end/../Data_structures/Trie/trie.h \
 Front_end/../Data_structures/Vector/vector.h \
 Front_end/../preprocessor/preprocessor.h \
 Front_end/../preprocessor/../Data_structures/Trie/trie.h \
 Front_end/../preprocessor/../Data_structures/Vector/vector.h

preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h \
 preprocessor/../Data_structures/Trie/trie.h \
 preprocessor/../Data_structures/Vector/vector.h

output_files.o: Back_end/output_files.c Back_end/output_files.h \
 Back_end/../Middle_end/assembler.h \
 Back_end/../Middle_end/../Data_structures/Vector/vector.h \
 Back_end/../Middle_end/../Data_structures/Trie/trie.h \
 Back_end/../Middle_end/../preprocessor/preprocessor.h \
 Back_end/../Middle_end/../preprocessor/../Data_structures/Trie/trie.h \
 Back_end/../Middle_end/../preprocessor/../Data_structures/Vector/vector.h \
 Back_end/../Middle_end/../Front_end/lexical_analysis.h \
 Back_end/../Middle_end/../Front_end/../Data_structures/Trie/trie.h \
 Back_end/../Middle_end/../Front_end/../Data_structures/Vector/vector.h \
 Back_end/../Middle_end/../Front_end/../preprocessor/preprocessor.h
 
 main.o: main.c Middle_end/assembler.h \
 Middle_end/../Data_structures/Vector/vector.h \
 Middle_end/../Data_structures/Trie/trie.h \
 Middle_end/../preprocessor/preprocessor.h \
 Middle_end/../preprocessor/../Data_structures/Vector/vector.h \
 Middle_end/../preprocessor/../Data_structures/Trie/trie.h \
 Middle_end/../Front_end/lexical_analysis.h \
 Middle_end/../Front_end/../Data_structures/Trie/trie.h \
 Middle_end/../Front_end/../Data_structures/Vector/vector.h \
 Middle_end/../Front_end/../preprocessor/preprocessor.h

 
%.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJECT_DIR)/$@

