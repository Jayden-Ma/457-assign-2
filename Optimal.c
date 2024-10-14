#include <stdio.h>
#include <stdlib.h>
#include "pageReplacement.h"

// Function to find the page that will not be used for the longest time
int findOptimalPageToReplace(Page frames[], int frame_count, Page pages[], int current_page_index, int total_pages) {
    int farthest = current_page_index;
    int page_to_replace = -1;

    for (int i = 0; i < frame_count; i++) {
        int j;
        for (j = current_page_index; j < total_pages; j++) {
            if (frames[i].page_number == pages[j].page_number) {
                // If the page is found in future references, break
                if (j > farthest) {
                    farthest = j;
                    page_to_replace = i; // Update the page to replace
                }
                break;
            }
        }
        // If the page is never found, replace it immediately
        if (j == total_pages) {
            return i; // The frame is eligible for replacement
        }
    }
    return page_to_replace; // Return the frame to replace
}

// Optimal Page Replacement Algorithm
void Optimal(Page pages[], int count, int frame_count) {
    Page frames[frame_count];       // Array of frames with size of frame_count 
    int page_faults = 0;  // Count of page faults
    int writeBacks = 0;
    
    // Initialize frames
    for (int i = 0; i < frame_count; i++) {
        frames[i].page_number = -1; // fill the frame with empty pages
        frames[i].dirty = 0; // fill the frame with empty pages
    }

    for (int i = 0; i < count; i++) {
        Page current_page = pages[i];

        // Check if the current page is already in the frames
        if (!isPagePresent(frames, frame_count, current_page)) {
            // Page fault occurs
            page_faults++;

            // Determine which page to replace using optimal algorithm
            int page_to_replace = findOptimalPageToReplace(frames, frame_count, pages, i, count);
            
            // Check if there is a need to write back a dirty page
            if (page_to_replace != -1 && frames[page_to_replace].dirty == 1) {
                writeBacks++;
            }

            // Replace the page in the frame
            frames[page_to_replace] = current_page;
        }  else {

            // if the page is present, but the dirty bit is different
            for (int j = 0; j < frame_count; j++) {
                if(frames[j].page_number == current_page.page_number){
                    if (frames[j].dirty == 0 && current_page.dirty ==1)    
                        frames[j].dirty = 1;
                        //writeBacks++;
                }

            
            }
        }

    }

    // Print results for this iteration
    printf("| %-6d | %-12d | %-11d |\n", frame_count, page_faults, writeBacks);
    
    // print the last line
    printf("+--------+--------------+-------------+\n");
}

int mainOPT(char *inputFile) {
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
        if (line[0] != '\n' && strlen(line) > 1) {
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
        if (line[0] == '\n' || strlen(line) == 0) continue; // Improved empty line check

        if (sscanf(line, "%d,%d", 
            &listOfPages[index].page_number, 
            &listOfPages[index].dirty) == 2) { // Ensure two values are read
            index++; // Increment the page index
        }
    }

    fclose(file); // Close the file after reading

    // Print header
    printf("+--------+--------------+-------------+\n");
    printf("| Frames | Page Faults  | Write backs |\n");
    printf("+--------+--------------+-------------+\n");
    
    // Call Optimal from frame size 1-100
    for (int i = 1; i < 101; i++) {
        Optimal(listOfPages, index, i);
    }

    // Free the dynamically allocated memory
    free(listOfPages);
    return 0;
}
