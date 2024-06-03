// banlet_main.c: main function which uses functions in banlet_funcs.c
// to print messages in a "fontified" fashion.

#include "banlet.h"

int main(int argc, char *argv[]){
  // PROVIDED: print a usage message and exit if no arguments are
  // provided to the program.
  if(argc < 2){
    printf("usage: %s '<text>'\n",argv[0]);
    printf("       %s '<text>' --fontfile <fontfile>\n",argv[0]);
    printf("       %s --textfile <textfile>\n",argv[0]);
    printf("       %s --textfile <textfile> --fontfile <fontfile>\n",argv[0]);
    printf("Arguments can appear in any order only one of <text> or <textfile> can be used\n");
    printf("\n");
    printf("Note: In some shells, embedded newline \\n characters, require\n");
    printf("the syntax $'text' as in\n");
    printf("        %s $'HELLO\\nWORLD\\n!!!'\n",argv[0]);
    printf("to be treated correctly. This is NOT necessary on grace.umd.edu\n");
    printf("This is NOT necessary on grace.umd.edu where the syntax\n");
    printf("        %s 'HELLO\\nWORLD\\n!!!'\n",argv[0]);
    printf("will behave correctly to embed newlines due to tcsh being used.\n");
    return 1;
  }

  // PROVIDED: Set default options
  char *text = NULL;                  // text to print, from command line or file
  int use_textfile = 0;               // free 'text' or not?
  font_t *font = &font_standard;      // default to built in standard font
  //printf(argv[0]);
  //printf(argv[1]);
  //printf(argv[2]);
  // REQUIRED: Command line parsing loop which handles --fontfile,
  // --textfile, and text arguments, sets variables above
  // appropriately.
  int use_fontfile = 0; // check if its needed to free the font
  for(int i = 1; i <argc; i++){ // iterate through ther command arguments
    if(strcmp(argv[i], "--fontfile") == 0){ // if the argument is a font file
      if(i != argc){ // if this is not the last argument
        font = font_load(argv[i+1]);
        i++;
        use_fontfile = 1;
      }
    }
    else if(strcmp(argv[i], "--textfile") == 0){ // if the argument is a text file
      if(i != argc){
        text = slurp_file(argv[i+1]); // if this is not the last argument
        i++;
        use_textfile = 1;
      }
    }
    else{ // if the argument is text
      text = argv[i];
    }
  }

  // REQUIRED: Call to print_fontified to print the banner text
  print_fontified(text, font);

  // REQUIRED: Checks to free memory if needed. If a font was loaded
  // from a file, free it. If text was loaded from a file was loaded,
  // free it.
  if(use_fontfile == 1){ // if its needed to free the font, free it
    font_free(font);
  }
  if(use_textfile == 1){ // if its needed to free the text, free it
    free(text);
  }
  
  // PROVIDED: return 0 on a normal exit.
  return 0;
}
    
