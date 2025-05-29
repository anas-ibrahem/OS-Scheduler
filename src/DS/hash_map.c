#include "hash_map.h"

size_t hashmap_int_hash(int key) {
    // Knuth's multiplicative method
    return (size_t)((key * 2654435761UL) >> 16);
}

HashMap* hashmap_create(size_t capacity) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (map == NULL) return NULL;
    
    map->buckets = (HashNode**)calloc(capacity, sizeof(HashNode*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    map->capacity = capacity;
    map->size = 0;

    return map;
}


int hashmap_put(HashMap* map, int key, void* value) {
    if (map == NULL) return -1;
    if(map->size >= map->capacity) return -1; // HashMap is full
    
    // Get hash index
    size_t index = hashmap_int_hash(key) % map->capacity;
    size_t original_index = index;
    size_t step = 1;
    
    // Check if key already exists
    HashNode* current = map->buckets[index];
    while (current) {
        if (current->key == key) {
            // Key exists, update value
            current->value = value;
            return 0;
        }
        index = (original_index + step * step) % map->capacity; // Quadratic probing
        step++;
        current = map->buckets[index];
        if (index == original_index) { // Full circle, no space
            return -1;
        }
    }
    
    // Insert new key-value pair
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) return -1;
    new_node->key = key;
    new_node->value = value;
    map->buckets[index] = new_node;
    map->size++;

    return 1;
}


void* hashmap_get(HashMap* map, int key) {
    if (map == NULL) return NULL;
    
    // Get hash index
    size_t index = hashmap_int_hash(key) % map->capacity;
    size_t original_index = index;
    size_t step = 1;
    
    HashNode* current = map->buckets[index];
    while (current) {
        if (current->key == key) {
            return current->value; // Key found
        }
        index = (original_index + step * step) % map->capacity; // Quadratic probing
        step++;
        current = map->buckets[index];
        if (index == original_index) { // Full circle, key not found
            break;
        }
    }
    
    return NULL; // Key not found
}
int hashmap_remove(HashMap* map, int key) {
    if (map == NULL) return -1;
    
    // Get hash index
    size_t index = hashmap_int_hash(key) % map->capacity;
    size_t original_index = index;
    size_t step = 1;
    
    HashNode* current = map->buckets[index];
    while (current) {
        if (current->key == key) {
            free(current); // Free the node
            map->buckets[index] = NULL; // Remove from bucket
            map->size--;
            return 0; // Key removed
        }
        index = (original_index + step * step) % map->capacity; // Quadratic probing
        step++;
        current = map->buckets[index];
        if (index == original_index) { // Full circle, key not found
            break;
        }
    }
    
    return -1; // Key not found
}
void hashmap_free(HashMap* map) {
    if (map == NULL) return;
    
    for (size_t i = 0; i < map->capacity; i++) {
        HashNode* current = map->buckets[i];
        if(current != NULL) {
            free(current);
        }
    }
    
    free(map->buckets);
    free(map);
}