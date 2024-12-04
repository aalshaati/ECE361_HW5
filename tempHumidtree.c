#include "tempHumidtree.h"
#include "float_rndm.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Create a new Temperature/Humidity tree
TempHumidTreePtr_t createTempHumidTree(void) {
    TempHumidTreePtr_t tree = (TempHumidTreePtr_t)malloc(sizeof(TempHumidTree_t));
    if (tree == NULL) {
        printf("ERROR[createTempHumidTree]: Memory allocation failed.\n");
        return NULL;
    }
    tree->root = NULL;
    tree->n = 0;
    return tree;
}

// Helper function to create a new BST node
static BSTNodePtr_t createNode(DataItem_t data) {
    BSTNodePtr_t newNode = (BSTNodePtr_t)malloc(sizeof(BSTNode_t));
    if (newNode == NULL) {
        printf("ERROR[createNode]: Memory allocation failed.\n");
        return NULL;
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Insert a new node into the tree
BSTNodePtr_t insert(TempHumidTreePtr_t tree, DataItem_t info) {
    if (tree == NULL) {
        printf("ERROR[insert]: The provided tree is NULL.\n");
        return NULL;
    }

    BSTNodePtr_t newNode = createNode(info);
    if (newNode == NULL) {
        return NULL; // Error message already handled in createNode()
    }

    if (tree->root == NULL) {
        tree->root = newNode;
    } else {
        BSTNodePtr_t current = tree->root, parent = NULL;

        while (current != NULL) {
            parent = current;
            if (info.timestamp < current->data.timestamp) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        if (info.timestamp < parent->data.timestamp) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }
    }

    tree->n++;
    return newNode;
}

// Search for a node in the tree by timestamp
BSTNodePtr_t search(TempHumidTreePtr_t tree, time_t timestamp) {
    if (tree == NULL || tree->root == NULL) {
        return NULL;
    }

    BSTNodePtr_t current = tree->root;

    while (current != NULL) {
        if (timestamp == current->data.timestamp) {
            return current;
        } else if (timestamp < current->data.timestamp) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return NULL; // Node not found
}

// Helper function for inorder traversal
static void _inorder(BSTNodePtr_t node) {
    if (node == NULL) {
        return;
    }

    _inorder(node->left);
    printf("Timestamp: %ld, Temp: %u, Humid: %u\n",
           node->data.timestamp, node->data.temp, node->data.humid);
    _inorder(node->right);
}

// Perform inorder traversal of the tree
void inorder(TempHumidTreePtr_t tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("The tree is empty.\n");
        return;
    }
    _inorder(tree->root);
}

// Populate the BST with random temperature/humidity data
void populateBST(TempHumidTreePtr_t tree, int numEntries) {
    if (tree == NULL) {
        printf("ERROR[populateBST]: The provided tree is NULL.\n");
        return;
    }

    time_t currentTimestamp = time(NULL); // Start with the current time
    for (int i = 0; i < numEntries; i++) {
        DataItem_t newItem;

        // Generate sequential timestamps
        newItem.timestamp = currentTimestamp + i;

        // Generate random temperature (scaled to 20-bit representation)
        double temp = float_rand_in_range(-50.0, 150.0); // Temp range: -50°C to 150°C
        newItem.temp = (uint32_t)(((temp + 50.0) / 200.0) * (1 << 20));

        // Generate random humidity (scaled to 20-bit representation)
        double humid = float_rand_in_range(0.0, 100.0); // Humidity range: 0% to 100%
        newItem.humid = (uint32_t)((humid / 100.0) * (1 << 20));

        // Insert the new data into the BST
        insert(tree, newItem);

#if _DEBUG
        printf("DEBUG[populateBST]: Inserted {timestamp: %ld, temp: %u, humid: %u}\n",
               newItem.timestamp, newItem.temp, newItem.humid);
#endif
    }
}
