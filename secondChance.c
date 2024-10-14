#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 50

typedef struct {
    int page_number;
    unsigned int ref_register;  //Reference register of n bits
    int dirty;                  
} Page;

Page *frames;
int num_frames = MAX_FRAMES;
int write_back_count = 0;  //Count the number of write backs

//function to find a page in the frame
int find_page_in_frames(int page_number) {
    for (int i = 0; i < num_frames; i++) {
        if (frames[i].page_number == page_number) {
            return i;
        }
    }
    return -1;
}

//Function to check if all reference registers are equal
int all_ref_registers_equal() {
    unsigned int first_reg = frames[0].ref_register;
    for (int i = 1; i < num_frames; i++) {
        if (frames[i].ref_register != first_reg) {
            return 0;  //Registers are not equal
        }
    }
    return 1;  //All registers are equal
}

//Function to find the index of the page with the lowest reference register
int find_lowest_reference_page() {
    int lowest_index = 0;
    unsigned int lowest_value = frames[0].ref_register;
    for (int i = 1; i < num_frames; i++) {
        if (frames[i].ref_register < lowest_value) {
            lowest_index = i;
            lowest_value = frames[i].ref_register;
        }
    }
    return lowest_index;
}

//Function to find the replacement index in the Second Chance
int get_replacement_index(int *clock_hand, int n) {
    //If all reference registers are equal then fall back to FIFO
    if (all_ref_registers_equal()) {
        int oldest_index = *clock_hand;
        *clock_hand = (*clock_hand + 1) % num_frames;
        return oldest_index;  // FIFO tiebreaker
    }

    //replace the page with the lowest reference register
    return find_lowest_reference_page();
}
//Simulate the Second Chance algorithm with reference registers
int simulate_second_chance(Page *listOfPages, int num_pages, int n, int m) {
    int page_faults = 0;
    int clock_hand = 0;
    int reference_count = 0;
    write_back_count = 0;  //Reset the write back count for each experiment

    //Initialize frames
    for (int i = 0; i < num_frames; i++) {
        frames[i].page_number = -1;  //This is an empty frame
        frames[i].ref_register = 0;  //Initialize the reference register to 0
        frames[i].dirty = 0;
    }

    for (int i = 0; i < num_pages; i++) {
        int page_number = listOfPages[i].page_number; //Get the current page number from the list of pages
        int dirty_bit = listOfPages[i].dirty;  //Get the dirty bit 
        int page_index = find_page_in_frames(page_number);

        if (page_index == -1) {
            //Page fault 
            page_faults++;

            //Find replacement page using clock hand
            int replace_index = get_replacement_index(&clock_hand, n);

            //Simulate writeback if the page to be replaced is dirty
            if (frames[replace_index].dirty == 1) {
                write_back_count++;
            }

            //Replace the page in frames
            frames[replace_index].page_number = page_number;
            frames[replace_index].ref_register = (1 << (n - 1));  //Set the leftmost bit of the register
            frames[replace_index].dirty = dirty_bit;  //Set the dirty bit for the new page
        } else {
            //if the page is found set the leftmost bit of the reference register and update dirty bit
            frames[page_index].ref_register |= (1 << (n - 1));  //Set leftmost bit of the register
            frames[page_index].dirty = dirty_bit;  //Update the dirty bit
        }

        //Increment reference count and check if we need to shift the registers
        reference_count++;
        if (reference_count == m) {
            //Shift reference registers
            for (int j = 0; j < num_frames; j++) {
                frames[j].ref_register >>= 1;  //Shift reference register to the right
                frames[j].ref_register &= (1 << n) - 1;  //Keep only the n bits in the register
            }
            reference_count = 0;
        }
    }

    return page_faults;
}
//Experiment 1: Vary n from 1 to 32 with m = 10
void run_experiment_vary_n(Page *listOfPages, int num_pages) {
    int m_fixed = 10;
    printf("Experiment 1: Vary n (m = 10)\n");
    printf("+--------+--------------+-----------------+\n");
    printf("| n      | Page Faults   | Write Backs     |\n");
    printf("+--------+--------------+-----------------+\n");

    for (int n = 1; n <= 32; n++) {
        int page_faults = simulate_second_chance(listOfPages, num_pages, n, m_fixed);
        printf("| %-6d | %-12d | %-15d |\n", n, page_faults, write_back_count);
    }

    printf("+--------+--------------+-----------------+\n");
}

//Experiment 2: Vary m from 1 to 100 with n = 8
void run_experiment_vary_m(Page *listOfPages, int num_pages) {
    int n_fixed = 8;
    printf("Experiment 2: Vary m (n = 8)\n");
    printf("+--------+--------------+-----------------+\n");
    printf("| m      | Page Faults   | Write Backs     |\n");
    printf("+--------+--------------+-----------------+\n");

    for (int m = 1; m <= 100; m++) {
        int page_faults = simulate_second_chance(listOfPages, num_pages, n_fixed, m);
        printf("| %-6d | %-12d | %-15d |\n", m, page_faults, write_back_count);
    }

    printf("+--------+--------------+-----------------+\n");
}
int main() {
    FILE *file;
    char line[256];
    Page *listOfPages = NULL;  // Pointer for dynamic allocation
    int pageCount = 0;

    // Open CSV file for reading
    file = fopen("inputfile.txt", "r");
    if (file == NULL) {
        perror("File not found, make sure input file is named: inputfile.txt");
        return 1;
    }

    // Count the number of lines with valid data in the file
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '\n' && strlen(line) > 1) { // Check if the line is not empty and not just a newline
            pageCount++;
        }
    }

    // Allocate memory for the pages
    listOfPages = malloc(pageCount * sizeof(Page));
    frames = malloc(MAX_FRAMES * sizeof(Page));  // Allocate memory for frames
    if (listOfPages == NULL || frames == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    // Rewind the file to read the data again
    rewind(file);

    // Read and ignore the header line, if present
    fgets(line, sizeof(line), file);  // Skip the header line

    // Read each line of the CSV file
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || strlen(line) == 0) continue; // Improved empty line check

        if (sscanf(line, "%d,%d", &listOfPages[index].page_number, &listOfPages[index].dirty) == 2) {
            index++; // Increment the page index
        }
    }

    fclose(file);  // Close the file after reading

    // Run experiment 1: Fix m = 10, vary n
    run_experiment_vary_n(listOfPages, pageCount);

    // Run experiment 2: Fix n = 8, vary m
    run_experiment_vary_m(listOfPages, pageCount);

    
    free(listOfPages);
    free(frames);

    return 0;
}
