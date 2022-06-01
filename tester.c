/**
 * @file tester.c
 * @author Edouard Desparois-Perault (eperrault23@andover.edu)
 * @brief Testing functions for my C hash table implementation.
 * @version 0.1
 * @date 2022-05-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "./hash_table.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* The maximum data value. */
#define MAX_VALUE 100

// ================
// HELPER FUNCTIONS
// ================

/**
 * A struct to encapsulate the data needed to
 * test a random hash table.
 */
struct rand_ht {
    HT_Ht* h_table;
    char** keys;
    int num_keys;
    int key_size;
    int* values;
};

/**
 * @brief Generate a random key of the provided
 * size. See https://stackoverflow.com/questions/15767691/whats-the-c-library-function-to-generate-random-string.
 * 
 * @param size - Length of the resulting key, exclusing the null character.
 * @return char* - The randomly generated key.
 */
char* _random_key(int size) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* key = malloc(sizeof(char) * (size+1)); // Create the array.
    for (int i = 0; i < size; i++) {
        size_t index = (double) rand() / RAND_MAX * (sizeof(charset) - 1);
        key[i] = charset[index];
    }
    key[size] = '\0'; // make sure it ends with null.
    return key;
}

/**
 * @brief Helper function to determine if a provided
 * key is within an array of keys.
 * 
 * @param key - The key to compare to the provided array of
 * keys.
 * @param keys - The array of keys to search
 * @param amount - The size of the provided array of keys.
 * @return int - 1 if it's inside the array, 0 if it's not.
 */
int _is_inside(char* key, char** keys, int amount) {
    for (int i = 0; i < amount; i++) {
        if (!strcmp(key, keys[0])) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Generates an array of unique keys not present in the
 * provided array of keys.
 * 
 * @param keys - The array of keys.
 * @param amount - The amount of keys to generate.
 * @param size - The size of each generated key.
 * @return char* - The array of keys with no overlapping keys.
 */
char** _random_keys_ex(char** keys, int amount, int size) {
    char** new_keys = malloc(sizeof(char*) * amount);
    int i = 0;
    while (i < amount) {
        char* new_key = _random_key(size);
        if (!_is_inside(new_key, keys, amount)) {
            new_keys[i] = new_key;
            i++;
        } else {
            free(new_key); // Will cause a memory leak if not freed.
        }
    }
    return new_keys;
}

/**
 * @brief A helper method to generate random
 * keys.
 * 
 * @param amount - The amount of keys to generaye
 * @param size - The size each key.
 * @return keys - The pointer to the created array
 * of keys.
 * 
 */
char** _random_keys(int amount, int size) {
    char** keys = malloc(sizeof(char*) * amount);
    for (int i = 0; i < amount; i++) {
        keys[i] = _random_key(size);
    }
    return keys;
}

/**
 * @brief Generate an array of random values.
 * 
 * @param amount - The amount of random values to generate.
 * @param max_size - The max size of each value.
 * @return int* - The pointer to the array of values.
 */
int* _random_values(int amount, int max_size) {
    int* vals = malloc(sizeof(int) * amount);
    for (int i = 0; i < amount; i++) {
        vals[i] = rand() % max_size;
    }
    return vals;
}

/**
 * @brief Destroy the provided array of keys. This is necessary
 * as each string is in effect an array of characters, making the array
 * of keys a 2D array.
 * 
 * @param keys - The array of keys to destroy.
 * @param size - The size of the provided target array.
 */
void _destroy_keys(char** keys, int size) {
    for (int i = 0; i < size; i++) {
        free(keys[i]);
    }
}

/**
 * @brief Generate a hash table made up of
 * random values.
 * 
 * @param size - The size of the random
 * hash table to generate. NOTE: This is the size of the
 * memory block itself, not the length of the keys.
 * @param amount_keys - The amount of keys to add to the generated hash
 * table.
 * @param key_size - The size of each key within the hash table.
 * @return struct _rand_ht* - The resulting random hash table, 
 * which encapsulates both the hash table itself and any additional data
 * necessary for testing.
 */
struct rand_ht* _random_ht(unsigned int size, unsigned int amount_keys, int key_size) {
    struct rand_ht* rand_data = malloc(sizeof(struct rand_ht));
    rand_data->h_table = HT_create(size);
    rand_data->keys = _random_keys(amount_keys, key_size);
    rand_data->values = _random_values(amount_keys, MAX_VALUE);
    rand_data->num_keys = amount_keys;
    rand_data->key_size = key_size;
    // Generate keys
    for (int i = 0; i < amount_keys; i++) {
        HT_add(rand_data->h_table, rand_data->keys[i], rand_data->values[i]);
    }
    return rand_data;
}

/**
 * @brief A helper function to completely and properly destroy
 * a provided random hash table. Destroys both the hash table itself
 * and any additional encapsulated data (i.e. keys).
 * 
 * @param ht - The random hash table to destroy.
 */
void _destroy_random_ht(struct rand_ht* ht) {
    _destroy_keys(ht->keys, ht->num_keys); // Each key must be destroyed.
    HT_destroy(ht->h_table);
    free(ht->keys);
    free(ht->values);
    free(ht);
}

/**
 * @brief A helper function to create a random hash table and find values
 * within. It allows various parameters to be modfied for reuse accross tests.
 * 
 * @param size - The size of the hash table itself.
 * @param amount_keys - The amount of keys that should be added to the hash table.
 * @param key_size - The size of each key within the hash table.
 */
void _test_find(int size, int amount_keys, int key_size) {
    struct rand_ht* ht_rand = _random_ht(size, amount_keys,key_size);
    for (int i = 0; i < amount_keys; i++) {
        int expected_val = HT_find(ht_rand->h_table, ht_rand->keys[i]);
        CU_ASSERT(expected_val == ht_rand->values[i]);
    }
    _destroy_random_ht(ht_rand);
}

// ======
// TESTS
// ======

/**
 * @brief Basic testing of the hash function. NOTE: This hash
 * function in demo projects, so I'm quite certain that it works. This
 * is more a proof of my implementation than an actual test.
 * 
 * @return int - 0 if fail, 1 if success.
 */
int test_hash(void) {
    const int ATTEMPTS = 10;
    const int HASH_SIZE = 10;
    // The same value should produce the same hash.
    char** keys = _random_keys(ATTEMPTS,30);
    for (int i = 0; i < ATTEMPTS; i++) {
        int hash = HT_hash(keys[i], HASH_SIZE);
        for (int j = 0; j < 3; j++) {
            CU_ASSERT(hash == HT_hash(keys[i], ATTEMPTS));
        }
    }
    // Changing one letter should dramatically alter
    // the output, resulting in a different index.
    for (int i = 0; i < ATTEMPTS; i++) {
        int index1 = HT_hash(keys[i], HASH_SIZE);
        char* key = keys[i];
        key[10]++;
        int index2 = HT_hash(keys[i], HASH_SIZE);
        // Changing a letter should produce different indices.
        CU_ASSERT(index1 != index2);
    }
    _destroy_keys(keys, ATTEMPTS);
}

/**
 * @brief Adds and removes from a random hash
 * table.
 * 
 * @return int - 0 if fail, 1 if pass.
 */
int test_add_find(void) {
    _test_find(10,10,30);
}

/**
 * @brief A tester to force collisions and test how 
 * they are handled to ensure that the collision-handling
 * machinery is functional.
 * 
 * @return int - 0 if fail, 1 if success.
 */
int test_collision(void) {
    // With a list of 2 and a size of 100 keys, there will definitely be collisions.
    _test_find(2,100,30);
}

/**
 * @brief Testing the check function on a random HT. Tests for both the existence of 
 * keys that exist and keys that do not.
 * 
 * @return int - 0 if fail, 1 if success.
 */
int test_check(void) {
    const int AMOUNT_KEYS = 30;
    const int SIZE_KEYS = 30;
    struct rand_ht* ht_rand = _random_ht(10,AMOUNT_KEYS,30);
    // These keys exist.
    for (int i = 0; i < AMOUNT_KEYS; i++) {
        CU_ASSERT(HT_check(ht_rand->h_table,ht_rand->keys[i]));
    }
    // These keys do not exist by definition.
    char** nonexistent_keys = _random_keys_ex(ht_rand->keys,AMOUNT_KEYS, SIZE_KEYS);
    for (int i = 0; i < AMOUNT_KEYS; i++) {
        CU_ASSERT(!HT_check(ht_rand->h_table,nonexistent_keys[i]));
    }
    _destroy_keys(nonexistent_keys, AMOUNT_KEYS);
    _destroy_random_ht(ht_rand);
}

/**
 * @brief Testing the remove function on elements within a hash table.
 * It generates the nodes to be removed, then it removes them, and checks whether
 * they are still accessible. This function cannot perform tests for memory
 * leaks -- that task is reserved for Valgrind.
 * 
 * @return int - 0 if fail, 1 if success.
 */
int test_remove(void) {
    const int AMOUNT_KEYS = 10;
    const int KEY_SIZE = 30;
    struct rand_ht* ht_rand = _random_ht(5,AMOUNT_KEYS,AMOUNT_KEYS);
    char** nonexistant_keys = _random_keys_ex(ht_rand->keys, AMOUNT_KEYS, AMOUNT_KEYS);
    int* nonexistant_values = _random_values(AMOUNT_KEYS,1000);
    for (int i = 0; i < AMOUNT_KEYS; i++) {
        HT_remove(ht_rand->h_table,ht_rand->keys[i]);
        CU_ASSERT(!HT_check(ht_rand->h_table,ht_rand->keys[i])); // Should not exist.
    }
    // The hash table should still be usable after values
    // have been removed.
    for (int i = 0; i < AMOUNT_KEYS; i++) {
        HT_add(ht_rand->h_table,nonexistant_keys[i],nonexistant_values[i]);
    }
    _destroy_keys(nonexistant_keys, AMOUNT_KEYS);
    _destroy_random_ht(ht_rand);
}

/**
 * @brief Testing the value of the change function. Attempts to change
 * the values of provided keys, then checks whether the keys were successfully changed
 * by retrieving their values.
 * 
 * @return int - 0 if fail. 1 if success.
 */
int test_change(void) {
    srand(time(NULL)); // Seed the random number generator.
    const int HASH_TABLE_SIZE = 30;
    const int AMOUNT_KEYS = 100;
    const int KEY_SIZE = 30;
    struct rand_ht* tab = _random_ht(HASH_TABLE_SIZE,AMOUNT_KEYS,KEY_SIZE);
    int* new_values = _random_values(AMOUNT_KEYS, 1000);
    for (int i = 0; i < AMOUNT_KEYS; i++) {
        HT_change(tab->h_table,tab->keys[i], new_values[i]);
        CU_ASSERT(HT_find(tab->h_table,tab->keys[i]) == new_values[i]);
    }
    _destroy_random_ht(tab);
    free(new_values);
}

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Main Tests", NULL, NULL);
    CU_ADD_TEST(suite, test_add_find);
    CU_ADD_TEST(suite, test_collision);
    CU_ADD_TEST(suite, test_check);
    CU_ADD_TEST(suite, test_change);
    CU_ADD_TEST(suite, test_remove);
    CU_ADD_TEST(suite, test_hash);
    CU_basic_run_tests();
    CU_cleanup_registry();
}
