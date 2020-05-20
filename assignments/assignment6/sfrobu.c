#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

int frobcmp(char const* a, char const* b); // lexigraphically compare two strings
int gencmp(const void* a, const void* b); // cast inputs and dereference
void checkInputErr(ssize_t result); // check for any reading errors
void checkOutputErr(ssize_t result); // check for any writing errors

int fOption = 0; // keep track of if [-f] option is passed

int main(int argc, char** argv) {

    if (argc == 2){
        char flag[] = "-f";
        if (strcmp(argv[1], flag) != 0) {
            fprintf(stderr, "Error: only the [-f] option is valid.");
            exit(1);
        }
        fOption = 1;
    }
    else {
        if (argc != 1) {
            fprintf(stderr, "Error: Please provide zero arguments, or the [-f] option.");
            exit(1);
        }
    }

    char* buf; // updated buffer
    size_t bufSize; //  buffer size
    // check for input file
    struct stat inputFile;
    if (fstat(0, &inputFile) < 0) {
        fprintf(stderr, "Error: input file cannot be read.");
        exit(1);
    }
    // determine initial buffer size based on input type
    if (S_ISREG(inputFile.st_mode))
        bufSize = ((inputFile.st_size + 100)*sizeof(char));
    else 
        bufSize = 8192; // 8 KiB
    // allocate memory for buffer
    char* bufTemp = (char*)malloc(bufSize);
    if (bufTemp == NULL) {
        fprintf(stderr, "Error: additional memory cannot be allocated.");
        exit(1);
    }
    buf = bufTemp;
    size_t bytesRead = 0; // counter of number of bytes read
    ssize_t c; // store result of read
    do { // read into buf, double size of buf when over half full
        if (bytesRead > bufSize/2) {
            bufSize <<= 1;
            char* bufTemp = (char*)realloc(buf, bufSize);
            if (bufTemp == NULL) {
                fprintf(stderr, "Error: additional memory cannot be allocated.");
                exit(1);
            }
            buf = bufTemp;
        }
        size_t readSize = bufSize - bytesRead; // max # of bytes allowed to be read into buffer
        c = read(0, &buf[bytesRead], readSize); // read bytes, starting at an empty position
        checkInputErr(c);
        bytesRead += c;
    } while (c != 0);

    char** wordArray = (char**)malloc(sizeof(char*)); // holds multi-level array
    char* word = (char*)malloc(sizeof(char)); // holds one input word at a time
    if (word == NULL || wordArray == NULL) { // check if malloc call was successful
        fprintf(stderr, "Error: Additional memory cannot be allocated.");
        exit(1);
    }

    size_t d = 0;
    int wordCount = 1, charCount = 1; // initialize counters of words and characters
    if (d == bytesRead) {
        wordCount = 0;
        charCount = 0;
    }
    // add all chars to words and words to tbe array
    while (d < bytesRead) {
        word[charCount - 1] = buf[d]; // adds char to word
        if (buf[d] == ' ') { // if previous char added was space
            d++;
            wordArray[wordCount - 1] = word; // point row of array to word
            if (d == bytesRead) // if last char, then do not allocate new row
                break;
            wordCount++;
            char** wordArrayTemp = (char**)realloc(wordArray, wordCount*sizeof(char*));
            if (wordArrayTemp == NULL) {
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
            d++; // get index of next char
            charCount++;
            if (d == bytesRead) { // if last char of word was not space, add space
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
            // allocate memory for a new char in word
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

    ssize_t o; // to store write output
    // write and free all dynamically allocated memory
    for (size_t i=0; i<wordCount; i++) {
        for (size_t j=0; ; j++) {
            o = write(1, &wordArray[i][j], 1); // note: unbuffered write calls
            checkOutputErr(o);
            if (wordArray[i][j] == ' ')
                break;
        }
        free(wordArray[i]);
    }
	free(wordArray);
	free(buf); // free original input buffer
    exit(0); // exit with a success status
}

// takes two char pointers, terminated with a space byte
// lexigraphically compares them
int frobcmp(char const* a, char const* b) {
    // runs until either string reaches space
    for (; *a != ' ' && *b != ' '; ++a, ++b) {
        char deObfsA = (*a ^ 42); // de-obfuscate
        char deObfsB = (*b ^ 42);
        if (fOption == 1) {
            deObfsA = toupper((unsigned char)deObfsA);
            deObfsB = toupper((unsigned char)deObfsB);
        }
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

// check for any reading errors with read()
void checkInputErr(ssize_t result) {
  if (result == -1) {
    fprintf(stderr, "Error: character cannot be read in.");
    exit(1);
  }
}

// check for any writing errors with write()
void checkOutputErr(ssize_t result) {
  if (result == -1) {
    fprintf(stderr, "Error: character cannot be written.");
    exit(1);
  }
}
