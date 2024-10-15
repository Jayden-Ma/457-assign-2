#include <stdio.h>
#include <stdlib.h>
#include "pageReplacement.h"

// FIFO Page Replacement Algorithm
// signature contains: a list of pages read from the input file, counter that counts the number of pages, frame count for number of frames available 
void FIFO(Page pages[], int count, int frame_count) {
    Page frames[frame_count];       // creates array of frames with size of frame_count 
    int frame_index = 0;  // Index for the FIFO replacement
    int page_faults = 0;  // Count of page faults
    int writeBacks = 0;

    Page emptyPage;
    emptyPage.page_number = -1;
    emptyPage.dirty = 0;

    // Initialize frames
    for (int i = 0; i < frame_count; i++) {
        frames[i] = emptyPage; // fill the frame with empty pages
    }

    for (int i = 0; i < count; i++) {
        Page current_page = pages[i];

        // Check if the current page is already in the frames
        if (!isPagePresent(frames, frame_count, current_page)) {
            // Page fault occurs
            page_faults++;

            // if a dirty page is evicted from memory, add one to writeBacks
            if(frames[frame_index].page_number != -1 && frames[frame_index].dirty == 1){
                writeBacks++;       
            }

            // Replace the page using FIFO method
            frames[frame_index] = current_page;
            frame_index = (frame_index + 1) % frame_count; // Move to the next frame in a circular manner

        } else {

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
    printf("| %-6d | %-12d | %-11d |\n",frame_count,page_faults,writeBacks);    // Write backs not implemented yet.

    
    // print the last line
    printf("+--------+--------------+-------------+\n");
       
    
    //free(frames); // Free the frame memory
}

int mainFIFO(char *inputFile) {
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
    // call FIFO from frame size 0-100
    for (int i = 1; i < 101; i++){
        FIFO(listOfPages,index,i);
    }


    // Free the dynamically allocated memory
    free(listOfPages);

    return 0;
}
