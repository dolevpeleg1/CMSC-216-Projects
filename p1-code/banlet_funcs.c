// banlet_funcs.c: support functions for the banlet_main program.

#include "banlet.h"

// PROBLEM 1: Replace instances of character 'old' with 'new' in the
// string 'str'.  May use the strlen() library function to determine
// string length or directly look for a '\0' null termination
// character to end the string.
// 
// EXAMPLES:
// char s1[]="A A B B A"; string_replace_char(s1, 'A', 'X'); // s1 is "X X B B X"
// char s2[]="A A B B A"; string_replace_char(s2, 'B', 'Y'); // s2 is "A A Y Y A"
// char s3[]="A A B B A"; string_replace_char(s3, ' ', '-'); // s3 is "A-A-B-B-A"
void string_replace_char(char *str, char old, char new){
  // WRITE ME
  int len = strlen(str);          // length of the string
  for(int i = 0; i < len; i++){
    if(str[i] == old)             // old char matches the one replaced
    {
      str[i] = new;               // old char replaced with the new one
    }
  }
}

// PROBLEM 1: Counts the number of newline characters '\n' in the
// string 'msg'; return this number +1 as the end of lines will always
// be a line break. May use the strlen() library function to determine
// string length or directly look for a '\0' null termination
// character to end the string.
// 
// EXAMPLES:
// count_linebreaks("Hi")        ->  1
// count_linebreaks("Hi There")  ->  1
// count_linebreaks("Hi\nThere") ->  2
// count_linebreaks("O\nM\nG")   ->  3
int count_linebreaks(char *msg){
  // WRITE ME
  int len = strlen(msg);          // length of the string
  int count = 0;                  // count of new lines
  for(int i = 0; i < len; i++){
    if(msg[i] == '\n')            // char at location is a new line
    {
      count++;                    // increment the count
    }
  }
  return count + 1;
}

// PROBLEM 1: Counts the linebreaks (newline '\n' chars and end of
// string) and returns an array of integers with the position for each
// linebreak in string 'msg'.  Uses the count_linebreaks()
// function. The 'nbreaks' parameter is an OUTPUT integer that should
// be set to the number of breaks in 'msg' using the C dereference
// operator (*).
//
// EXAMPLES:
// int nbreaks = -1;
// int *breaks = find_linebreaks("Hello\nWorld", &nbreaks);
// //            index in string: 012345 67890
// // nbreaks is now 2
// // breask is now [5, 11]
int *find_linebreaks(char *msg, int *nbreaks){
  // WRITE ME
  *nbreaks = count_linebreaks(msg); // amount of linebreaks
  int *breaks_array = malloc(sizeof(int) * (*nbreaks)); // allocating memory for the array
  int breaks_index = 0; // index of the current element added to the breaks array
  int len = strlen(msg);  // length of the string

  for(int i = 0; i < len; i++){
    if(msg[i] == '\n')  // char at location is a new line
    {
      breaks_array[breaks_index] = i; // add the current index to the array
      breaks_index++; // increment the breaks index
    }
  }

  breaks_array[*nbreaks - 1] = len; // add the last element + 1 of the array to the breaks array

  return breaks_array;
}

// PROBLEM 1: Prints string 'msg' using 'font'. Only prints characters
// 0 to 'length-1'.  Iterates over each row in font->height and then
// scans across the charactes in 'msg' printing each "row" of the
// character. On reaching index 'length', prints a newline and then
// scans across 'msg' again printing characters from the next row.
// Each msg[i] character is used to as the index into fonts->glyphs[]
// to access the "glyph" that will be printed.
//
// NOTE: This function does NOT handle embedded newline '\n'
// characters. It is intended to be called repeatedly on each line in
// multiline text with '\n' characters found using the
// 'find_linebreaks()' function.
//
// EXAMPLE:
//
// print_fontified_oneline("Hello!", &font_standard, 6);
// // Prints the following on standard output:
// ._. ._.      ,-,,-,       ._.
// | | | |  ___ | || |  ___  | |
// | |_| | /'_`\| || | / _ \ | |
// |  _  ||  __/| || || (_) ||_|
// |_| |_| \___||_||_| \___/ (_)
void print_fontified_oneline(char *msg, font_t *font, int length){
  // WRITE ME
  int len = strlen(msg);  // length of the string
  for (int i = 0; i < font->height; i++){  // iterate through the hight of the font
    for (int j = 0; j < length; j++) // iterate through the msg to the specified length
    {
      printf("%s", font->glyphs[(int)msg[j]].data[i]); // print the current char
    }
    printf("\n");
  }
}

// PROBLEM 2: Uses print_fontified_oneline() with find_linebreaks() to
// correctly print 'msg' with 'font' even if there are linebreaks in
// 'msg'.  Uses find_linebreaks() to find the end of each line in
// 'msg' and then iterates over lines printing each.  Uses pointer
// arithmetic to pass the starting position of each line into calls of
// print_fontified_oneline(). Frees memory allocated (such as through
// use of count_linebreaks()) before returning.
//
// EXAMPLE:
//   print_fontified("apple\nB@N@N@\nCarr0^", &font_standard);
// Shows the following on standard output:
//                      ,-,      
//   __ _ ,_ __. ,_ __. | |  ___ 
//  / _` || '_ \ | '_ \ | | /'_`\
// | (_| || |_) || |_) || ||  __/
//  \__,_|| .__/ | .__/ |_| \___|
//        |_|    |_|             
// ,____    ____  ._  ._.   ____  ._  ._.   ____  
// | == )  / __ \ | \ | |  / __ \ | \ | |  / __ \ 
// |  _ \ / / _` ||  \| | / / _` ||  \| | / / _` |
// | |_)|| | (_| || |\  || | (_| || |\  || | (_| |
// |____/ \ \__,.||_| \_| \ \__,.||_| \_| \ \__,.|
//         \____/          \____/          \____/ 
//   ____.                     ___   /\ 
//  / ___|  __ _ ._ __.._ __. / _ \ |/\|
// | |     / _` || `__|| `__|| | | |    
// | |___.| (_| || |   | |   | |_| |    
//  \____| \__,_||_|   |_|    \___/     
void print_fontified(char *msg, font_t *font){
  // WRITE ME
  int nbreaks = count_linebreaks(msg); // number of breaks
  int *breaksIndex = find_linebreaks(msg, &nbreaks); // array of the breaks index
  
  
  for(int i = 0; i < nbreaks; i++){  // iterrate once for each break
    if(i == 0){ // if this is the first iteration, print the line
      print_fontified_oneline(msg, font, breaksIndex[i]);
    }
    else{ // else, start printing from the previous new line
      print_fontified_oneline(msg + breaksIndex[i - 1] + 1, font, breaksIndex[i] - breaksIndex[i - 1] - 1);
    }
    
  }
  free(breaksIndex); // free the arrat of breaks index
}


// PROVIDED: Initializes a glyph to mostly X's except for its
// codepoint on the first line.
void glyph_init(glyph_t *glyph, int codepoint){
  glyph->codepoint = codepoint;
  glyph->width = 6;
  for(int i=0; i<MAX_HEIGHT; i++){
    for(int j=0; j<MAX_WIDTH; j++){
      if(j == glyph->width){
        glyph->data[i][j] = '\0'; // null terminate
      }
      else{
        glyph->data[i][j] = 'X';
      }
    }
  }
  int len = sprintf((char *)glyph->data, "%d",codepoint); // add codepoint # to glyph
  glyph->data[0][len] = 'X';                              // remove null termination char
}        

// PROBLEM 2: Loads a banner font from 'filename'.  The format of font
// files is documented more thoroughly in the project specification
// but is generally comprised of a first line that indicate the height
// of each glyph in the font followed by a sequence of each glyph
// starting with its codepoint (ASCII index) and a grid of characters
// in it. To make parsing easier, the ? character is used to represent
// blank spaces in glyph shapes.
//
// EXAMPLE:
// height: 4
// 42
// ???
// \|/
// /|\
// ???
// 43
// ???
// _|_
// ?|?
// ???
//
// The two characters above are the codepoint 42 '*' and codepoint 43
// '+' with the ? symbols eventually being replaced by spaces during
// loading.
// 
// If 'filename' cannot be opened for reading, returns NULL.
//
// Memory allocation happens in two steps: (1) allocates memory for a
// font_t struct then (2) allocates memory for an array of glyph_t
// structs of length NUM_ASCII_GLYPHS (a constant defined in
// banlet.h). Sets the font->glyphs field to be the allocated array of
// glyphs.  Reads the font->height field from teh first line of the
// file.  Iterates over each element of the glyphs array and calls the
// glyph_init() function on it to populate it with default data.  Then
// proceeds to read glyphs from the file. Glyphs are read by reading
// the integer codepoint first which determins which element of the
// glyph array to read into.  Then a loop over the height of the font
// is used to read each row of the glyph into the
// glyph[codepoint]->data[row]; fscanf() with '%s' specifier is used
// for this.  Finally, the string_replace_char() function is used to
// replace all '?' characters with ' ' (space) characters in the glyph
// data. Sets the width of each glyph using the strlen() function on
// any balid row of the glyph data.
//
// Glyphs are read from 'filename' until an attempt to read a glyph's
// codepoint with fscanf() returns EOF (end of file). This causes the
// routine to return the allocated font_t data for use elsewhere.
font_t *font_load(char *filename){
  // WRITE ME
  font_t *font = malloc(sizeof(font_t));  // allocate memory for the font struct
  FILE *file_handle = fopen(filename, "r"); // open the file for reading
  font->glyphs = malloc(NUM_ASCII_GLYPHS * sizeof(glyph_t)); // allocate memory for the glyph array

  for (int i = 0; i < NUM_ASCII_GLYPHS; i++){ // initiate the glyph array
    glyph_init(&font->glyphs[i], i);
  }
  
  if (file_handle == NULL){ // Check if file is opened
  font_free(font);
    return NULL; // return null
  }
  else{
    fscanf(file_handle, "height: %d", &font->height); // get the height
    int codepoint;
    while (1){  // run once for every glyph
      int ret = fscanf(file_handle, "%d", &codepoint); // get the codepoint
      if(ret == EOF){ // if the return value is EOF, break the loop
        break;
      }
      else{
        font->glyphs[codepoint].codepoint = codepoint; // assign the codepoint

        for (int i = 0; i < font->height; i++) { // run once for each line of data
        fscanf(file_handle, "%s", font->glyphs[codepoint].data[i]); // get the data
        font->glyphs[codepoint].width = strlen(font->glyphs[codepoint].data[i]); // set the width
        string_replace_char(font->glyphs[codepoint].data[i], '?', ' '); // replace '?' with spaces 
        }
      }
    }
  }

  
  fclose(file_handle);  // close the file
  return font;
}

// PROBLEM 2: Frees the memory associated with 'font'.  First frees
// the glyph array, then frees the font itself. Hint: this funciton
// should be 2 lines long.
void font_free(font_t *font){
  free(font->glyphs);
  free(font);
}


// PROBLEM 2: Read all characters from the file 'filename' into a
// dynamcally allocated array. Terminates the array with the ''
// character to form a valid C string and return that string. If the
// file cannot be opened, return NULL.
// 
// CONSTRAINT: Does not use any functions that determine the size of a
// file a priori such as fstat() or fseek() or their kin.
//
// BASELINE: Uses a "2-Pass" solution. Counts the characters in file
// via calls to fscanf() or fgetc(), then returns to beginning of the
// file with rewind(), allocates an appropriately sized block of
// memory, then reads all characters into it in a second pass.
//
// OPTIONAL MAKEUP CREDIT: Uses a single I/O pass and dynamically
// expands memory to handle larger inputs. For full makeup credit,
// this approach must abide by the following additional conditions:
// - Does not use functions that determine the size of the file
// - Does not allocate/copy memory each loop iteration
// - Does not use the realloc() function, only malloc()/free()
// - Has amortized linear time and linear space; the allocated array may
//   not be any larger than 2x the number of characters in it
// - Uses a mmeory allocation scheme similar to thos used with
//   dynamic arrays to meet the above requirement; see
//   https://en.wikipedia.org/wiki/Dynamic_array
// WRITE COMMENTS DESCRIBING YOUR MAKEUP CREDIT APPROACH; solutions
// without reasonably detailed comments will not receive any MAKEUP
// credit.
char *slurp_file(char *filename){
  FILE *file_handle = fopen(filename, "r"); // open the file for reading
  char *return_value = NULL;
  int num_of_chars = 1; // number of characters in the file. Start with 1 because of the terminating character

  if (file_handle == NULL){ // Check if file is opened
    return NULL;// return null
  }
  else{
    while (fgetc(file_handle) != EOF){ // run until EOF reached
      num_of_chars++; // increment the number of characters
    }
    // printf("%d", num_of_chars);
    return_value = malloc(num_of_chars); // create an array to return with the size of the file
    rewind(file_handle);                       // rewind the file

    for (int i = 0; i < num_of_chars; i++){
      if (i < num_of_chars - 1)
      { // add all the characters from the file
        return_value[i] = fgetc(file_handle);
      }
      else if (i == num_of_chars - 1)
      { // if this is the last iteration, add the terminating character
        return_value[i] = '\0';
      }
    }
  fclose(file_handle);
  }
  return return_value;

  // Start
  
pn_t *get_pn(int*arr, int len){
  pn_t *return_value = malloc(sizeof(pn_t));
  if(arr == NULL || len < 0){
    return NULL;
  }
  
  for(int i = 0; i <len; i++){
    if(arr[i] >= 0){
      return_value->poss++;
    }
    else{
      return_valu->negs++;
    }
  }
  return return_value;
}
  
}
