/**
 * @file hash_table.c
 * @author Edouard Desparois-Perrault (eperrault23@andover.com)
 * @brief An implementation of hash tables in C.
 * @version 0.1
 * @date 2022-05-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"

// Prime number for the hashing function. See `The C Programming Language Section Second Edition 6.6`.
#define HASHPRIME 31 

/**
 * @brief Hash the provided key and return the hashed value.
 * 
 * @param key - The key to hash.
 * @param size - The capacity of the hash table. NOTE: NOT the
 * amount of keys within the hash table.
 * @return int - The hashed value of the provided key.
 */
unsigned int HT_hash(char* key, int size) {
    unsigned int hashval;
    for (hashval = 0; *key != '\0'; key++) {
        hashval = *key + HASHPRIME * hashval;
    }
    return hashval % size;
}

/**
 * @brief Initializer function for the hash table.
 * 
 * @param size - The capacity of the hashtable to generate. A larger size
 * means less collisions, but a larger memory allocation.
 * 
 */
HT_Ht* HT_create(unsigned int size) {
    HT_Ht* hash_table = malloc(sizeof(HT_Ht));
    hash_table->capacity = size;
    hash_table->nodes = malloc(sizeof(HT_Node) * size);
    // Initializing all entries in the table to NULL may take longer, but
    // it's a one-off operation that saves some computations in the long term.
    // If the values are not explicitly set to NULL, we cannot rely on their
    // values.
    for (int i = 0; i < size; i++)
        hash_table->nodes[i] = NULL;
    return hash_table;
}

/**
 * @brief Helper function to recursively search a linked
 * list of nodes within a bucket of the hash table to find 
 * whether or not the provided value exists.
 * 
 * @param node - The root node of the linked list of nodes
 * to search.
 * @param key - The key to search within the provided linked list. 
 * @return int - 0 if not found, 1 if found.
 */
int _HT_check(HT_Node* node, char* key) {
    
    if (node == NULL) {
        return 0;
    }
    
    if (!strcmp(node->key,key)) {
        return 1;
    }
    return _HT_check(node->next, key);
}

/**
 * @brief Determine whether or not the provided key
 * exists within the hash table. NOTE: The function does
 * not return the value of the provided key -- only whether
 * or not it exists.
 * 
 * @param h_table - The hash table to search.
 * @param key - The key of the searched value.
 * @return int - 0 if not found, 1 if found.
 */
int HT_check(HT_Ht* h_table, char* key) {
    unsigned int hash_val = HT_hash(key, h_table->capacity);
    return _HT_check(h_table->nodes[hash_val], key);
}

/**
 * @brief A helper function to search the provided linked
 * list of nodes to find the value of a particular key. It is
 * assumed that the key is present. Use `HT_check` if unsure.
 * 
 * @param nodes - Pointer to the root of the linked list
 * of nodes.
 * @param key - The key to search.
 * @return int - The value of the provided key.
 */
int _HT_find(HT_Node* nodes, char* key) {
    if (!strcmp(nodes->key,key)) { // if 0 they are equal (idk why they wrote it this way).
        return nodes->value;
    }
    return _HT_find(nodes->next, key);
}

/**
 * @brief Find the value of a provided key within a linked
 * list and return it. NOTE: This will segfault if the value does
 * not exist.
 * 
 * @param h_table - The hash table to search.
 * @param key - The key to compare within the hash table.
 * @return int - The value of the provided key within the hash
 * table.
 */
int HT_find(HT_Ht* h_table, char* key) {
    unsigned int hash_val = HT_hash(key, h_table->capacity);
    return _HT_find(h_table->nodes[hash_val], key);
}

/**
 * @brief A helper function to find the provided key
 * within a hash table and return a pointer to its node.
 * 
 * @param h_table - The hash table to search.
 * @param s - The key within the hash table to compare. The node
 * containing the first matching key will be returned.
 * @return HT_Node* - The node containing the provided
 * key within the hash table.
 */
HT_Node* _lookup(HT_Ht* h_table, char* s) {
    HT_Node* np;
    for (np = h_table->nodes[HT_hash(s, h_table->capacity)]; np != NULL; np = np->next) {
        if (!strcmp(s, np->key))
            return np; /* found */
    }
    return NULL; /* not found */
}

/**
 * @brief A helper function to recurse through a linked
 * list of nodes in a bucket and print them out in a
 * human-readable format.
 * 
 * @param node - The root node of the linked list representing
 * a bucket of the hash table.
 */
void _HT_print(HT_Node* node) {
    if (!node) return;
    printf("{\"%s\": %d}\n",node->key, node->value);
    _HT_print(node->next);
}

/**
 * @brief Print the provided hash table to STDOUT in a human-readable
 * manner.
 * 
 * @param h_table - The table to be printed out.
 */
void HT_print(HT_Ht* h_table) {
    for (int x = 0; x < h_table->capacity; x++) {
        printf("=====BUCKET %d=====\n",x);
        if (h_table->nodes[x]) {
            _HT_print(h_table->nodes[x]);
        } else {
            printf("{{{EMPTY}}}\n");
        }
    }
}

/**
 * @brief Add a key-value pair to the provided hash table.
 * 
 * @param key - The key to be added.
 * @param value - The value to be added.
 */
void HT_add(HT_Ht* h_table, char* key, int value) {
    unsigned int hash_val = HT_hash(key, h_table->capacity);
    HT_Node* new_node = malloc(sizeof(HT_Node));
    char* new_key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(new_key, key);
    new_node->key = new_key;
    new_node->value = value;
    // We can do this because the values are initialized
    // to null.
    new_node->next = h_table->nodes[hash_val];
    h_table->nodes[hash_val] = new_node;
}

/**
 * @brief Change the value of an entry in the hash
 * table provided the key. NOTE: This assumes the value
 * already exists. This will cause errors if it dosen't.
 * 
 * @param h_table - The hash table to change.
 * @param key - The key of the value to change.
 * @param new_value - The new value to add.
 */
void HT_change(HT_Ht* h_table, char* key, int new_value) {
    HT_Node* node = _lookup(h_table,key);
    node->value = new_value;
}

/**
 * @brief Helper function to recursively remove a node given a key within a
 * bucket of a hash-table.
 * 
 * @param node - The root node of the linked list to search.
 * @param key - The key to compare within the linked list.
 */
void _HT_remove_node(HT_Node** node, char* key) {
    if (!strcmp((*node)->key,key)) {
        // The first node is the one that must be removed.
        free(*node);
        *node = NULL;
        return;
    }
    if (!strcmp((*node)->next->key,key)) {
        HT_Node* next = (*node)->next->next;
        HT_Node* cur = (*node)->next;
        HT_Node* prev = *node;
        prev->next = next;
        free(cur);
        return;
    }
    _HT_remove_node(&((*node)->next), key);
}

/**
 * @brief Remove a key-value pair from the hash table given
 * a key. The key's node will be deleted. NOTE: Assumes the
 * key already exists within the hash table. Will cause errors
 * if it dosen't.
 * 
 * @param h_table - The hash table on which the lookup is to
 * be performed.
 * @param key - The key of the key-value pair to be deleted. 
 */
void HT_remove(HT_Ht* h_table, char* key) {
    return _HT_remove_node(&(h_table->nodes[HT_hash(key,h_table->capacity)]), key);
}

/**
 * @brief A helper function to destroy an array
 * of nodes within a bucket within a hash table.
 * 
 * @param node - The first node in the linked
 * list of nodes to destroy.
 */
void _HT_destroy_nodes(HT_Node * node) {
    if (node == NULL) return;
    HT_Node* next = node->next;
    free(node->key); // Free string key.
    free(node);
    _HT_destroy_nodes(next);
}

/**
 * @brief Destroy the provided hash table.
 * 
 * @param h_table - The table to destroy.
 * 
 */
void HT_destroy(HT_Ht* h_table) {
    // Free each individual node to account for nodes
    // having been added. The entire block cannot
    // simply be removed because of `HT_add`.
    for (int cur = 0; cur < h_table->capacity; cur++) {
        if (!(h_table->nodes[cur] == NULL)) {
            // Exists.
            _HT_destroy_nodes(h_table->nodes[cur]);
        }
    }
    free(h_table); // Destroy the struct itself.
}
