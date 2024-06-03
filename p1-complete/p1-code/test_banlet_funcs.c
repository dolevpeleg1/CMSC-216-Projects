#include "banlet.h"

// macro to set up a test with given name, print the source of the
// test; very hacky, fragile, but useful
#define IF_TEST(TNAME) \
  if( RUNALL || strcmp( TNAME, test_name)==0 ) { \
    sprintf(sysbuf,"awk 'NR==(%d){P=1;gsub(\"^ *\",\"\");} P==1 && /ENDTEST/{P=0; print \"}\\n---OUTPUT---\"} P==1{print}' %s", __LINE__, __FILE__); \
    system(sysbuf); nrun++;  \
  } \
  if( RUNALL || strcmp( TNAME, test_name)==0 )

char sysbuf[1024];
int RUNALL = 0;
int nrun = 0;

// #define PRINT_TEST sprintf(sysbuf,"awk 'NR==(%d+1){P=1;print \"{\"} P==1 && /ENDTEST/{P=0; print \"}\"} P==1{print}' %s", __LINE__, __FILE__); \
//                    system(sysbuf);

void print_int_array(int *arr, int length){
  printf("[");
  for(int i=0; i<length-1; i++){
    printf("%2d, ",arr[i]);
  }
  if(length > 0){
    printf("%2d",arr[length-1]);
  }
  printf("]");
}

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s <test_name>\n", argv[0]);
    return 1;
  }
  char *test_name = argv[1];
  char sysbuf[1024];

  if(0){}
  IF_TEST("string_replace_char_1") { 
    // Tests replacing characters in a string
    char string[]="A A B B A";
    string_replace_char(string, 'A', 'X');
    printf("result: '%s'\n", string);
  } // ENDTEST

  IF_TEST("string_replace_char_2") { 
    // Tests replacing characters in a string
    char string[]="A A B B A";
    string_replace_char(string, 'B', 'Y');
    printf("result: '%s'\n", string);
  } // ENDTEST

  IF_TEST("string_replace_char_3") { 
    // Tests replacing characters in a string
    char string[]="A A B B A";
    string_replace_char(string, ' ', '-');
    printf("result: '%s'\n", string);
  } // ENDTEST

  IF_TEST("string_replace_char_4") { 
    // Tests replacing characters in a string
    char string[]=
      "Four score and seven years ago our forefathers brought forth...";
    string_replace_char(string, 'F', 'P');
    string_replace_char(string, 'f', 'p');
    printf("result: '%s'\n", string);
  } // ENDTEST

  IF_TEST("count_linebreaks_1") { 
    // Tests counting linebreaks (\n and end of string) in a string
    char string[]="Hi";
    int count = count_linebreaks(string);
    printf("result: %d\n", count);
  } // ENDTEST

  IF_TEST("count_linebreaks_2") { 
    // Tests counting linebreaks (\n and end of string) in a string
    char string[]="O\nM\nG";
    int count = count_linebreaks(string);
    printf("result: %d\n", count);
  } // ENDTEST

  IF_TEST("count_linebreaks_3") { 
    // Tests counting linebreaks (\n and end of string) in a string
    char string[]=
      "If you lie to\nthe compiler,\nit will get\nits revenge.\n-Henry Spencer";
    int count = count_linebreaks(string);
    printf("result: %d\n", count);
  } // ENDTEST

  IF_TEST("find_linebreaks_1") { 
    // Tests locating linebreak indices in a string
    char string[]="Hi";
    int count = -1;
    int *linebreaks = find_linebreaks(string, &count);
    printf("count: %d\n",count);
    printf("array: "); print_int_array(linebreaks, count); printf("\n");
    free(linebreaks);
  } // ENDTEST

  IF_TEST("find_linebreaks_2") { 
    // Tests locating linebreak indices in a string
    char string[]="O\nM\nG";
    int count = -1;
    int *linebreaks = find_linebreaks(string, &count);
    printf("count: %d\n",count);
    printf("array: "); print_int_array(linebreaks, count); printf("\n");
    free(linebreaks);
  } // ENDTEST

  IF_TEST("find_linebreaks_3") { 
    // Tests locating linebreak indices in a string
    char string[]="If you lie to\nthe compiler,\nit will get\nits revenge.\n-Henry Spencer";
    int count = -1;
    int *linebreaks = find_linebreaks(string, &count);
    printf("count: %d\n",count);
    printf("array: "); print_int_array(linebreaks, count); printf("\n");
    free(linebreaks);
  } // ENDTEST

  IF_TEST("find_linebreaks_4") { 
    // Tests locating linebreak indices in a string
    char string[]="10\n20\n30\n40 50\n60 70 80\n 90\n100\n110 120\n130";
    int count = -1;
    int *linebreaks = find_linebreaks(string, &count);
    printf("count: %d\n",count);
    printf("array: "); print_int_array(linebreaks, count); printf("\n");
    free(linebreaks);
  } // ENDTEST

  IF_TEST("print_fontified_oneline_1") { 
    // print_fontified_oneline function with builtin font_standard
    char string[]="Hello world!";
    print_fontified_oneline(string, &font_standard, strlen(string));
  } // ENDTEST

  IF_TEST("print_fontified_oneline_2") { 
    // print_fontified_oneline function with builtin font_standard
    char *string;
    string="Holy $#!^";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string="it WORKED.";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string="SHOCKING, I know.";
    print_fontified_oneline(string, &font_standard, strlen(string));
  } // ENDTEST


  IF_TEST("print_fontified_oneline_fox") { 
    // print_fontified_oneline function with builtin font_standard
    char *string;
    string = "The quick brown fox";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "jumps OVER the lazy";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "dog.";
    print_fontified_oneline(string, &font_standard, strlen(string));
  } // ENDTEST


  IF_TEST("print_fontified_oneline_3") { 
    // print_fontified_oneline function with builtin font_standard
    char *string;
    string = " !\"#$%&'()";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "*+,-./0123";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "456789:;<=";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = ">?@ABCDEFG";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "HIJKLMNOPQ";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "RSTUVWXYZ[";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "\\]^_`abcde";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "fghijklmno";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "pqrstuvwxy";
    print_fontified_oneline(string, &font_standard, strlen(string));
    string = "z{|}~";
    print_fontified_oneline(string, &font_standard, strlen(string));
  } // ENDTEST


  IF_TEST("print_fontified_oneline_length") { 
    // Checks print_fontified_oneline honors the length parameter.
    char *string;
    string = "One two three four, tell me that you love me more.";
    print_fontified_oneline(string, &font_standard, 4);
    print_fontified_oneline(string, &font_standard, 8);
    print_fontified_oneline(string, &font_standard, 19);
    print_fontified_oneline(string+20, &font_standard, 12);
    print_fontified_oneline(string+37, &font_standard, 13);
  } // ENDTEST


  IF_TEST("print_fontified_1") { 
    // Tests print_fontified_linebreak function with builtin font_standard
    char *string;
    string =
"apple\n\
B@N@N@\n\
Carr0^";
    print_fontified(string, &font_standard);
  } // ENDTEST

  IF_TEST("print_fontified_2") { 
    // Tests print_fontified_linebreak function with builtin font_standard
    char *string;
    string =
"The quick brown fox\n\
jumps OVER the lazy\n\
dog.";
    print_fontified(string, &font_standard);
  } // ENDTEST

  IF_TEST("print_fontified_3") { 
    // Tests print_fontified_linebreak function with builtin font_standard
    char *string;
    string =
" !\"#$%&'()\n\
*+,-./0123\n\
456789:;<=\n\
>?@ABCDEFG\n\
HIJKLMNOPQ\n\
RSTUVWXYZ[\n\
\\]^_`abcde\n\
fghijklmno\n\
pqrstuvwxy\n\
z{|}~";
    print_fontified(string, &font_standard);
  } // ENDTEST

  IF_TEST("load_font_1") { 
    // Tests load_font() on the font_alternate.txt file
    // ALSO checks that free_font() function deallocates.
    char *font_file = "data/font_alternate.txt";
    font_t *font = font_load(font_file);
    char *string =
" !\"#$%&'()\n\
*+,-./0123\n\
456789:;<=\n\
>?@ABCDEFG\n\
HIJKLMNOPQ\n\
RSTUVWXYZ[\n\
\\]^_`abcde\n\
fghijklmno\n\
pqrstuvwxy\n\
z{|}~";
    print_fontified(string, font);
    font_free(font);
  } // ENDTEST

  IF_TEST("load_font_2") { 
    // Tests load_font() on the font_mini.txt file
    // ALSO checks that free_font() function deallocates.
    char *font_file = "data/font_mini.txt";
    font_t *font = font_load(font_file);
    char *string =
" !\"#$%&'()\n\
*+,-./0123\n\
456789:;<=\n\
>?@ABCDEFG\n\
HIJKLMNOPQ\n\
RSTUVWXYZ[\n\
\\]^_`abcde\n\
fghijklmno\n\
pqrstuvwxy\n\
z{|}~";
    print_fontified(string, font);
    font_free(font);
  } // ENDTEST

  IF_TEST("load_font_3") { 
    // Tests load_font() on the font_capsonly.txt file.
    // This font does not have all ASCII codepoints defined
    // so some of the glyphs will appear as defaults / XXXs.
    char *font_file = "data/font_capsonly.txt";
    font_t *font = font_load(font_file);
    char *string =
" ! \" # $ % & ' ( )\n\
+ * , - . / 0 1 2 3\n\
4 5 6 7 8 9 : ; < =\n\
> ? @ A B C D E F G\n\
H I J K L M N O P Q\n\
R S T U V W X Y Z [\n\
\\ ] ^ _ ` a b c d e\n\
f g h i j k l m n o\n\
p q r s t u v w x y\n\
z { | } ~";
    print_fontified(string, font);
    font_free(font);
  } // ENDTEST

  IF_TEST("load_font_fail") { 
    // Tests that load_font() returns NULL if font is not found
    char *font_file = "data/no_such_font.txt.txt";
    font_t *font = font_load(font_file);
    if(font != NULL){
      printf("What the deuce just happened?\n");
    }
    else{
      printf("NULL returned correctly\n");
    }
  } // ENDTEST

  IF_TEST("slurp_file_1") { 
    // Print contents of a text file using slurp_file().
    // Single line file
    char *text_file = "data/asimov.txt";
    char *text = slurp_file(text_file);
    printf("%s",text);
    free(text);
  } // ENDTEST

  IF_TEST("slurp_file_2") { 
    // Print contents of a text file using slurp_file().
    // Multi-line file
    char *text_file = "data/bruce.txt";
    char *text = slurp_file(text_file);
    printf("%s",text);
    free(text);
  } // ENDTEST

  IF_TEST("slurp_file_fail") { 
    // Ensure slurp_file() returns NULL on failing to open a file
    char *text_file = "data/no_such_file.txt";
    char *text = slurp_file(text_file);
    if(text != NULL){
      printf("What the deuce just happened?\n");
    }
    else{
      printf("NULL returned correctly\n");
    }
  } // ENDTEST

  if(nrun == 0){
    printf("No test named '%s' found\n",test_name);
    return 1;
  }

  return 0;
}
