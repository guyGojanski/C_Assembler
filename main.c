#include "Middle_end/assembler.h"

int main(int argc, char **argv){
    assembler(argc-1,argv+1);
    return 0;
}