#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "iom361_r2.h"
#include "tempHumidtree.h"

// Define constants
#define TEMP_RANGE_LOW 42.0
#define TEMP_RANGE_HI  52.0
#define HUMID_RANGE_LOW 72.6
#define HUMID_RANGE_HI 87.3
#define MAX_ENTRIES 100

// Function prototypes
void populateBSTFromIO(TempHumidTreePtr_t tree, int numEntries);

int main() {
    // Initialize variables
    TempHumidTreePtr_t tempHumidTree;
    int rtn_code, numEntries;
    int startMonth, startDay;

    // Greet user and initialize the I/O module
    printf("Populating the Temperature/Humidity BST using I/O module data...\n");
    iom361_initialize(16, 16, &rtn_code);
    if (rtn_code != 0) {
        printf("FATAL(main): Could not initialize I/O module\n");
        return 1;
    }

    // Get user input for the number of entries and start date
    printf("Enter the starting month (1 to 12), day (1 to 31), and number of days (1 to 100): ");
    scanf("%d,%d,%d", &startMonth, &startDay, &numEntries);

    if (numEntries <= 0 || numEntries > MAX_ENTRIES) {
        printf("ERROR(main): Invalid number of entries. Must be between 1 and %d.\n", MAX_ENTRIES);
        return 1;
    }

    printf("User requested %d data items starting at %d/%d/2023\n", numEntries, startMonth, startDay);

    // Create the BST
    tempHumidTree = createTempHumidTree();
    if (tempHumidTree == NULL) {
        printf("FATAL(main): Could not create Temperature/Humidity tree\n");
        return 1;
    }

    // Populate the BST from I/O module data
    populateBSTFromIO(tempHumidTree, numEntries);

    // Perform in-order traversal of the tree
    printf("\nPerforming in-order traversal of the tree:\n");
    inorder(tempHumidTree);

    // Allow user to search for data by timestamp
    time_t searchTimestamp;
    struct tm searchDate = {0};

    while (true) {
        printf("\nEnter a search date (mm/dd/yyyy): ");
        int res = scanf("%d/%d/%d", &searchDate.tm_mon, &searchDate.tm_mday, &searchDate.tm_year);
        if (res != 3) break;

        // Convert the entered date to a timestamp
        searchDate.tm_year -= 1900; // Adjust year
        searchDate.tm_mon -= 1;    // Adjust month (0-based)
        searchDate.tm_hour = 13;   // Use 1:00 PM for all timestamps
        searchTimestamp = mktime(&searchDate);

        if (searchTimestamp == -1) {
            printf("ERROR(main): Invalid date entered\n");
            continue;
        }

        // Search for the timestamp in the BST
        BSTNodePtr_t result = search(tempHumidTree, searchTimestamp);
        if (result == NULL) {
            printf("Did not find data for Timestamp %02d-%02d-%04d\n",
                   searchDate.tm_mday, searchDate.tm_mon + 1, searchDate.tm_year + 1900);
        } else {
            // Display the found data
            DataItem_t data = result->data;
            float temp = (data.temp / powf(2, 20)) * 200.0 - 50;
            float humid = (data.humid / powf(2, 20)) * 100;
            printf("Found data for Timestamp %02d-%02d-%04d\n", searchDate.tm_mday, searchDate.tm_mon + 1, searchDate.tm_year + 1900);
            printf("%02d-%02d-%04d\t%4.1fF(%08X) %4.1f%%(%08X)\n",
                   searchDate.tm_mday, searchDate.tm_mon + 1, searchDate.tm_year + 1900,
                   temp, data.temp, humid, data.humid);
        }
    }

    printf("\nINFO(main): End search for data items in BST\n");

    // Perform a final in-order traversal and display the temperature/humidity table
    printf("\nTemperature/Humidity table:\n");
    printf("---------------------------\n");
    inorder(tempHumidTree);

    return 0;
}

/**
 * populateBSTFromIO() - Populates a BST with data from the I/O module
 *
 * @param tree: Pointer to the Temperature/Humidity tree
 * @param numEntries: Number of entries to populate in the tree
 */
void populateBSTFromIO(TempHumidTreePtr_t tree, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        DataItem_t data;
        time_t timestamp = 1690833600 + (i * 86400); // Start from July 31, 2023

        // Generate random temperature and humidity
        _iom361_setSensor1_rndm(TEMP_RANGE_LOW, TEMP_RANGE_HI, HUMID_RANGE_LOW, HUMID_RANGE_HI);
        data.timestamp = timestamp;
        data.temp = iom361_readReg(iom361_initialize(16, 16, NULL), TEMP_REG, NULL);
        data.humid = iom361_readReg(iom361_initialize(16, 16, NULL), HUMID_REG, NULL);

        // Insert the data into the tree
        insert(tree, data);

        // Display debug info
        printf("INFO(main()): added timestamp %ld to BST\n", timestamp);
    }
}
