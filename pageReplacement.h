// header file

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define PAGE_REPLACEMENT_H  // Define the macro
#define MAX_FRAMES 50

typedef struct {
    int page_number;
    unsigned int ref_register;  //Reference register of n bits
    int dirty;                  
} Page;

Page *frames;
int num_frames = MAX_FRAMES;
int write_back_count = 0;  //Count the number of write backs

// Function to find if the page is present in the frame
bool isPagePresent(Page frames[], int frame_count, Page page) {
    for (int i = 0; i < frame_count; i++) {
        if (frames[i].page_number == page.page_number) {  
            return true;
        }
    }
    return false;
}
