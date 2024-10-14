#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_PAGES 500

// Structure to hold page information
typedef struct {
    int page_number;
    int dirty; // 0 or 1
} Page;

// Function to find if the page is present in the frame
bool isPagePresent(Page frames[], int frame_count, Page page) {
    for (int i = 0; i < frame_count; i++) {
        if (frames[i].page_number == page.page_number) {  
            return true;
        }
    }
    return false;
}

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
    printf("| %-6d | %-12d | %-11d |\n",frame_count,page_faults-1,writeBacks);    // Write backs not implemented yet.

    
    // print the last line
    printf("+--------+--------------+-------------+\n");
       
    
    //free(frames); // Free the frame memory
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
    printf("| %-6d | %-12d | %-11d |\n", frame_count, page_faults-1, writeBacks);
    
    // print the last line
    printf("+--------+--------------+-------------+\n");
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

    printf("| %-6d | %-12d | %-11d |\n", frame_count, page_faults-1, writeBacks);

    free(frames);
    free(access_time);
    free(dirty_bits);
}

// Main function
int main(int argc, char *argv[]) {
    // Check if the user has provided the correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Error: Please provide 1 argument (page replacement algorithm).\n");
        return EXIT_FAILURE;
    }

    char buffer[256];
    
    // Count the number of lines in the input
    int lineCount = 0;
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        lineCount++;
    }

    // Rewind the file pointer to read the data again
    rewind(stdin);

    // Allocate memory for the pages
    Page *pages = malloc(lineCount * sizeof(Page));
    if (pages == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    // Read and store each Page
    int i = 0;
    while (fgets(buffer, sizeof(buffer), stdin) != NULL && i < lineCount) {
        sscanf(buffer, "%d,%d", &pages[i].page_number, &pages[i].dirty);
        i++;
    }

    // Print the header for output
    printf("+--------+--------------+-------------+\n");
    printf("| Frames | Page Faults  | Write backs |\n");
    printf("+--------+--------------+-------------+\n");

    // Process using FIFO if it is the selected scheduler
    if (strcmp(argv[1], "FIFO") == 0) {
        for (int i = 1; i < 101; i++) {
            FIFO(pages, lineCount, i);
        }
    } else if (strcmp(argv[1], "OPT") == 0) {
        for (int i = 1; i < 101; i++) {
            Optimal(pages, lineCount, i);
        }
    } else if (strcmp(argv[1], "LRU") == 0) {
        for (int i = 1; i < 101; i++) {
            LRU(pages, lineCount, i);
        }
    } else {
        fprintf(stderr, "Error: Invalid page replacement algorithm specified.\n");
        free(pages);
        return EXIT_FAILURE;
    }

    // Free the memory allocated for the pages
    free(pages);

    return EXIT_SUCCESS;
}
