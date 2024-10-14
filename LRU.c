#include <stdio.h>
#include <stdlib.h>
#include "pageReplacement.h"


//Function to find if the page is in the frame
int findPageIndex(int *frames, int frame_count, int page) {
    for (int i = 0; i < frame_count; i++) {
        if (frames[i] == page) {
            return i;
        }
    }
    return -1;
}

//Function to find the least recently used page index
int findLRUIndex(int *access_time, int frame_count) {
    int lru_index = 0;
    for (int i = 1; i < frame_count; i++) {
        if (access_time[i] < access_time[lru_index]) {
            lru_index = i;
        }
    }
    return lru_index;
}

void LRU(Page pages[], int count, int frame_count) {
    int *frames = (int *)malloc(frame_count * sizeof(int)); //Array for the frames
    int *access_time = (int *)malloc(frame_count * sizeof(int)); //Array to track access time
    int *dirty_bits = (int *)malloc(frame_count * sizeof(int)); //Array to track the dirty bits for frames
    int page_faults = 0;  //number of page faults
    int writeBacks = 0;
    int current_time = 0; //tracking the access order

    //Initialize frames, access times, and dirty bits
    for (int i = 0; i < frame_count; i++) {
        frames[i] = -1; //Empty frame
        access_time[i] = -1; //set frame as never been accessed
        dirty_bits[i] = 0; //Initialize the dirty bits
    }

    for (int i = 0; i < count; i++) {  //Iterate through all of the pages
        int current_page = pages[i].page_number; //Get the current page number from the list of pages
        int current_dirty = pages[i].dirty; //Get the current page dirty status
        int page_index = findPageIndex(frames, frame_count, current_page); //Current page in the frame

        if (page_index == -1) { //Page fault occurs
            page_faults++;

            //Find the least recently used page index
            int lru_index = findLRUIndex(access_time, frame_count);

            //Check and replace if a different page is being loaded into the frame
            if (frames[lru_index] != current_page) {
                //Check if the frame to be replaced is dirty
                if (frames[lru_index] != -1 && dirty_bits[lru_index] == 1) {
                    writeBacks++;
                }
                
                                                                                                                                                                                                    //Replace the page with the current page
                frames[lru_index] = current_page;
                access_time[lru_index] = current_time; //Update the access time for the new page
                dirty_bits[lru_index] = current_dirty; //Set the dirty bit for the new page
            }
        } else {
            access_time[page_index] = current_time; //Update access time

            //Update the dirty bit 
            if (dirty_bits[page_index] == 0 && current_dirty == 1) {
                dirty_bits[page_index] = 1;
            }
        }

        current_time++;
    }

    printf("| %-6d | %-12d | %-11d |\n", frame_count, page_faults, writeBacks);

    free(frames);
    free(access_time);
    free(dirty_bits);
}

int mainLRU(char *inputFile) {
    FILE *file;
    char line[256];
    Page *listOfPages = NULL; // Pointer for dynamic allocation
    int pageCount = 0;

    // Open CSV file for reading
    file = fopen(inputFile, "r");
    if (file == NULL) {
        perror("File not found, make sure input file is named: inputfile.txt");
        return 1;
    }

    // Count the number of lines with valid data in the file
    while (fgets(line, sizeof(line), file)) {
        // Ignore empty lines
        if (line[0] != '\n' && strlen(line) > 1) { // Check if the line is not empty and not just a newline
            pageCount++;
        }
    }

    // Allocate memory for the pages
    listOfPages = malloc(pageCount * sizeof(Page));
    if (listOfPages == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    // Rewind the file to read the data again
    rewind(file);

    // Read and ignore the header line, if present
    fgets(line, sizeof(line), file); // Skip the header line

    // Read each line of the CSV file
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        // Ignore empty lines
        if (line[0] == '\n' || strlen(line) == 0) continue; // Improved empty line check

        if (sscanf(line, "%d,%d", &listOfPages[index].page_number, &listOfPages[index].dirty) == 2) {
            index++; // Increment the page index
        }
    }

    fclose(file); // Close the file after reading

    printf("+--------+--------------+-------------+\n");
    printf("| Frames | Page Faults  | Write backs |\n");
    printf("+--------+--------------+-------------+\n");

    //Call LRU function for frame sizes 1 to 100
    for (int i = 1; i < 101; i++) {
        LRU(listOfPages, index, i);
        printf("+--------+--------------+-------------+\n");

    }


    // Free the dynamically allocated memory
    free(listOfPages);

    return 0;
}
                                                                                                                                                                           
