#include "WhoHeader.h"
#include "NetworkHeader.h"
#include "getLengthField.c"

/* Classwide constants */
#define MAXIMUM_DATABASE_ENTRY_LENGTH 158
#define SONG_LENGTH 5
#define SHA_LENGTH 128
#define MAX_NUM_RECORDS 500

/* Classwide variables */
static FILE* filePointer;
static char* songList[MAX_NUM_RECORDS]; // list of song:sha pairings in local database
static int numEntries; // number of entries in local database



/****************************************************************************************************************
* Opens the local database where song:SHA pairings are stored and stores them in the classwide array.
* Parameter is the filename of the local database. If the filename is not found an error message is displayed
* and the program exits execution.
****************************************************************************************************************/
void open_database(char* filename) 
{
  if ( (filePointer = fopen(filename,"r+")) == NULL ) {
    fprintf(stderr, "Error: Can't open file %s\n", filename);
    exit(1);
  }

  intializeSongList();
}


/****************************************************************************************************************
* Closes the local database where song:SHA pairings are stored and frees associated memory and variables. 
****************************************************************************************************************/
void close_database() 
{
  fclose(filePointer);
  int i;
  for (i = 0; i < numEntries; i++)
  {
    free(songList[i]);
  }
  numEntries = 0;
}


void intializeSongList()
{
  char* currentLine = (char *) malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  // for holding song name when going through the database
  char* firstField = malloc(SONG_LENGTH + 1);

  // for holding a song's SHA when going through the database
  char* secondField = malloc(SHA_LENGTH + 1);

  // intialize the number of entries
  numEntries = 0;

  // loop through the entries on each line of the database until null is encountered
  while (fgets(currentLine, MAXIMUM_DATABASE_ENTRY_LENGTH, filePointer) !=  NULL ) 
  { 
      // retrieve the name of the song at the current line
      firstField = strtok(currentLine, ":");

      // retrieve the SHA of the song at the current line and null terminate
      secondField = strtok(NULL, ":");
      secondField[SHA_LENGTH] = '\0';

      // allocate space for a song and SHA entry in the classwide variable
      songList[numEntries] = (char *) malloc(SONG_LENGTH + SHA_LENGTH + 2); 

      // add a song and its SHA to the list of entries in the classwide variable
      sprintf(songList[(numEntries)++],"%s:%s", firstField, secondField);

  }

}


/****************************************************************************************************************
* Takes as a parameter a location in which to store the number of files in the database.
*
* Returns an array containing the song names in the database and their corresponding SHAs
****************************************************************************************************************/
char** lookup_songs (int* no_of_entries) 
{
  // specify the number of songs in the list into the passed in variable
  *no_of_entries = numEntries;

  // return the location to where the contents of the local database where stored
  return songList;
}




/****************************************************************************************************************
* Takes filename and SHA value as parameters and determines if the database contains that file based on SHA value
* and returns 1 if true and 0 if false.
****************************************************************************************************************/
int containsSong(char* comparedSha) 
{
  // for holding the current song:SHA pairing to be broken apart by strtok()
  char* currentLine = (char *)  malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  // for holding the current SHA retrieved by breaking apart the song:SHA pairing with strtok()
  char* shaField = malloc(SHA_LENGTH + 1);

  // loop through the list of song:SHA pairings in the local database
  int i;
  for (i = 0; i < numEntries; i++) 
  {
    // get current song and sha into a temporary variable
    strcpy(currentLine, songList[i]);

    // break the current line along the ':' to prepare to get the SHA
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



/****************************************************************************************************************
* Given an SHA returns a song name based on SHA to song name mappings in the local database. 
****************************************************************************************************************/
char* getSongName(char* SHA)
{
  char* currentLine = (char *) malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  char* nameField = malloc(SONG_LENGTH);

  char* shaField = malloc(SHA_LENGTH + 1);

  // loop through the list of song:SHA pairings in the local database
  int i;
  for (i = 0; i < numEntries; i++) 
  {
    // get current song and sha into a temporary variable
    strcpy(currentLine, songList[i]);

    // break the current line along the ':' to prepare to get the SHA
    nameField = strtok(currentLine, ":"); 

    // get the second field of the current line
    shaField = strtok(NULL, ":"); 

    if(strcmp(SHA, shaField) == 0) // if passed in SHA equals current SHA
    {
      return nameField; // return the name of the song
    }
    
  }


  return "";
}


/****************************************************************************************************************
* Takes song name and SHA as a parameter. Adds the song and SHA to the local database and array containings the 
* entries in that database.
* 
* Note: Assumes a new line is at the end of the file so if a non-newline character exists at the end it will 
* result in a combined string at the place of the addition.
****************************************************************************************************************/
void addSong(char* songName, char* SHA) 
{
  FILE* append = filePointer;
  fseek(append, 0, SEEK_END); // get to the end of the file

  fprintf(append, "%s:%s\n", songName, SHA); // add to end of file
  fclose(append); // close the file pointer

  // allocate space for a song and SHA entry in the classwide array
  songList[numEntries] = (char *) malloc(SONG_LENGTH + SHA_LENGTH + 2); 

  // add a song and its SHA to the list of entries in the classwide variable
  sprintf(songList[numEntries], "%s:%s", songName, SHA);
  numEntries++;
}





/**************************************************************************************************************** 
* Takes as a parameter the list of songs:SHA pairings to compare to our local database contents.
*
* Returns a list of songs:SHA pairings of files not stored on our local database based on SHA values. 
****************************************************************************************************************/
char* compareSongsToClient(char* inputBuffer, int numBufferEntries) 
{
  // allocate a variable for names in the buffer
  char* name = malloc(SONG_LENGTH);
  char* sha = malloc(SHA_LENGTH + 1);

  // string to be returned containing songs
  char* result = malloc(BUFFSIZE);

  // pointer to a location in 'result' being added to 
  char* ptr = result;

  // move pointer past location that will store the length field
  ptr += 2;

  // number of songs currently in the array being returned
  int numEntries = 0; 

  // go through the entire input buffer
  int i;
  for (i = 0; i < numBufferEntries; i++)
  {
      // retrieve the current song
      strncpy(name, inputBuffer, SONG_LENGTH);

      // move pointer past current song to start of sha field
      inputBuffer += SONG_LENGTH;

      // retrieve the current sha
      strncpy(sha, inputBuffer, SHA_LENGTH);

      // move pointer past current sha to start of next song field
      inputBuffer += SHA_LENGTH;

      // if the song is not found in the database, add it to output buffer
      if(containsSong(sha) == 0) 
      {
        // add song name to result to be returned
        strncpy(ptr, name, SONG_LENGTH);

        // move pointer past song and null character to place sha is to be added
        ptr += SONG_LENGTH;

        // add sha to result to be returned
        strncpy(ptr, sha, SHA_LENGTH);

        numEntries++;
      }
  }

  // null terminate the result
  strcat(result, "\0");


  // add the length field to the first 2 bytes of result
  convertLengthTo2Bytes(result, numEntries);


  return result;
}





/**************************************************************************************************************** 
* Takes as a parameter the list of songs:SHA pairings to compare to our local database contents.
*
* Returns a list of songs:SHA pairings of files not stored on the server database based on SHA values. 
****************************************************************************************************************/
char* compareSongsToServer(char* inputBuffer, int numBufferEntries) 
{
  char* currentLine = (char*) malloc(SONG_LENGTH + SHA_LENGTH + 1);

  // allocate a variable for names in the buffer
  char* name = malloc(SONG_LENGTH);
  char* sha = malloc(SHA_LENGTH + 1);

  // string to be returned containing songs
  char* result = malloc(BUFFSIZE);

  // pointer to a location in 'result' being added to 
  char* ptr = result;

  // move pointer past location that will store the length field
  ptr += 2;

  // number of songs currently in the array being returned
  int numEntries = 0; 

  // go through the entire song list
  int i;
  for (i = 0; i < numEntries; i++)
  {

      // retrieve the current song and sha
      strcpy(currentLine, songList[i]);

      // get the name of the song only
      name = strtok(currentLine, ":");

      // retrieve the SHA of the song
      sha = strtok(NULL, ":");

      // if the song is not found in the database, add it to output buffer
      if(listContainsSong(sha, inputBuffer, numBufferEntries) == 0) 
      {
        // add song name : SHA to result to be returned
        strcat(result, name); 
        strcat(result, ":");
        strcat(result, sha);
      }
  }

  // null terminate the result
  strcat(result, "\0");

  return result;
}


/****************************************************************************************************************
* Takes filename and SHA value as parameters and determines if the database contains that file
*  and returns 1 if true and 0 if false. 
****************************************************************************************************************/
int listContainsSong(char* comparedSha, char* inputBuffer, int numBufferEntries)
{
  // for holding the current SHA retrieved by breaking apart the song:SHA pairing with strtok()
  char* shaField = malloc(SHA_LENGTH);

  // move pointer past first song to start of sha
  inputBuffer += SONG_LENGTH;

  // loop through the list of song:SHA pairings in the local database
  int i;
  for (i = 0; i < numBufferEntries; i++) 
  {
    // get current song and sha into a temporary variable
    strncpy(shaField, inputBuffer, SHA_LENGTH);

    // move pointer past current sha and past next song name to start of next sha
    inputBuffer += SHA_LENGTH + SONG_LENGTH;

    if(strcmp(comparedSha, shaField) == 0) // if passed in SHA equals current SHA
    {
      return 1; // return true
    }
  }

  return 0; // return false
}


/****************************************************************************************************************
* Takes in song name find out in database contains song of the same name.
****************************************************************************************************************/
int fileExists(char* fileName)
{
  // for holding the current song:SHA pairing to be broken apart by strtok()
  char* currentLine = (char *)  malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  // for holding the current SHA retrieved by breaking apart the song:SHA pairing with strtok()
  char* songName = malloc(SONG_LENGTH + 1);

  // loop through the list of song:SHA pairings in the local database
  int i;
  for (i = 0; i < numEntries; i++) 
  {
    // get current song and sha into a temporary variable
    strcpy(currentLine, songList[i]);

    // break the current line along the ':' to prepare to get the song
    songName = strtok(currentLine, ":"); 

    songName[SONG_LENGTH] = '\0'; // null terminate

    if(strcmp(fileName, songName) == 0) // if passed in SHA equals current SHA
    {
      return 1; // return true
    }
  }

  return 0; // return false
}


//Sets the file pointer to a song given the specs of the song
void getSong(char* songName, char* song, int* numBytes)
{
  FILE* localPointer;
  if ( (localPointer = fopen(songName, "r+")) == NULL )
  {
    fprintf(stderr, "Error: Can't open file %s\n", songName);
    exit(1);
  }

  int i = 0;
  int c;
  do
  {
    c = getc (localPointer);
    song[i] = c;
    i++;
  } while (c != EOF);

  *numBytes = i;
}


//Stores a song given the song information
void storeSong(char* songName, char* song, int numBytes)
{
  FILE* localPointer;
  if ( (localPointer = fopen(songName, "w+")) == NULL )
  {
    fprintf(stderr, "Error: Can't open file %s\n", songName);
    exit(1);
  }

  int i;
  for (i = 0; i < numBytes; i++)
  {
    fputc(song[i], localPointer);
  }
}


main()
{
  printf("hello\n");
  char* ptr = malloc(5);
  convertLengthTo2Bytes(ptr, 5);

  printf("%d\n", ptr[1]);

  
  open_database("compare.dat");
  char* in =  "Name913221111111111111222222222222222222221222111111111111122222222222222222222N12221111111111111222222222222222222222222222222222226Name412221111111111111222222222222222222221222111111111111122222222222222222222N12221111111111111222222222222222222222222222222222221Name112221111111111111222222222222222222221222111111111111122222222222222222222N12221111111111111222222222222222222222222222222222226";
  char* coma = compareSongsToClient(in, 2);
  char* comb = compareSongsToServer(in, 2);

  int lena = getLength(coma);
  coma += 2;
  for(int i = 0; i < lena; i++)
  {
    printf("song %s\n", coma);
    coma += SONG_LENGTH;
    printf("sha %s\n", coma);
    coma += SHA_LENGTH;
  }
  //printf("a: %s\n", coma);

  int lenb = getLength(comb);
  comb += 2;
  printf("\nnext\n");
  for(int i = 0; i < lenb; i++)
  {
    printf("song %s\n", comb);
    comb += SONG_LENGTH;
    printf("sha %s\n", comb);
    comb += SHA_LENGTH;
  }
  close_database();
}
