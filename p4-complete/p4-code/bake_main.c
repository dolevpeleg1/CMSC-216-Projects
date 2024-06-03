#include "bake.h"

int main(int argc, char *argv[]){
  bake_t *bakefile;
  char *file_name;
  // process the command line arguments to determine if the default Bakefile or some other file should be loaded
  if(argc == 1 || (argc == 2 && strcmp(argv[1], "all") == 0) || (argc == 2 && strcmp(argv[1], "clean") == 0)){ // default bake
    file_name = "Bakefile";
  }
  else{ // specified file
    if(strcmp(argv[1], "-f") == 0){
      file_name = argv[2]; 
    }
    else{
      file_name = argv[1];
    }
  }

  bakefile = bake_create_from_file(file_name); // create bakefile from file
  // check for errors
  /*if(access(file_name, F_OK) != 0){
    printf("ERROR: unable to process file '%s'\n", file_name);
    bake_free(bakefile); // free the bakefile
    exit(EXIT_FAILURE);
  }*/
  if(bakefile == NULL){ // if the load fails print an error and exit with EXIT_FAILURE
    printf("ERROR: unable to process file '%s'\n", file_name);
    exit(EXIT_FAILURE);
  }
  if(bakefile->rule_count == 0){ // if the loaded file has no rules in it print an error and exit with EXIT_FAILURE
    printf("ERROR: '%s' has 0 rules\n", file_name);
    bake_free(bakefile); // free the bakefile
    exit(EXIT_FAILURE);
  }
  
  // add implicit rules
  bake_add_implicit_rules(bakefile);

  // post process the bakefile
  bake_post_process(bakefile);

  char *target;
  if((argc == 2 && strcmp(argv[1], "all") == 0 )|| (argc == 4 && strcmp(argv[3], "all") == 0 )){ // if target is "all"
    target = "all";
  }
  if((argc == 2 && strcmp(argv[1], "clean") == 0 )|| (argc == 4 && strcmp(argv[3], "clean") == 0 )){ // if target is "clean"
    target = "clean";
  }
  else{ // set target to the first rule
    target = bakefile->rules[0].target;
  }

  // set the updates
  int updates = bake_set_updates(bakefile, target); 
  if(updates == -1){  // check if failed
    printf("bake failed\n");
    bake_free(bakefile); // free the bakefile
    exit(EXIT_FAILURE);
  }
  else if(updates == 0){
    if(access(target, F_OK) == 0){ // file already exist
      printf("bake: file '%s' is up to date\n", target);
      bake_free(bakefile); // free the bakefile
      exit(0);
    }
    else{ // file does not exist
      printf("bake: nothing to be done for target '%s'\n", target);
      bake_free(bakefile); // free the bakefile
      exit(0);
    }
  }

  // do the updates
  updates = bake_do_updates(bakefile, target); 
  if(updates == -1){  // check if failed
    printf("bake failed\n");
    bake_free(bakefile); // free the bakefile
    exit(EXIT_FAILURE);
  }
  else{ // successful updates
    printf("bake complete, %d update(s) performed\n", updates);
    bake_free(bakefile); // free the bakefile
  }
   return 0;

























  /*
  if(argc == 1 || (argc == 2 && strcmp(argv[1], "all") == 0)){ // default bakefile
    bake_t *bakefile = bake_create_from_file("Bakefile"); // create bakefile from file

    if(bakefile == NULL){ // if the load fails print an error and exit with EXIT_FAILURE
      printf("ERROR: unable to process file 'Bakefile'\n");
      exit(EXIT_FAILURE);
    }
    if(bakefile->rule_count == 0){ // if the loaded file has no rules in it print an error and exit with EXIT_FAILURE
      printf("ERROR: 'Bakefile' has 0 rules\n");
      exit(EXIT_FAILURE);
    }
    
    // add implicit rules
    bake_add_implicit_rules(bakefile);

    // post process the bakefile
    bake_post_process(bakefile);

    // Check for updates needed
    if(bake_set_updates(bakefile, bakefile->rules[0].target) == 0){ // if no updates needed
      if(access("Bakefile", F_OK) == 0){ // file already exist
        printf("bake: file '%s' is up to date\n", bakefile->rules[0].target);
        exit(0);
      }
      else{ // file does not exist
        printf("bake: nothing to be done for target '%s'\n", bakefile->filename);
        exit(0);
      }
    }
    else{ // file need to be updated
      int updates = bake_do_updates(bakefile, bakefile->rules[0].target);
      printf("bake complete, %d update(s) performed\n", updates);
    }
    bake_free(bakefile); // free the bakefile
  }
  else{ // "bake filename/ bake filename all" command 
    char *filename = argv[1];
    bake_t *bakefile = bake_create_from_file(filename); // create bakefile from file specified
    
    if(bakefile == NULL){ // if the load fails print an error and exit with EXIT_FAILURE
      printf("ERROR: unable to process file '%s'\n", filename);
      exit(EXIT_FAILURE);
    }
    if(bakefile->rule_count == 0){ // if the loaded file has no rules in it print an error and exit with EXIT_FAILURE
      printf("ERROR: '%s' has 0 rules\n", filename);
      exit(EXIT_FAILURE);
    }
    
    // add implicit rules
    bake_add_implicit_rules(bakefile);

    // post process the bakefile
    bake_post_process(bakefile);

    if(argc == 2){ // "bake filename" command
      // Check for updates needed
      if(bake_set_updates(bakefile, filename) == 0){ // if no updates needed
        if(access(filename, F_OK) == 0){ // file already exist
          printf("bake: file '%s' is up to date\n", filename);
          exit(0);
        }
        else{ // file does not exist
          printf("bake: nothing to be done for target '%s'\n", filename);
          exit(0);
        }
      }
      else{ // file need to be updated
        int updates = bake_do_updates(bakefile, filename);
        if(updates == -1){ // if update failed print an error and exit with EXIT_FAILURE
          printf("bake failed");
          exit(EXIT_FAILURE);
        }
        printf("bake complete, %d update(s) performed\n", updates);
      }
      bake_free(bakefile); // free the bakefile
    }
  }
  */
}