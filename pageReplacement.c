#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pageReplacement.h"
#include "FIFO.c"
#include "Optimal.c"
#include "LRU.c"
#include "secondChance.c"

// main function
int main(int argc, char *argv[]) {
        // Check if the user has provided the correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Error: Please provide 2 arguments (page replacement algorithm and input file).\n");
        return EXIT_FAILURE;
    }


    // Check if the correct number of arguments is provided
    if (argc < 2) {
        fprintf(stderr, "Error: No page replacement algorithm specified.\n");
        return EXIT_FAILURE;
    }

    // Process using the selected page replacement algorithm
    if (strcmp(argv[1], "FIFO") == 0) {
        mainFIFO(argv[2]);
    } else if (strcmp(argv[1], "OPT") == 0) {
        mainOPT(argv[2]);
    } else if (strcmp(argv[1], "LRU") == 0) {
        mainLRU(argv[2]);
    } else if (strcmp(argv[1], "CLK") == 0) {
        mainSecondChance(argv[2]);
    } else {
        fprintf(stderr, "Error: Unknown page replacement algorithm %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS; // Successful termination
}
