CC = gcc
CFLAGS = -ansi -pedantic -Wall
TARGET = assembler_program

VPATH = Assembler:Lexical_analysis:preprocessor:Data_structures/Trie:Data_structures/Vector:Output_files

ifdef DEBUG
CFLAGS = -ansi -pedantic -Wall -g
endif


$(TARGET): assembler.o lexical_analysis.o preprocessor.o trie.o vector.o output_files.o main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

Assembler/assembler.o: Assembler/assembler.c Assembler/assembler.h \
 Assembler/../Data_structures/Vector/vector.h \
 Assembler/../Data_structures/Trie/trie.h \
 Assembler/../preprocessor/preprocessor.h \
 Assembler/../preprocessor/../Data_structures/Vector/vector.h \
 Assembler/../preprocessor/../Data_structures/Trie/trie.h \
 Assembler/../Lexical_analysis/lexical_analysis.h \
 Assembler/../Lexical_analysis/../Data_structures/Trie/trie.h \
 Assembler/../Lexical_analysis/../Data_structures/Vector/vector.h \
 Assembler/../Lexical_analysis/../preprocessor/preprocessor.h \
 Assembler/../Output_files/output_files.h \
 Assembler/../Output_files/../Assembler/assembler.h

Lexical_analysis/lexical_analysis.o: Lexical_analysis/lexical_analysis.c \
 Lexical_analysis/lexical_analysis.h Lexical_analysis/../Data_structures/Trie/trie.h \
 Lexical_analysis/../Data_structures/Vector/vector.h \
 Lexical_analysis/../preprocessor/preprocessor.h \
 Lexical_analysis/../preprocessor/../Data_structures/Vector/vector.h \
 Lexical_analysis/../preprocessor/../Data_structures/Trie/trie.h

preprocessor/preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h \
 preprocessor/../Data_structures/Vector/vector.h \
 preprocessor/../Data_structures/Trie/trie.h

Data_structures/Trie/trie.o: Data_structures/Trie/trie.c Data_structures/Trie/trie.h

Data_structures/Vector/vector.o: Data_structures/Vector/vector.c Data_structures/Vector/vector.h

Output_files/output_files.o: Output_files/output_files.c Output_files/output_files.h \
 Output_files/../Assembler/assembler.h \
 Output_files/../Assembler/../Data_structures/Vector/vector.h \
 Output_files/../Assembler/../Data_structures/Trie/trie.h \
 Output_files/../Assembler/../preprocessor/preprocessor.h \
 Output_files/../Assembler/../preprocessor/../Data_structures/Vector/vector.h \
 Output_files/../Assembler/../preprocessor/../Data_structures/Trie/trie.h \
 Output_files/../Assembler/../Lexical_analysis/lexical_analysis.h \
 Output_files/../Assembler/../Lexical_analysis/../Data_structures/Trie/trie.h \
 Output_files/../Assembler/../Lexical_analysis/../Data_structures/Vector/vector.h \
 Output_files/../Assembler/../Lexical_analysis/../preprocessor/preprocessor.h

main.o: main.c Assembler/assembler.h \
 Assembler/../Data_structures/Vector/vector.h \
 Assembler/../Data_structures/Trie/trie.h \
 Assembler/../preprocessor/preprocessor.h \
 Assembler/../preprocessor/../Data_structures/Vector/vector.h \
 Assembler/../preprocessor/../Data_structures/Trie/trie.h \
 Assembler/../Lexical_analysis/lexical_analysis.h \
 Assembler/../Lexical_analysis/../Data_structures/Trie/trie.h \
 Assembler/../Lexical_analysis/../Data_structures/Vector/vector.h \
 Assembler/../Lexical_analysis/../preprocessor/preprocessor.h

