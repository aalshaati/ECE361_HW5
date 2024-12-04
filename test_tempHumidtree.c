#include <stdio.h>
#include <time.h>
#include "tempHumidtree.h"

int main() {
    TempHumidTreePtr_t tree = createTempHumidTree();
    if (!tree) {
        printf("ERROR: Could not create the tree.\n");
        return 1;
    }

    // Insert test data
    DataItem_t data1 = { .timestamp = 1672531200, .temp = 25, .humid = 60 }; // Example timestamp
    DataItem_t data2 = { .timestamp = 1672617600, .temp = 22, .humid = 55 };
    DataItem_t data3 = { .timestamp = 1672704000, .temp = 20, .humid = 50 };

    insert(tree, data1);
    insert(tree, data2);
    insert(tree, data3);

    // Search for a timestamp
    time_t search_time = 1672617600; // Example timestamp
    BSTNodePtr_t foundNode = search(tree, search_time);
    if (foundNode) {
        printf("Found node: Timestamp: %ld, Temp: %d, Humid: %d\n",
               foundNode->data.timestamp, foundNode->data.temp, foundNode->data.humid);
    }

    // Perform inorder traversal
    printf("Inorder Traversal:\n");
    inorder(tree);

    return 0;
}

