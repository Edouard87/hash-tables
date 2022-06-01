/**
 * @file hash_table.h
 * @author Edouard Desparois-Perrault (eperrault23@andover.edu)
 * @brief Header definitions for my implementation of hash tables in C.
 * @version 0.1
 * @date 2022-06-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

struct HT_node {
    struct HT_node * next;
    unsigned char* key;
    int value;
};

struct HT_ht {
    unsigned int capacity;
    struct HT_node ** nodes;
};

typedef struct HT_node HT_Node;
typedef struct HT_ht HT_Ht;

void HT_add(HT_Ht* h_table, char* key, int value);
int HT_check(HT_Ht* h_table, char* key);
void HT_print(HT_Ht*);
int HT_find(HT_Ht* h_table, char* key);
void HT_remove(HT_Ht* h_table, char* key);
void HT_change(HT_Ht*, char* key, int value);
void HT_destroy(HT_Ht* h_table);
unsigned int HT_hash(char* key, int size);
HT_Ht* HT_create(unsigned int size);
