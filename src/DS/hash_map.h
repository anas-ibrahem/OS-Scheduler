#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>

/**
 * @brief Node in the hash map containing key-value pair
 */
typedef struct HashNode {
    int key;
    void* value;
} HashNode;

/**
 * @brief Hash map data structure
 */
typedef struct {
    HashNode** buckets;
    size_t capacity;
    size_t size;
} HashMap;

/**
 * @brief Hash function for integer keys
 * 
 * @param key Integer key to hash
 * @return size_t Hashed value
 */
size_t hashmap_int_hash(int key);

/**
 * @brief Create a new hash map
 * 
 * @param capacity Initial capacity of the hash map
 * @return HashMap* Pointer to the created hash map or NULL if allocation fails
 */
HashMap* hashmap_create(size_t capacity);

/**
 * @brief Insert or update a key-value pair in the hash map
 * 
 * @param map The hash map
 * @param key The key
 * @param value Pointer to the value
 * @return int 0 on success, -1 on failure
 */
int hashmap_put(HashMap* map, int key, void* value);

/**
 * @brief Retrieve a value by key from the hash map
 * 
 * @param map The hash map
 * @param key The key to look up
 * @return void* Pointer to the value or NULL if key not found
 */
void* hashmap_get(HashMap* map, int key);

/**
 * @brief Remove a key-value pair from the hash map
 * 
 * @param map The hash map
 * @param key The key to remove
 * @return int 0 on success, -1 if key not found
 */
int hashmap_remove(HashMap* map, int key);

/**
 * @brief Free all resources used by the hash map
 * 
 * @param map The hash map to free
 */
void hashmap_free(HashMap* map);

#endif /* HASH_MAP_H */