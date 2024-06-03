// main function for a linked list manager, student version

#include <stdio.h>
#include <string.h>
#include "hashmap.h"

int main(int argc, char *argv[]){
  int echo = 0;                                // controls echoing, 0: echo off, 1: echo on
  if(argc > 1 && strcmp("-echo",argv[1])==0) { // turn echoing on via -echo command line option
    echo=1;
  }

  printf("Hashmap Main\n");
  printf("Commands:\n");
  printf("   hashcode <key>   : prints out the numeric hash code for the given key (does not change the hash map)\n");
  printf("   put <key> <val>  : inserts the given key/val into the hash map, overwrites existing values if present\n");
  printf("   get <key>        : prints the value associated with the given key or NOT FOUND\n");
  printf("   print            : shows contents of the hashmap ordered by how they appear in the table\n");
  printf("   structure        : prints detailed structure of the hash map\n");
  printf("   clear            : reinitializes hash map to be empty with default size\n");
  printf("   save <file>      : writes the contents of the hash map the given file\n");
  printf("   load <file>      : clears the current hash map and loads the one in the given file\n");
  printf("   next_prime <int> : if <int> is prime, prints it, otherwise finds the next prime and prints it\n");
  printf("   expand           : expands memory size of hashmap to reduce its load factor\n");
  printf("   quit             : exit the program\n");
  
  char cmd[128];
  hashmap_t map;
  int success;
  hashmap_init(&map, HASHMAP_DEFAULT_TABLE_SIZE);
  while(1){
    success = fscanf(stdin,"%s",cmd); // read a command
    if(success==EOF){                 // check for end of input
      printf("HM>");                   // found end of input
      break;                          // break from loop
    }

    if( strcmp("quit", cmd)==0 ){     // check for exit command
      if(echo){
        printf("quit\n");
      }
      break;                          // break from loop
    }

    else if( strcmp("hashcode", cmd)==0 ){ // hashcode command
      fscanf(stdin,"%s",cmd);            // read string to insert
      if(echo){
        printf("hashcode %s\n",cmd);
      }
      printf("%ld\n", hashcode(cmd));
    }

    else if( strcmp("put", cmd)==0 ){ // put command
      fscanf(stdin,"%s",cmd); // read string to insert
      char val[128];
      fscanf(stdin,"%s",val); // read value to insert
      if(echo){
        printf("put %s %s\n",cmd, val);
      }
      success = hashmap_put(&map, cmd, val); // call put function
      if(!success){                      // check for success
        printf("Overwriting previous key/val\n");
      }
    }

    else if( strcmp("get", cmd)==0 ){     // get command
      fscanf(stdin,"%s",cmd);          // read the key
      if(echo){
        printf("get %s\n",cmd);
      }
      char *val = hashmap_get(&map, cmd); // call get function
      if(val == NULL){   // check if found
        printf("NOT FOUND\n");
      }
      else {
            printf("FOUND: %s\n",val);
            free(val); // free the string
      }
    }

    else if( strcmp("print", cmd)==0 ){   // print command
      if(echo){
        printf("print\n");
      }
      for(int i = 0; i < map.table_size; i++){
        hashnode_t *node = map.table[i]; // get the node in the position
        while(node != NULL){ // if the item in the position is not NULL
            printf("%12s : %s\n", node->key, node->val);
            node = node->next; // set the node to the next node
        }
      }
    }

    else if( strcmp("structure", cmd)==0 ){   // structure command
      if(echo){
        printf("structure\n");
      }
      hashmap_show_structure(&map);
    }

    else if( strcmp("clear", cmd)==0 ){   // clear command
      if(echo){
        printf("clear\n");
      }
      hashmap_free_table(&map);
      hashmap_init(&map, HASHMAP_DEFAULT_TABLE_SIZE);
    }
    
    else if( strcmp("save", cmd)==0 ){     // save command
      fscanf(stdin,"%s",cmd);          // read the file name
      if(echo){
        printf("save %s\n",cmd);
      }
      hashmap_save(&map,cmd); // call the hashap save func
    }

    else if( strcmp("load", cmd)==0 ){     // load command
      fscanf(stdin,"%s",cmd);          // read the file name
      if(echo){
        printf("load %s\n",cmd);
      }
      int success = hashmap_load(&map, cmd); // call the hashmap load func
      if(!success){ // check for success
        printf("load failed\n");
      }
    }

    else if( strcmp("next_prime", cmd)==0 ){   // next prime command
      int prime;
      fscanf(stdin,"%d", &prime); // read value to insert
      if(echo){
        printf("next_prime %d\n", prime);
      }
      printf("%d\n", next_prime(prime)); // call the next prime func
    }

    else if( strcmp("expand", cmd)==0 ){   // expand command
      if(echo){
        printf("expand\n");
      }
      hashmap_expand(&map);
    }

    else{                                 // unknown command
      if(echo){
        printf("%s\n",cmd);
      }
      printf("unknown command %s\n",cmd);
    }
  }
  // end main while loop
  hashmap_free_table(&map);                      // clean up the list
  return 0;
}
