#include "hashmap.h"
// hashmap_funcs.c: utility functions for operating on hash maps. Most
// functions are used in hash_main.c which provides an application to
// work with the functions.

long hashcode(char key[]){
// PROVIDED: Compute a simple hash code for the given character
// string. The code is "computed" by casting the first 8 characters of
// the string to a long and returning it. The empty string has hash
// code 0. If the string is a single character, this will be the ASCII
// code for the character (e.g. "A" hashes to 65).  Longer strings
// will have numbers which are the integer interpretation of up to
// their first 8 bytes.  ADVANTAGE: constant time to compute the hash
// code. DISADVANTAGE: poor distribution for long strings; all strings
// with same first 8 chars hash to the same location.
  union {
    char str[8];
    long num;
  } strnum;
  strnum.num = 0;

  for(int i=0; i<8; i++){
    if(key[i] == '\0'){
      break;
    }
    strnum.str[i] = key[i];
  }
  return strnum.num;
}

void hashmap_init(hashmap_t *hm, int table_size){
// Initialize the hash map 'hm' to have given size and item_count
// 0. Ensures that the 'table' field is initialized to an array of
// size 'table_size' and filled with NULLs. 
  hm->item_count = 0; // set item count to 0
  hm->table_size = table_size;
  hm->table = malloc(8 * table_size); // allocate memory for the pointers table
  for(int i = 0; i < table_size; i++){ // fill the table with NULLs
    hm->table[i] = NULL;
  }
}

int hashmap_put(hashmap_t *hm, char key[], char val[]){
// Adds given key/val to the hash map. 'hashcode(key) modulo
// table_size' is used to calculate the position to insert the
// key/val.  Searches the entire list at the insertion location for
// the given key. If key is not present, a new node is added. If key
// is already present, the current value is altered in place to the
// given value "val" (no duplicate keys are every introduced).  If new
// nodes are added, increments field "item_count".  Makes use of
// standard string.h functions like strcmp() to compare strings and
// strcpy() to copy strings. Lists in the hash map are arbitrarily
// ordered (not sorted); new items are always appended to the end of
// the list.  Returns 1 if a new node is added (new key) and 0 if an
// existing key has its value modified.

  int position = hashcode(key) % hm->table_size; // get the position

  if(hm->table[position] == NULL){ // if the current position is NULL, fill it with the new node
    hashnode_t *node = malloc(sizeof(hashnode_t)); // create a new node with the key and value
    strcpy(node->key, key);
    strcpy(node->val, val);
    node->next = NULL;
    hm->table[position] = node;
  }
  else{ // if current position is not empty
    hashnode_t *current_node = hm->table[position];
    while(current_node != NULL){ // itterate until found the last node
      if(strcmp(current_node->key, key) == 0){ // if the key already exist, change its value, return 1
        strcpy(current_node->val, val);
        return 0;
      }
      if(current_node->next != NULL){
        current_node = current_node->next;
      }
      else{
        hashnode_t *node = malloc(sizeof(hashnode_t)); // create a new node with the key and value
        strcpy(node->key, key);
        strcpy(node->val, val);
        node->next = NULL;
        current_node->next = node; // add the new node to the end of the list
        break;
      }
    }
  }
  hm->item_count++; // increment the item count
  return 1;
}

char *hashmap_get(hashmap_t *hm, char key[]){
// Looks up value associated with given key in the hashmap. Uses
// hashcode() and field "table_size" to determine which index in table
// to search.  Iterates through the list at that index using strcmp()
// to check for matching key. If found, returns a pointer to the
// associated value.  Otherwise returns NULL to indicate no associated
// key is present.
  if(hm->item_count == 0){ // if the table is empty, return NULL
    return NULL;
  }
  int position = hashcode(key) % hm->table_size; // get the position
  if(hm->table[position] == NULL){ // if there is nothing in this position, return NULL
    return NULL;
  }
  hashnode_t node = *hm->table[position]; // get the node in the position
  while(1){
    if(strcmp(node.key,key) == 0){ // the key matches, set item the value
      if(node.val != NULL){ // if the value is not NULL, copy it
        char* item = malloc(strlen(node.val) + 1); // create a string to return
        strcpy(item, node.val);
        return item;
      }
    }
    else if(node.next != NULL){
      node = *node.next; 
    }
    else{
      return NULL;
    }
  }
}

void hashmap_free_table(hashmap_t *hm){
// De-allocates the hashmap's "table" field. Iterates through the
// "table" array and its lists de-allocating all nodes present
// there. Subsequently de-allocates the "table" field and sets all
// fields to 0 / NULL. Does NOT attempt to free 'hm' as it may be
// stack allocated.

  for(int i = 0; i < hm->table_size; i++){ // itterate through the table
    hashnode_t *node = hm->table[i]; // get the node in the position
    while(node != NULL){ // if the item in the position is not NULL
      hashnode_t *next_node = node->next; // get the next node in the position
      free(node); // free the node
      node = next_node; // set the node to the next node
      }
    }
  free(hm->table);
  hm->item_count = 0; // set the item count to 0
  hm->table_size = 0; // set the table size to 0
}
 
void hashmap_show_structure(hashmap_t *hm){
// Displays detailed structure of the hash map. Shows stats for the
// hash map as below including the load factor (item count divided
// by table_size) to 4 digits of accuracy.  Then shows each table
// array index ("bucket") on its own line with the linked list of
// key/value pairs on the same line. The hash code for keys is appears
// prior to each key.  EXAMPLE:
// 
// item_count: 6
// table_size: 5
// load_factor: 1.2000
//   0 : {(65) A : 1} 
//   1 : 
//   2 : {(122) z : 3} {(26212) df : 6} 
//   3 : {(98) b : 2} {(25443) cc : 5} 
//   4 : {(74) J : 4} 
//
// NOTES:
// - Uses format specifier "%3d : " to print the table indices
// - Shows the following information for each key/val pair
//   {(25443) cc : 5}
//     |      |    |
//     |      |    +-> value
//     |      +-> key
//     +-> hashcode("cc"), print using format "%ld" for 64-bit longs

  printf("item_count: %d\n", hm->item_count);
  printf("table_size: %d\n", hm->table_size);
  double load_factor = 0;
  if(hm->table_size != 0){
    load_factor = (double)hm->item_count / hm->table_size;
  }
  printf("load_factor: %.4f\n", load_factor);

  for(int i = 0; i < hm->table_size; i++){ // print the table
    printf("%3d : ", i);
    hashnode_t *node = hm->table[i]; // get the node in the position
    while(node != NULL){ // if the item in the position is not NULL
      printf("{(%ld) %s : %s} ", hashcode(node->key), node->key, node->val); // print the current node)
      node = node->next; // set the node to the next node
      }
      printf("\n");
  }
}

void hashmap_write_items(hashmap_t *hm, FILE *out){
// Outputs all elements of the hash table according to the order they
// appear in "table". The format is
// 
//       Peach : 3.75
//      Banana : 0.89
//  Clementine : 2.95
// DragonFruit : 10.65
//       Apple : 2.25
// 
// with each key/val on a separate line. The format specifier
//   "%12s : %s\n"
// 
// is used to achieve the correct spacing. Output is done to the file
// stream 'out' which is standard out for printing to the screen or an
// open file stream for writing to a file as in hashmap_save().
  for(int i = 0; i < hm->table_size; i++){ // iterate through the table
   hashnode_t *node = hm->table[i]; // get the node in the position
   while(node != NULL){ // if the item in the position is not NULL
    fprintf(out, "\n%12s : %s", node->key, node->val);
    node = node->next; // set the node to the next node
   }
  }
}

void hashmap_save(hashmap_t *hm, char *filename){
// Writes the given hash map to the given 'filename' so that it can be
// loaded later.  Opens the file and writes its 'table_size' and
// 'item_count' to the file. Then uses the hashmap_write_items()
// function to output all items in the hash map into the file.
// EXAMPLE FILE:
// 
// 5 6
//            A : 2
//            Z : 2
//            B : 3
//            R : 6
//           TI : 89
//            T : 7
// 
// First two numbers are the 'table_size' and 'item_count' field and
// remaining text are key/val pairs.
  FILE *fh = fopen(filename, "w"); // open the file for writing
  fprintf(fh, "%d %d", hm->table_size, hm->item_count); // write the table size and the item count
  hashmap_write_items(hm, fh); // write the items
  fclose(fh); // close the file
}

int hashmap_load(hashmap_t *hm, char *filename){
// Loads a hash map file created with hashmap_save(). If the file
// cannot be opened, prints the message
// 
// ERROR: could not open file 'somefile.hm'
//
// and returns 0 without changing anything. Otherwise clears out the
// current hash map 'hm', initializes a new one based on the size
// present in the file, and adds all elements to the hash map. Returns
// 1 on successful loading. This function does no error checking of
// the contents of the file so if they are corrupted, it may cause an
// application to crash or loop infinitely.
  FILE *fh = fopen(filename, "r"); // open the file for reading
  if(fh == NULL){ // if file could not be opened
    printf("ERROR: could not open file '%s'\n", filename);
    return 0;
  }
  hashmap_free_table(hm); // clear out the map
  int table_size;
  int item_count;
  fscanf(fh, "%d", &table_size); // get the table size from the file
  fscanf(fh, "%d", &item_count); // get the item count from the file
  hashmap_init(hm, table_size); // initiate the map accordign to the table size
  char key[128];
  char val[128];
  while(fscanf(fh, " %s : %s", key, val) != EOF){ // read from the file until reach EOF
    hashmap_put(hm, key, val); // put the new node in
  }
  fclose(fh); // close the file
  return 1;
}

int next_prime(int num){
// If 'num' is a prime number, returns 'num'. Otherwise, returns the
// first prime that is larger than 'num'. Uses a simple algorithm to
// calculate primeness: check if any number between 2 and (num/2)
// divide num. If none do, it is prime. If not, tries next odd number
// above num. Loops this approach until a prime number is located and
// returns this. Used to ensure that hash table_size stays prime which
// theoretically distributes elements better among the array indices
// of the table.
  while(1){ // else
    for(int i = 2; i <= (num / 2); i++){ // check if the number is divisble by another number
      if((num % i) == 0){ // if it is divisible, break the for loop
        break;
      }
      if(i == num / 2){ // if this is the last iteration, return the number
        return num;
      }
    }
    if(num % 2 == 0){ // of the number is even, incement it by 1
      num++;
    }
    else{ // else, increment it by 2
      num += 2;
    }
  }
}

void hashmap_expand(hashmap_t *hm){
// Allocates a new, larger area of memory for the "table" field and
// re-adds all items currently in the hash table to it. The size of
// the new table is next_prime(2*table_size+1) which keeps the size
// prime.  After allocating the new table, all entries are initialized
// to NULL then the old table is iterated through and all items are
// added to the new table according to their hash code. The memory for
// the old table is de-allocated and the new table assigned to the
// hashmap fields "table" and "table_size".  This function increases
// "table_size" while keeping "item_count" the same thereby reducing
// the load of the hash table. Ensures that all memory associated with
// the old table is free()'d (linked nodes and array). Cleverly makes
// use of existing functions like hashmap_init(), hashmap_put(),
// and hashmap_free_table() to avoid re-writing algorithms
// implemented in those functions.
  hashnode_t **old_table = hm->table;
  int old_size = hm->table_size;
  hashmap_init(hm, next_prime(2 * old_size + 1));

  for(int i = 0; i < old_size; i++){ // itterate through the old table
    hashnode_t *node = old_table[i]; // get the node in the position
    while(node != NULL){ // if the item in the position is not NULL
      hashmap_put(hm, node->key, node->val);
      hashnode_t *next_node = node->next; // get the next node in the position
      free(node); // free the node
      node = next_node; // set the node to the next node
      }
    }
  free(old_table);
}
