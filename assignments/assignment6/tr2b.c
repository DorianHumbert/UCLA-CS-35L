#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void checkInputErr(); // check for any reading errors
void checkOutputErr(); // check for any writing errors
void transliterate(char* from, char* to); // perform transliteration

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    fprintf(stderr, "Error: Please provide two sets of characters and input from standard input.");
    exit(1);
  }
  transliterate(argv[1], argv[2]);
  exit(0);
}

// check for any reading errors
void checkInputErr()
{
  if (ferror(stdin))
  {
    fprintf(stderr, "Error: character cannot be read in.");
    exit(1);
  }
}

// check for any writing errors
void checkOutputErr()
{
  if (ferror(stdout))
  {
    fprintf(stderr, "Error: character cannot be written.");
    exit(1);
  }
}

// perform transliteration
void transliterate(char* from, char* to)
{
  // check that from and to are of equal lengths
  if (strlen(from) != strlen(to))
  {
    fprintf(stderr, "Error: Transliteration sets must be of the same length.");
    exit(1);
  }
  // check that from has no duplicates
  size_t length = strlen(from);
  for (size_t i=0; i<length; i++)
  {
    for (size_t j=i+1; j<length; j++)
    {
      if (from[i] == from[j])
        {
          fprintf(stderr, "Error: From set cannot contain duplicates.");
          exit(1);
        }
    }
  }    
  // perform transliteration
  int c;
  c = getchar();
  while (c != EOF)
  {
    checkInputErr();
    for (size_t i=0; i<length; ++i) // iterate through from and to
    {
      if (from[i] == c) // check if c is in from
      {
        c = (to[i]); // alter c accordingly
        break;
      }
    }
    putchar(c);
    checkOutputErr();
    c = getchar();
  }
  return;
}
