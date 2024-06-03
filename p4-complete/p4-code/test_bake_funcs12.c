// test_bake_funcs12.c: unit tests for bake functions and data for
// problems 1 and 2
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
    
  IF_TEST("slurp_file_efficient1") { 
    // Create a file then slurp it to show its contents are
    // corect. Single line file with text.
    system("echo 'Bake me a cake as quick as you can' > test-results/slurp1.txt");
    char *file_bytes = slurp_file_efficient("test-results/slurp1.txt");
    printf("file_bytes START\n%s\nfile_bytes STOP\n",file_bytes);
    free(file_bytes);
  } // ENDTEST

  IF_TEST("slurp_file_efficient2") { 
    // Create a file then slurp it to show its contents are corect.
    // Larger file with number data.
    system("seq -w 100000000 1000000 200000000 > test-results/slurp2.txt");
    char *file_bytes = slurp_file_efficient("test-results/slurp1.txt");
    printf("file_bytes START\n%s\nfile_bytes STOP\n",file_bytes);
    free(file_bytes);
  } // ENDTEST

  IF_TEST("slurp_file_efficient_empty") { 
    // Check for correct handling of empty files.
    system("touch test-results/slurp-empty.txt");
    char *file_bytes = slurp_file_efficient("test-results/slurp-empty.txt");
    printf("file_bytes START\n%s\nfile_bytes STOP\n",file_bytes);
    free(file_bytes);
  } // ENDTEST

  IF_TEST("slurp_file_efficient_fail") { 
    // Check that slurping correctly returns NULL when a file can't be
    // opened. The function should use perror() to print the message
    // "Couldn't open file" which will append a reason for it.
    system("rm -f test-results/slurp-not-there.txt");
    char *file_bytes = slurp_file_efficient("test-results/slurp-not-there.txt");
    printf("file_bytes START\n%s\nfile_bytes STOP\n",file_bytes);
    // free(file_bytes);   // should be NULL so no need to free
  } // ENDTEST


  // // NOTE: this test is likely to be flaky on GRACE as AFS uses a
  // // different permissions system than standard
  // IF_TEST("slurp_file_efficient_fail") { 
  //   // Check that slurping correctly returns NULL when a file can't be
  //   // opened. The function should use perror() to print the message
  //   // "Couldn't open file" which will append a reason for it.
  //   system("echo 'Dont touch me' > test-results/no-permissions.txt");
  //   system("chmod ugo-rwx test-results/no-permissions.txt");
  //   char *file_bytes = slurp_file_efficient("test-results/no-permissions.txt");
  //   printf("file_bytes START\n%s\nfile_bytes STOP\n",file_bytes);
  //   // free(file_bytes);   // should be NULL so no need to free
  // } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////

  IF_TEST("bake_target_rule1") { 
    // checks that the rule associated with an existing target is
    // found in a small bake
    rule_t rules[8] = {
      { .target      = "target0",
        .deps        = {"dep0A","dep0B","dep0C"},
        .cmd_count   = 2,
        .cmds        = {
          { .tokens = {"a","b","c"} },
          { .tokens = {"d","e"} },
        }
      },
      { .target      = "target1",
        .deps        = {"dep1A","dep1B"},
        .cmd_count   = 3,
        .cmds        = {
          { .tokens = {"a","b","c"} },
          { .tokens = {"d","e"} },
          { .tokens = {"f","g","h"} },
        }
      },            
      { .target      = "target2",
        .deps        = {"dep2A","dep2B","dep2C","dep2D"},
        .cmd_count   = 1,
        .cmds        = {
          { .tokens = {"a","b","c"} },
        }
      },            
    };
    bake_t bake = {
      .filename      = "<none>",
      .rule_capacity = 8,
      .rule_count    = 3,
      .rules         = rules,
      .string_table  = NULL
    };
    rule_t *rule;

    printf("FIRST LOOKUP\n");
    rule = bake_target_rule(&bake, "target1");
    if(rule == NULL){
      printf("rule not found\n");
    }
    else{
      bake_show_rule(rule,0);
    }

    printf("SECOND LOOKUP\n");
    rule = bake_target_rule(&bake, "target2");
    if(rule == NULL){
      printf("rule not found\n");
    }
    else{
      bake_show_rule(rule,0);
    }
  } // ENDTEST
  
  IF_TEST("bake_target_rule2") { 
    // Checks if a variety of targets are present/absent in a 
    // Bakefile loaded from the data/ directory
    printf("BAKEFILE CONTENTS\n");
    system("cat data/Bakefile3");
    bake_t *bake = bake_create_from_file("data/Bakefile3"); 
    char *targname;  
    printf("\nCHECKING TARGETS\n");
    targname = "all";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "hello.o";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "clean";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "demo";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "bye.o";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "nada.c";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "missing";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    targname = "nowhere.txt";
    printf("CHECK %12s : %s\n",targname,
           bake_target_rule(bake,targname)!=NULL?"FOUND":"NULL");
    bake_free(bake);
  } // ENDTEST


  IF_TEST("bake_target_rule_missing") { 
    // checks that NULL is returned when a target is missing from all
    // rules
    rule_t rules[8] = {
      { .target      = "target0",
        .deps        = {"dep0A","dep0B","dep0C"},
        .cmd_count   = 2,
        .cmds        = {
          { .tokens = {"a","b","c"} },
          { .tokens = {"d","e"} },
        }
      },
      { .target      = "target1",
        .deps        = {"dep1A","dep1B"},
        .cmd_count   = 3,
        .cmds        = {
          { .tokens = {"a","b","c"} },
          { .tokens = {"d","e"} },
          { .tokens = {"f","g","h"} },
        }
      },            
      { .target      = "target2",
        .deps        = {"dep2A","dep2B","dep2C","dep2D"},
        .cmd_count   = 1,
        .cmds        = {
          { .tokens = {"a","b","c"} },
        }
      },            
    };
    bake_t bake = {
      .filename      = "<none>",
      .rule_capacity = 8,
      .rule_count    = 3,
      .rules         = rules,
      .string_table  = NULL
    };
    rule_t *rule = bake_target_rule(&bake, "targetX");
    if(rule == NULL){
      printf("rule not found\n");
    }
    else{
      bake_show_rule(rule,0);
    }
  } // ENDTEST
  
  ////////////////////////////////////////////////////////////////////////////////

  
  IF_TEST("bake_add_empty_rule1") { 
    // Loads a Bakefile with a single rule, adds an empty rule and
    // modifies to check data lands in the right place
    printf("BAKEFILE CONTENTS\n");
    system("cat data/Bakefile1");
    bake_t *bake = bake_create_from_file("data/Bakefile1"); // 1 rule
    rule_t *new_rule = bake_add_empty_rule(bake);
    new_rule->target = "new_target";
    printf("\nAFTER add\n");
    bake_show_bake(bake,0);
    bake_free(bake);
  } // ENDTEST

  IF_TEST("bake_add_empty_rule2") { 
    // Loads a Bakefile with a single rule, adds an several empty
    // rules with modifications. This should trigger expansion of the
    // rules array accommodate the new rules.
    printf("BAKEFILE CONTENTS\n");
    system("cat data/Bakefile2");
    bake_t *bake = bake_create_from_file("data/Bakefile2"); // 3 rules
    rule_t *new_rule;
    new_rule = bake_add_empty_rule(bake); // 4 rules
    new_rule->target = "target4";
    new_rule = bake_add_empty_rule(bake); // 5 rules
    new_rule->target = "target5";         // capacity to 8
    new_rule = bake_add_empty_rule(bake); // 6 rules
    new_rule->target = "target6";
    printf("\n====3 ADDS: CAPACITY 8====\n");
    bake_show_bake(bake,0);

    new_rule = bake_add_empty_rule(bake); // 7 rules
    new_rule->target = "target7";
    new_rule = bake_add_empty_rule(bake); // 8 rules
    new_rule->target = "target8";
    new_rule = bake_add_empty_rule(bake); // 9 rules
    new_rule->target = "target9";         // capacity to 16
    printf("\n====6 ADDS CAPACITY 16====\n");
    bake_show_bake(bake,0);

    bake_free(bake);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////

  IF_TEST("bake_add_implicit_rules1") { 
    // Loads a Bakefile with a single explicit rule and checks that
    // bake_add_implicit_rules() adds in implict rules for its
    // dependency
    printf("ORIGINAL FILE\n");
    system("cat data/BakeImplicit1");
    bake_t *bake = bake_create_from_file("data/BakeImplicit1");
    bake_add_implicit_rules(bake);
    printf("\nAFTER bake_add_implicit_rules()\n");
    bake_show_bake(bake,0);
    bake_free(bake);
  } // ENDTEST


  IF_TEST("bake_add_implicit_rules2") { 
    // Loads a Bakefile with several rules with dependencies which
    // need to be have implicit rules created for them
    printf("ORIGINAL FILE\n");
    system("cat data/BakeImplicit2");
    bake_t *bake = bake_create_from_file("data/BakeImplicit2");
    bake_add_implicit_rules(bake);
    printf("\nAFTER bake_add_implicit_rules()\n");
    bake_show_bake(bake,0);
    bake_free(bake);
  } // ENDTEST

  IF_TEST("bake_add_implicit_rules3") { 
    // Loads a larger bakefile wiht the need to create several
    // implicit rules.
    printf("ORIGINAL FILE\n");
    system("cat data/BakeImplicit3");
    bake_t *bake = bake_create_from_file("data/BakeImplicit3");
    bake_add_implicit_rules(bake);
    printf("\nAFTER bake_add_implicit_rules()\n");
    bake_show_bake(bake,0);
    bake_free(bake);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////

  IF_TEST("bake_print_cmd_simple") { 
    // Checks output of a very simple command without any special
    // flags set.
    cmd_t cmd = {
      .tokens = {"gcc","-o","myprog","-g","prog_main.c"},
      .input_redirect  = NULL,
      .output_redirect = NULL,
      .line_number = 32,
      .cmd_flags = 0
    };
    bake_print_cmd(&cmd);
  } // ENDTEST

  IF_TEST("bake_print_cmd_silence") { 
    // Checks that when the SILENCE_BIT is set, no output is printed.
    cmd_t cmd = {
      .tokens = {"echo","Alas poor Yorick, I knew him, Horatio."},
      .input_redirect  = NULL,
      .output_redirect = NULL,
      .line_number = 13,
      .cmd_flags = CMD_SILENCE_BIT,
    };
    bake_print_cmd(&cmd);
  } // ENDTEST

  IF_TEST("bake_print_cmd_silence") { 
    // Checks that when the SILENCE_BIT is set, no output is printed.
    cmd_t cmd = {
      .tokens = {"echo","Alas poor Yorick, I knew him, Horatio."},
      .input_redirect  = NULL,
      .output_redirect = "hamlet.txt",
      .line_number = 8,
      .cmd_flags = CMD_SILENCE_BIT|CMD_OUTREDI_BIT,
    };
    bake_print_cmd(&cmd);
  } // ENDTEST

  IF_TEST("bake_print_cmd_io_redir") { 
    // Checks output redirection detected and printed correctly when
    // displaying commands
    cmd_t cmd = {
      .tokens = {"seq","10","5","50"},
      .input_redirect  = NULL,
      .output_redirect = "nums.txt",
      .line_number = 31,
      .cmd_flags = CMD_OUTREDI_BIT,
    };
    bake_print_cmd(&cmd);
  } // ENDTEST

  IF_TEST("bake_print_cmd_io_redir") { 
    // Checks input redirection detected and printed correctly when
    // displaying commands
    cmd_t cmd = {
      .tokens = {"wc","-l"},
      .input_redirect  = "essay.txt",
      .output_redirect = NULL,
      .line_number = 145,
      .cmd_flags = CMD_INPREDI_BIT,
    };
    bake_print_cmd(&cmd);
  } // ENDTEST

  IF_TEST("bake_print_cmd_io_redir") { 
    // Checks input/output redirection detected and printed correctly
    // when displaying commands
    cmd_t cmd = {
      .tokens = {"tr","a-z","A-Z"},
      .input_redirect  = "mixed.txt",
      .output_redirect = "upper.txt",
      .line_number = 259,
      .cmd_flags = CMD_OUTREDI_BIT | CMD_INPREDI_BIT
    };
    bake_print_cmd(&cmd);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////

  IF_TEST("bake_execute_cmd1") { 
    // Checks that simple command excutions works correctly
    cmd_t cmd = {
      .tokens = {"echo","Alas poor Yorick, I knew him, Horatio."},
      .input_redirect  = NULL,
      .output_redirect = NULL,
      .line_number = 13,
      .cmd_flags = 0,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd2") { 
    // Checks that simple command excutions works correctly
    cmd_t cmd = {
      .tokens = {"seq","1","10","60"},
      .input_redirect  = NULL,
      .output_redirect = NULL,
      .line_number = 0,
      .cmd_flags = 0,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd3") { 
    // Checks that simple command excutions works correctly
    cmd_t cmd = {
      .tokens = {"wc","-l","data/randnums.txt",
                 "data/randnums50.txt","data/gettysburg.txt"},
      .input_redirect  = NULL,
      .output_redirect = NULL,
      .line_number = 0,
      .cmd_flags = 0,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd_in1") { 
    // Checks command w/ input redirection executes correctly
    cmd_t cmd = {
      .tokens = {"wc","-w","-c"},
      .input_redirect  = "data/gettysburg.txt",
      .output_redirect = NULL,
      .line_number = 0,
      .cmd_flags = CMD_INPREDI_BIT,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd_out1") { 
    // Checks command w/ output redirection executes correctly
    cmd_t cmd = {
      .tokens = {"wc","data/gettysburg.txt"},
      .input_redirect  = NULL,
      .output_redirect = "test-results/cmd_out1.txt",
      .line_number = 0,
      .cmd_flags = CMD_OUTREDI_BIT,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
    printf("FILE CONTENTS\n");
    system("cat test-results/cmd_out1.txt");
  } // ENDTEST

  IF_TEST("bake_execute_cmd_inout1") { 
    // Checks command w/ both input and output redirection executes
    // correctly
    cmd_t cmd = {
      .tokens = {"sed","s/f/p/g; s/F/P/g"},
      .input_redirect  = "data/gettysburg1.txt",
      .output_redirect = "test-results/cmd_inout1.txt",
      .line_number = 0,
      .cmd_flags = CMD_INPREDI_BIT | CMD_OUTREDI_BIT,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
    printf("FILE CONTENTS\n");
    system("cat test-results/cmd_inout1.txt");
  } // ENDTEST

  IF_TEST("bake_execute_cmd_fail1") { 
    // Check for non-zero exit code from child is passed up
    cmd_t cmd = {
      .tokens = {"ls","data/no-such-file.txt"},
      .input_redirect  = NULL,
      .output_redirect = NULL,
      .line_number = 0,
      .cmd_flags = 0,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd_fail2") { 
    // Checks that failing to exec() will return the correct exit code
    cmd_t cmd = {
      .tokens = {"wubba_lubba","dub","dub"}, // no such command
      .input_redirect  = NULL,               // so exec will fail
      .output_redirect = NULL,
      .line_number = 0,
      .cmd_flags = 0,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd_fail3") { 
    // Checks that failing to exec() will return the correct exit code
    cmd_t cmd = {
      .tokens = {"wc"},
      .input_redirect  = "data/no-such-file.txt", // open() will fail
      .output_redirect = NULL,
      .line_number = 0,
      .cmd_flags = CMD_INPREDI_BIT,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  IF_TEST("bake_execute_cmd_fail4") { 
    // Checks that failing to exec() will return the correct exit code
    cmd_t cmd = {
      .tokens = {"echo","Failed to write"},
      .input_redirect  = NULL,
      .output_redirect = "test-results",         // can't write to a directory
      .line_number = 0,
      .cmd_flags = CMD_OUTREDI_BIT,
    };
    int ret = bake_execute_cmd(&cmd);
    printf("ret: %d\n",ret);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////
  // END OF ALL TESTS

  if(nrun == 0){                // check that at least one test was run
    printf("No test named '%s' found\n",test_name);
    return 1;
  }
  return 0;
}

