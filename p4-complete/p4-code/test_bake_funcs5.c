// test_bake_funcs5.c: unit tests for bake functions and data for
// problem 5 on makeup credit
#include "bake.h"

// macro to set up a test with given name, print the source of the
// test; very hacky, fragile, but useful
#define IF_TEST(TNAME) \
  if( RUNALL ) { printf("\n"); } \
  if( RUNALL || strcmp( TNAME, test_name)==0 ) { \
    sprintf(sysbuf,"awk 'NR==(%d){P=1;gsub(\"^ *\",\"\");} P==1 && /ENDTEST/{P=0; print \"}\\n---OUTPUT---\"} P==1{print}' %s", __LINE__, __FILE__); \
    system(sysbuf); nrun++;  \
  } \
  if( RUNALL || strcmp( TNAME, test_name)==0 )

void print_sys(char *cmd){
  printf("%s\n",cmd);
  system(cmd);
}

char sysbuf[1024];
int RUNALL = 0;
int nrun = 0;

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s <test_name>\n", argv[0]);
    printf("       %s ALL\n", argv[0]);
    return 1;
  }
  char *test_name = argv[1];
  char sysbuf[1024];

  RUNALL = strcmp(test_name,"ALL")==0; // possibly run all tests

  system("mkdir -p test-results"); // ensure a subdirectory for data is present
    
  IF_TEST("bake_check_cycles_noprint1") { 
    // Checks several Bakefiles for cycles without printing to
    // determine if bake_check_cycles() correctly identifies cycles vs
    // not.  Printing is not enabled as getting the message printing
    // correct is trickier.
    bake_t *bake; int ret;

    // bake_check_cycles01
    system("rm -rf test-results/bake_check_cycles01");
    system("cp -r data5/bake_check_cycles01 test-results/");
    chdir("test-results/bake_check_cycles01");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles()---\n");
    ret = bake_check_cycles(bake,"fileA.txt",0);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

    // bake_check_cycles02
    system("rm -rf test-results/bake_check_cycles02");
    system("cp -r data5/bake_check_cycles02 test-results/");
    chdir("test-results/bake_check_cycles02");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles()---\n");
    ret = bake_check_cycles(bake,"fileA.txt",0);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

    // bake_check_cycles03
    system("rm -rf test-results/bake_check_cycles03");
    system("cp -r data5/bake_check_cycles03 test-results/");
    chdir("test-results/bake_check_cycles03");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("--- CALL TO bake_check_cycles() fileA.txt ---\n");
    ret = bake_check_cycles(bake,"fileA.txt",0);
    printf("ret: %d\n",ret);
    printf("--- CALL TO bake_check_cycles() fileX.txt ---\n");
    ret = bake_check_cycles(bake,"fileX.txt",0);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

    // bake_check_cycles04
    system("rm -rf test-results/bake_check_cycles04");
    system("cp -r data5/bake_check_cycles04 test-results/");
    chdir("test-results/bake_check_cycles04");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles() fileUL.txt---\n");
    ret = bake_check_cycles(bake,"fileUL.txt",0);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileUR.txt---\n");
    ret = bake_check_cycles(bake,"fileUR.txt",0);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

    // bake_check_cycles_none
    system("rm -rf test-results/bake_check_cycles_none");
    system("cp -r data5/bake_check_cycles_none test-results/");
    chdir("test-results/bake_check_cycles_none");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles()---\n");
    ret = bake_check_cycles(bake,"all",0);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

  } // ENDTEST

  IF_TEST("bake_check_cycles_print1") { 
    // Checks a couple Bakefiles for cycles WITH printing to
    // determine if bake_check_cycles() correctly identifies cycles vs
    // not.  These are simple, short cycles.
    bake_t *bake; int ret;

    // bake_check_cycles01
    system("rm -rf test-results/bake_check_cycles01");
    system("cp -r data5/bake_check_cycles01 test-results/");
    chdir("test-results/bake_check_cycles01");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles() fileA.txt---\n");
    ret = bake_check_cycles(bake,"fileA.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileB.txt---\n");
    ret = bake_check_cycles(bake,"fileB.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileC.txt---\n");
    ret = bake_check_cycles(bake,"fileC.txt",1);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

    // bake_check_cycles02
    system("rm -rf test-results/bake_check_cycles02");
    system("cp -r data5/bake_check_cycles02 test-results/");
    chdir("test-results/bake_check_cycles02");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles() fileC.txt---\n");
    ret = bake_check_cycles(bake,"fileC.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileD.txt---\n");
    ret = bake_check_cycles(bake,"fileD.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileA.txt---\n");
    ret = bake_check_cycles(bake,"fileA.txt",1);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");
  } // ENDTEST

  IF_TEST("bake_check_cycles_print2") { 
    // Checks more complex Bakefiles for cycles WITH printing to
    // determine if bake_check_cycles() correctly identifies cycles vs
    // not. Cycles are longer and somewhat more involved in this test.
    bake_t *bake; int ret;

    // bake_check_cycles03
    system("rm -rf test-results/bake_check_cycles03");
    system("cp -r data5/bake_check_cycles03 test-results/");
    chdir("test-results/bake_check_cycles03");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles() fileA.txt---\n");
    ret = bake_check_cycles(bake,"fileA.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileX.txt---\n");
    ret = bake_check_cycles(bake,"fileX.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileZ.txt---\n");
    ret = bake_check_cycles(bake,"fileZ.txt",1);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");

    // bake_check_cycles04
    system("rm -rf test-results/bake_check_cycles04");
    system("cp -r data5/bake_check_cycles04 test-results/");
    chdir("test-results/bake_check_cycles04");
    printf("\n--- CONTENTS OF BAKEFILE ---\n");
    system("cat Bakefile");
    bake = bake_create_from_file("Bakefile");
    bake_add_implicit_rules(bake);
    printf("---CALL TO bake_check_cycles() fileUL.txt---\n");
    ret = bake_check_cycles(bake,"fileUL.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileUR.txt---\n");
    ret = bake_check_cycles(bake,"fileUR.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileX.txt---\n");
    ret = bake_check_cycles(bake,"fileX.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileLL1.txt---\n");
    ret = bake_check_cycles(bake,"fileLL1.txt",1);
    printf("ret: %d\n",ret);
    printf("---CALL TO bake_check_cycles() fileLR1.txt---\n");
    ret = bake_check_cycles(bake,"fileLL1.txt",1);
    printf("ret: %d\n",ret);
    bake_free(bake);
    chdir("../..");
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////
  // END OF ALL TESTS

  if(nrun == 0){                // check that at least one test was run
    printf("No test named '%s' found\n",test_name);
    return 1;
  }
  return 0;
}

