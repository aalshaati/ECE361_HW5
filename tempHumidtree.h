/**
 * @file tempHumidtree.h
 * @brief Header file for Temp/Humidity BST
 */

#ifndef _TEMPHUMIDTREE_H
#define _TEMPHUMIDTREE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/***** typedefs, structs, etc *****/
// Data item struct
typedef struct _temp_humid_data_s {
    time_t timestamp;
    uint32_t temp;
    uint32_t humid;
} DataItem_t, *DataItemPtr_t;

// BST node struct
typedef struct _bstNode_s {
    DataItem_t data;
    struct _bstNode_s *left;
    struct _bstNode_s *right;
} BSTNode_t, *BSTNodePtr_t;

// Temp/Humidity BST struct
typedef struct _bstTree_s {
    BSTNodePtr_t root;
    int n;
} TempHumidTree_t, *TempHumidTreePtr_t;

/***** API FUNCTIONS *****/
TempHumidTreePtr_t createTempHumidTree(void);
BSTNodePtr_t insert(TempHumidTreePtr_t tree, DataItem_t info);
BSTNodePtr_t search(TempHumidTreePtr_t tree, time_t timestamp);
void inorder(TempHumidTreePtr_t tree);
void populateBST(TempHumidTreePtr_t tree, int numEntries);

#endif
