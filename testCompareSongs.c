/* Hello World program */
#include "WhoHeader.h"
#include "NetworkHeader.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>  // For isspace

#define MAXIMUM_DATABASE_ENTRY_LENGTH 158
#define SONG_LENGTH 255
#define SHA_LENGTH 128
#define MAX_NUM_RECORDS 100

static FILE* filePointer;
static char* songList[MAX_NUM_RECORDS];
static char** nullPtr = NULL;
static int numEntries;

char** lookup_songs (int* no_of_entries);

void getFields(char* line, char* songLocation, char* shaLocation)
{
  songLocation = strtok(line, ":");
  shaLocation = strtok(NULL, ":");
}

void open_database ( char *filename) {
  if ( (filePointer = fopen(filename,"r+")) == NULL ) {
    fprintf(stderr, "Error: Can't open file %s\n", filename);
    exit(1);
  }

  lookup_songs(&numEntries);


}

void printSongs(char* list[MAX_NUM_RECORDS], int num)
{
  char* song = malloc(SONG_LENGTH + 1);
  char* sha = malloc(SHA_LENGTH + 1);

 /* for(int i = 0; i < num; i++)
  {
    song = strtok(list[i], ":");
    sha = strtok(NULL, ":");
    printf("Song : %s\tSHA : %s\n", song, sha);
  }*/
  for(int i = 0; i < num; i++)
  {

    printf("Song : %s\n", list[i]);
  }
}

void close_database() 
{
  fclose(filePointer);
  for (int i = 0; i < numEntries; i++)
  {
    free(songList[i]);
  }
  //free(songList);
  numEntries = 0;
}



/**
* Takes as a parameter a location in which to store the number of files in the database.
*
* Returns an array containing the song names in the database and their corresponding SHAs
**/
char** lookup_songs (int* no_of_entries) 
{
  //printf("line 41\n");
  char* currentLine = (char *) malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  // for holding song name when going through the database
  char* firstField = malloc(SONG_LENGTH + 1);

  // for holding song SHA when going through the database
  char* secondField = malloc(SHA_LENGTH + 1);

  // intialize the number of entries
  *no_of_entries = 0;

  // loop through the entries on each line of the database until null is encountered
  while ( fgets(currentLine, MAXIMUM_DATABASE_ENTRY_LENGTH, filePointer) !=  NULL ) 
  { 
      // retrieve the name of the song
      firstField = strtok(currentLine, ":");

      // retrieve the SHA of the song
      secondField = strtok(NULL, ":");

      secondField[SHA_LENGTH] = '\0';

      // allocate space for a song and SHA entry
      songList[*no_of_entries] = (char *) malloc(SONG_LENGTH + SHA_LENGTH + 2); 

      // add a song and its SHA to the list of entries in the array
      sprintf(songList[(*no_of_entries)++],"%s:%s", firstField, secondField);

  }

  return songList;
}




/*
* Takes filename and SHA value as parameters and determines if the local database contains that file
* and returns 1 if true and 0 if false.
**/
int containsSong(char* comparedSha) 
{
  char* currentLine = (char *)  malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);
  //char* currentLine = malloc(SHA_LENGTH + SONG_LENGTH + 1);
  char* shaField = malloc(SHA_LENGTH + 1);


  for (int i = 0; i < numEntries; i++) 
  {
    // get current song and sha
    strcpy(currentLine, songList[i]);

    // break the current line to prepare to get the sha
    strtok(currentLine, ":"); 

    shaField = strtok(0, ":"); // get the second field of the current line

    shaField[SHA_LENGTH] = '\0'; // turn new line character to null 

    if(strcmp(comparedSha, shaField) == 0) // if passed in SHA equals current SHA
    {
      return 1; // return true
    }
  }


  return 0; // return false
}


/**
* Takes as a parameter the list of songs/SHAs to compare to our local database contents.
*
* Returns a list of songs/SHAs of files not stored on our local database.
**/
char* compareSongs(char** inputBuffer, int numEntries) 
{
  // allocate a variable for names in the buffer
  char* name = malloc(SONG_LENGTH);
  char* sha = malloc(SHA_LENGTH + 1);

  // string to be returned containing songs
  char* result = malloc(BUFFSIZE);

  char* currentLine = malloc(SONG_LENGTH+ SHA_LENGTH + 1);

  // go through the entire input buffer
  for (int i = 0; i < numEntries; i++)
  {
      // retrieve the current song and sha
      strcpy(currentLine, inputBuffer[i]);

      // get the name of the song only
      name = strtok(currentLine, ":");

      // retrieve the SHA of the song
      sha = strtok(NULL, ":");

      // if the song is not found in the database, add it to output buffer
      if(containsSong(sha) == 0) 
      {
        // add song name : SHA to result to be returned
        strcat(result, name); 
        strcat(result, ":");
        strcat(result, sha);

        // strcat(result, "\n");
      }
  }

  // null terminate the buffer
  strcat(result, "\0");

  return result;

}


main()
{
  
  open_database("compare.dat");
  char* outerDatabase[MAX_NUM_RECORDS];

  for (int i = 0; i < numEntries; i++)
  {
    outerDatabase[i] = malloc(MAXIMUM_DATABASE_ENTRY_LENGTH  + 1);
    strcpy(outerDatabase[i], songList[i]);
    //printf("%s\n", outerDatabase[i]);
  } 
  int numOuter = numEntries;

  close_database();
  open_database("database.dat");

  char* result  = compareSongs(outerDatabase, numOuter);

  printf("\nRESULTS: \n\n%s\n", result);



}