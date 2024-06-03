// bake_funcs.c: required service functions for handling of Bakefiles
#include "bake.h"

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 1
////////////////////////////////////////////////////////////////////////////////

char *slurp_file_efficient(char *fname){
// PROBLEM 1: Uses combination of stat() and read() to efficiently
// read in the entire contents of a file into a malloc()'d block of
// memory, null terminates it (\0). Returns a pointer to the file data
// or NULL if the file could not be opened.

  struct stat sb; // create a stat struct
  if(stat(fname, &sb) == -1){ // if file doesn't exist
    printf("Couldn't open file: No such file or directory\n"); 
    return NULL;
  }
  char *content = malloc(sb.st_size + 1); // malloc an array with the size of the file
  FILE *fh = fopen(fname, "r"); // open the file
  for(int i = 0; i < sb.st_size; i++){ // get all the characters from the file
    content[i] = fgetc(fh);
  }
  content[sb.st_size] = '\0'; // null terminate the array
  fclose(fh); // close the file

  return content;
}

rule_t *bake_target_rule(bake_t *bake, char *targname){
// PROBLEM 1: Searches bake for a rule with a target that equals
// `targname`. If found, returns a pointer to that rule. Otherwise
// returns NULL if no rule with target `targname` exists.
//
// DESIGN NOTE: The curent design makes use of linear search for
// target location, O(rule_count) for each lookup. Maintainting a hash
// table of target->rule key/vals would allow O(1) lookup for rules
// with given target names.
  for(int i = 0; i < bake->rule_count; i++){
    if(strcmp(bake->rules[i].target, targname) == 0){
        return &(bake->rules[i]);
    }
  }
  return NULL;
}

rule_t *bake_add_empty_rule(bake_t *bake){
// PROBLEM 1: Modifies `bake` to add a new, empty rule to it and
// returns a pointer to that rule. If bake->rules[] is full
// (rule_capacity and rule_count are equal) doubles the size of
// rules[] via realloc() in order create room at its end for the new
// empty rule. Returns a pointer to the new empty rule.
//
// CLARIFICATION: This function intitalizes all the date in the
// returned rule to NULL or 0 including the nested arrays. HINT: make
// use of the memset() to quickly initialize the entire rule_t struct
// to 0's. This is possible due to the nested arrays being within the
// rule_t rather than pointers to other blocks of memory which means a
// single memset() call will suffice.
// 
// CAUTION: Calling this function MAY invalidate any pointers to
// existing rules as the array that houses the rules may move.
// This sequence is dangerous:
//   rule_t *rule  = bake_target_rule(bake, "sometarget");
//   rule_t *empty = bake_add_empty_rule(bake);
//   rule may now point to de-allocated memory

  if(bake->rule_capacity == bake->rule_count){ // if the rule array is full
  bake->rule_capacity = bake->rule_capacity * 2; // double the capacity
  bake->rules = (rule_t *)realloc(bake->rules, bake->rule_capacity * sizeof(rule_t)); // double the size using realloc
  }
  rule_t *rule = &bake->rules[bake->rule_count]; // get the rule at the next available spot in the array
  memset(rule, 0, sizeof(rule_t)); // initiliaze all the data of the rule to 0
  bake->rule_count++; // increment the rule count
  return rule;
}

int bake_add_implicit_rules(bake_t *bake){
// PROBLEM 1: Iterate over all rules appending implicit rules for any
// dependency that is not an explicit target. New rules added in this
// way are marked with the RULE_IMPLICIT_BIT.
//
// CAUTION: Since bake->rules[] may expand, care must be taken when
// referencing rules in this function to avoid inadvertently derefing
// a pointer to free()'d memory.

  for(int i = 0; i < bake->rule_count; i++){
    if(CHECK_BIT(bake->rules[i].rule_flags, RULE_IMPLICIT_BIT) == 0){ // if the rule is explicit
      int j = 0;
      while(bake->rules[i].deps[j] != NULL){ // get the dependency at position j
        if(bake_target_rule(bake, bake->rules[i].deps[j]) == NULL){ // if the dependency does not have a rule 
          // create an implicit rule for the dependency
          rule_t *rule = bake_add_empty_rule(bake); // create an empty rule
          rule->target = bake->rules[i].deps[j]; // set its target to the dependency
          SET_BIT(rule->rule_flags, RULE_IMPLICIT_BIT); // set its flag's bit to the implicit bit
        }
        j++;
      }
    }
  }
  return 0;
}

void bake_print_cmd(cmd_t *cmd){
// PROBLEM 1: If the SILENCE_BIT is set, does nothing and immediately
// returns. Otherwise, prints the given command token by token. If I/O
// redirects are indicated by the fields of the command, prints the
// after all tokens with "< infile" for input followed by "> outfile"
// if present. This function is used to print out commands in
// bake_execute_cmd().
  if(CHECK_BIT(cmd->cmd_flags, CMD_SILENCE_BIT) == 0){ // if the silince bit in off
    int i = 0;
    while(cmd->tokens[i] != NULL){ // print the tokens 
      printf(cmd->tokens[i]);
      printf(" ");
      i++;
    }
    if(CHECK_BIT(cmd->cmd_flags, CMD_INPREDI_BIT) != 0){ // if the input redirect bit is on
      printf("< %s", cmd->input_redirect);
      printf(" ");
    }
    if(CHECK_BIT(cmd->cmd_flags, CMD_OUTREDI_BIT) != 0){ // if the output redirect bit is on
      printf("> %s", cmd->output_redirect);
    }
    printf("\n");
  }
}

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 2
////////////////////////////////////////////////////////////////////////////////

int bake_execute_cmd(cmd_t *cmd){
// PROBLEM 2: Called during bake_do_update(). Prints the command using
// bake_print_cmd() unless its SILENCE bit is set. fork()'s a child
// process which exec's the command specified in cmd->tokens.  Sets up
// I/O redirection in child process if indicated by cmd flags and
// fields. Uses a wait() call to wait until the child process is
// done. If the child completes normally, its exit code is passed up,
// 0 or non-zero. If the child process fails to complete normally or
// prints an error message with line number of command
// CMD_NONSTAND_EXIT. Non-zero returns from this function will usually
// be handled in a build by cancelling the build whil 0 return will
// cause the build to prceed and execute subsequent commands.
//
// During Input and Output redirection, if the requrested files cannot
// be opened, the child process exits with code CMD_FAIL_INPREDI or
// CMD_FAIL_OUTREDI. Additionally, if the child fails to exec() a
// command, CMD_FAIL_EXEC is used as its return code. All of these are
// non-zero and trigger builds to fail. In the above failure cases,
// prints one of the below error messages appropriate to the error.
// 
// perror("ERROR: can't open file for output")
// perror("ERROR: can't open file for output")
// perror("ERROR: job failed to exec");
//
// CLARIFICATION: When open()'ing files for output redirection,
// certain options should be passed to ensure that the created file
// adheres to conventions
// - For the second argument to open(), pass options
//     O_WRONLY|O_CREAT|O_TRUNC
//   which will open the file for writing, create it if not present,
//   and truncate the file if it already exists
// - For the third argument to open(), pass the arguments
//     S_IRUSR|S_IWUSR
//   which will set the "rw" permissions on the created file for the
//   owner (user) of the file

  bake_print_cmd(cmd); // print the command if it is not silenced
  pid_t child = fork(); // fork a child

  // child code
  if(child == 0){
    if(CHECK_BIT(cmd->cmd_flags, CMD_INPREDI_BIT) != 0){ // if the input redirect bit is on
      int in_fd = open(cmd->input_redirect, O_RDONLY); // open file for reading
      if(in_fd == -1){ // if file failed to open
        perror("ERROR: can't open file for input");
        exit(CMD_FAIL_INPREDI);
      }
      dup2(in_fd, STDIN_FILENO); // change stdin to go to the file 
    }
    if(CHECK_BIT(cmd->cmd_flags, CMD_OUTREDI_BIT) != 0){ // if the output redirect bit is on
      int out_fd = open(cmd->output_redirect, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR); // open output file for writing, give user permissions
      if(out_fd == -1){ // if file failed to open
        perror("ERROR: can't open file for output");
        exit(CMD_FAIL_OUTREDI);
      }
      dup2(out_fd, STDOUT_FILENO); // change stdout to go to the file 
    }

    int exec_ret = execvp(cmd->tokens[0], cmd->tokens); // run the command specified in the tokens
    if(exec_ret == -1){ // if exec failed
      perror("ERROR: command failed to exec");
      exit(CMD_FAIL_EXEC);
    }
  }
  
  // parent code
  int status; // used to check return code of child
  wait(&status); // block until child is finished
  if(WIFEXITED(status)){ // check if the child exited properly
    return WEXITSTATUS(status);
  }
  return CMD_NONSTAND_EXIT; // nonstandard Child Exit Detected
}

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 3
////////////////////////////////////////////////////////////////////////////////

int bake_set_updates(bake_t *bake, char *targname){
// PROBLEM 3: Starting at `targname`, recurses down the
// target/dependency DAG setting the UPDATE bit in rule_flags to
// indicate whether a target requires updates. Makes use of the
// CLEAR_BIT(), CHECK_BIT(), and SET_BIT() macros when dealing with
// bit-manipulations. For a given rule, first visits all its
// dependencies to check if they require updates via recursion. Then
// determines if `targname` requires an update. The conditions that
// require an update are described in detail in the program
// specification and shoudl be consulted while implementing this
// function. Returns 1 if target needs to be updated so associated
// rule commands should be run later. Returns 0 if the named target
// does not need to be updated. Prints an error if targname is not
// found and returns -1. If any dependency returns -1, an error
// ocurred lower down the DAG. In case, nothing is printed nothing and
// -1 is returned up the nested recursion.
  rule_t *rule = bake_target_rule(bake, targname); // get the rule at target name
  int error_found = 0; // flag if error was found

  // perform some initial checks / actions on the current rule
  if(rule == NULL){ // check that target is in the rules[] array of bake
    printf("No rule to create target '%s'\n", targname); // if not, return print "No rule to create target 'XXX'" and return -1 for an error
    return -1; 
  }
  CLEAR_BIT(rule->rule_flags, RULE_UPDATE_BIT); // clear the update bit
  
  // check if the target file does not exist
  if(access(rule->target, F_OK) != 0){
    if(rule->cmd_count > 0){ // if there are commands to run, those commands might create the target or achieve some other effect so it requires an update.
      SET_BIT(rule->rule_flags, RULE_UPDATE_BIT); // set the update bit
    }
  }
  
  // check if the rule is implicit
  if(CHECK_BIT(rule->rule_flags, RULE_IMPLICIT_BIT)){
    if(access(rule->target, F_OK) == 0){ // if the file exist return 0
      return 0;
    }
    else{ // if the file does not exist
      printf("Implicit rule cannot find file '%s'\n", rule->target);
      return -1;
    }
  }

  // recursively call bake_set_updates() on each dependency of the given target
  int i = 0;
  while(rule->deps[i] != NULL){ // iterate through the dependencies of the target recursively
    int ret = bake_set_updates(bake, rule->deps[i]);
    if(ret == 1){ // if the dependency needs to be updated
      SET_BIT(rule->rule_flags, RULE_UPDATE_BIT); // set the update bit
    }
    else if(ret == -1){ // if any dependency returned -1, set the flag to 1
      error_found = 1;
    }
    if(access(rule->target, F_OK) == 0 && access(rule->deps[i], F_OK) == 0){ // if both the target and the dependency have existing files
      struct stat sb_target;
      struct stat sb_dependency;
      stat(rule->target, &sb_target); // set the stats of the target
      stat(rule->deps[i], &sb_dependency); // set the stats of the dependency
      if((diff_timespec(sb_dependency.st_mtim, sb_target.st_mtim) > 0)){ // If the dependency is newer than the target
      SET_BIT(rule->rule_flags, RULE_UPDATE_BIT); // set the update bit
      }
    }
    i++;
  }
  if(error_found == 1){ // if an error was returned from any dependency, return -1
    return -1;
  }
  // return whether or not action was taken on the current target
  if(CHECK_BIT(rule->rule_flags, RULE_UPDATE_BIT) == 0){
    return 0;
  }
  SET_BIT(rule->rule_flags, RULE_UPDATE_BIT); // set the update bit
  return 1;
  
}

int bake_do_updates(bake_t *bake, char *targname){
// PROBLEM 3: Starting at `targname`, run commands if the UPDATE bit
// is set. Before running commands associated with `targname`,
// recursively visit dependencies and if their UPDATE bit is set, run
// their commands first. Returns number of rules that were updated or
// -1 to indicate an error ocurred while running commands.
  rule_t *rule = bake_target_rule(bake, targname); // get the rule at target name

  if(CHECK_BIT(rule->rule_flags, RULE_UPDATE_BIT) == 0){ // if the update bit is not set
    return 0;
  }

  // recursively call bake_do_updates() on each dependency of the given target
  int rules_updated = 1;
  int i = 0;
  while(rule->deps[i] != NULL){ // iterate through the dependencies of the target recursively
    int ret = bake_do_updates(bake, rule->deps[i]);
    if(ret == -1){ // if an error was found
      return -1;
    }
    else{ // else increment the number of rules updated
      rules_updated += ret;
    }
    i++;
  }
  // run updates for the current target
  printf("bake: updating '%s' via %d command(s)\n", rule->target, rule->cmd_count);  
  for(int i = 0; i < rule->cmd_count; i++){
    int ret = bake_execute_cmd(&rule->cmds[i]); // execute command
    if(ret != 0){ // if execute failed, print error and return -1
      printf("%s:%d ERROR during target '%s', exit code %d\n", bake->filename, (rule->line_number +1), rule->target, ret);
      return -1;
    }
  }
  CLEAR_BIT(rule->rule_flags, RULE_UPDATE_BIT); // clear the update bit
  return rules_updated; // return the amount of commands executed
}
////////////////////////////////////////////////////////////////////////////////
// PROBLEM 4
////////////////////////////////////////////////////////////////////////////////

int bake_cmd_postprocess(bake_t *bake, cmd_t *cmd){
// PROBLEM 4: Examines cmd->tokens to set cmd->flags and a few other
// fields. Used to post-process the raw tokens of a command. Likely
// uses the utility function array_shift() to ease the re-arrangment
// of arrays.
// Correctly handles any order of "< input" / "> output" and other
// tokens (ex: output redirection may appear first followed by normal
// tokens followed by input redirection)
// 
// If multiple "<" or ">" tokens appear, the behavior of this function
// is unspecified: it may return an error, segfault, or behave randomly.
// 
// Returns 0 on succesful completion.
//
// DESIGN NOTE: This function must be called prior to any commands
// being run. The ideal location is during parsing to modify commands
// as a bake_t structure is built. However, this functionality is
// separated out to enable future functionality such as variable
// substitution to be added to commands and tested separately from
// parsing. Any additional context needed for such things will be part
// of the 'bake' parameter (e.g. may contain a map of variable
// names/values for substitutions).
  
  // 1. If tokens[0] = "@", eliminates this token by shifting all tokens
  // over and sets the SILENCED bit
  if(strcmp(cmd->tokens[0], "@") == 0){
    SET_BIT(cmd->cmd_flags, CMD_SILENCE_BIT);
    array_shift(cmd->tokens, 0, MAX_TOKS);
  }

  // iterate through the tokens
  int i = 0;
  while(cmd->tokens[i] != NULL){
    // 2. If tokens[i] = "<", input redirection is desired; if tokens[i+1]
    // is not NULL, returns -1. Otherwise sets the INPREDI bit and
    // input_redirect field to tokens[i+1]. Then shifts all tokens over to
    // eliminate tokens[i] and tokens[i+1].
    if(strcmp(cmd->tokens[i], "<") == 0){
      if(cmd->tokens[i + 1] == NULL){
        return -1;
      }
      else{
        SET_BIT(cmd->cmd_flags, CMD_INPREDI_BIT);
        cmd->input_redirect = cmd->tokens[i + 1];
        array_shift(cmd->tokens, i, MAX_TOKS);
        array_shift(cmd->tokens, i, MAX_TOKS);
        i -= 2;
      }
    }
    // 3. If tokens[i] = ">", output redirection is desired and performs
    // similar operations to #2 with the OUTREDI flag and output_redirect
    // fields.
    if(strcmp(cmd->tokens[i], ">") == 0){
      if(cmd->tokens[i + 1] == NULL){
        return -1;
      }
      else{
        SET_BIT(cmd->cmd_flags, CMD_OUTREDI_BIT);
        cmd->output_redirect = cmd->tokens[i + 1];
        array_shift(cmd->tokens, i, MAX_TOKS);
        array_shift(cmd->tokens, i, MAX_TOKS);
        i -= 2;
      }
    }
    i++;
  }
  return 0;


}

void bake_post_process(bake_t *bake){
// PROBLEM 4: Applies postprocessing operations to the bake after
// loading it from a file. Currently only iterates over all rules and
// applies bake_cmd_postprocess() to each of their commands.
//
// DESIGN NOTE: This function is where additional operations could be
// used to further modify the bakefile such as performing variable
// substitutions on rules / targets, including other bakefiles. etc.
  for(int i = 0; i < bake->rule_count; i++){ // iterate through all the rules
    for(int j = 0; j < bake->rules[i].cmd_count; j++){
      bake_cmd_postprocess(bake, &bake->rules[i].cmds[j]); // call bake_cmd_postprocess 
    } 
  }
}

