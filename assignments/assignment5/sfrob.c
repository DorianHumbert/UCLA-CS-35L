#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// takes two char pointers, terminated with a space byte
// lexigraphically compares them
int frobcmp(char const* a, char const* b) {
    // runs until either string reaches space
    for (; *a != ' ' && *b != ' '; ++a, ++b) {
        char deObfsA = (*a ^ 42); // de-obfuscate
        char deObfsB = (*b ^ 42);
        if (deObfsA < deObfsB)
            return -1;
        else if (deObfsA > deObfsB)
            return 1;
    }
    // deals with one string being a prefix of the other
    if (*a != ' ')
        return 1;
    else if (*b != ' ')
        return -1;
    return 0;
}

// elements of arrays are pointers to pointers
// hence, cast inputs to pointers to pointers and dereference
int gencmp(const void* a, const void* b) {
    return frobcmp(*(char const**)a, *(char const**)b);
}

// exit if error in reading from stdin
void inputErrProcess() {
    if (ferror(stdin))
    {
        fprintf(stderr, "Error processing additional input.");
        exit(1);
    }
}

// exit if error in writing to stdout
void outputErrProcess() {
    if (ferror(stdout))
    {
        fprintf(stderr, "Error outputting sorted frobnicated words.");
        exit(1);
    }
}

int main() {
    char** wordArray = (char**)malloc(sizeof(char*)); // holds multi-level array
    char* word = (char*)malloc(sizeof(char)); // holds one input word at a time
    if (word == NULL || wordArray == NULL) { // check if malloc call was successful
        fprintf(stderr, "Error: Additional memory cannot be allocated.");
        exit(1);
    }
    int c = getchar(); // read from stdin
    inputErrProcess();
    int wordCount = 1, charCount = 1; // initialize counters of words and characters
    if (c == EOF) {
        wordCount = 0;
        charCount = 0;
    }
    while (c != EOF) { // add all chars to words and words to tbe array
        word[charCount - 1] = c; // adds char to word
        if (c == ' ') { // if previous char added was space
            c = getchar();
            inputErrProcess();
            wordArray[wordCount - 1] = word; // point row of array to word
            if (c == EOF) // if next char is EOF, do not allocate new row
                break;
            wordCount++;
            char** wordArrayTemp = (char**)realloc(wordArray, wordCount*sizeof(char*));
            if (wordArrayTemp == NULL) { // check for invalid reallocated memory
                fprintf(stderr, "Error: Additional memory cannot be allocated.");
                exit(1);
            }
            wordArray = wordArrayTemp;
            char* wordTemp = (char*)malloc(sizeof(char)); // point word at new byte in memory
            if (wordTemp == NULL) {
                fprintf(stderr, "Error: Additional memory cannot be allocated.");
                exit(1);
            }
            word = wordTemp;
            charCount = 1; // reset char count
        }
        else { // if char was not a space
            c = getchar(); // get next char
            inputErrProcess();
            charCount++;
            if (c == EOF) { // if next char is EOF, add space to word
                char* wordTemp = (char*)realloc(word, charCount*sizeof(char));
                if (wordTemp == NULL) {
                    fprintf(stderr, "Error: Additional memory cannot be allocated.");
                    exit(1);
                }
                word = wordTemp;
                word[charCount - 1] = ' '; // add last space to word
                charCount++;
                wordArray[wordCount - 1] = word; // add last word to word array
                break;
            }
            char* wordTemp = (char*)realloc(word, charCount*sizeof(char));
            if (wordTemp == NULL) {
                fprintf(stderr, "Error: Additional memory cannot be allocated.");
                exit(1);
            }
            word = wordTemp;
        }
    }

    // sort array of words based on gencmp
    qsort(wordArray, wordCount, sizeof(char*), gencmp);

    // free all pointers
    for (size_t i=0; i<wordCount; i++) {
        for (size_t j=0; ; j++) {
            putchar(wordArray[i][j]);
            outputErrProcess();

            if (wordArray[i][j] == ' ')
                break;
        }
        free(wordArray[i]);
    }
	free(wordArray);
    exit(0);
}


