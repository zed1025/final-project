#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>

// Define a simple structure representing a person
typedef struct {
    char name[50];
    int age;
} Person;

// Function to fetch and print an array of structures from Redis
void fetchAndPrintStructArray(redisContext *context, const char *key) {
    // Use GET to retrieve the JSON string from Redis
    redisReply *reply = redisCommand(context, "GET %s", key);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
        printf("Error retrieving data from Redis: %s\n", reply ? reply->str : "NULL");
        freeReplyObject(reply);
        return;
    }

    // Parse the JSON-like string and print the array of structures
    printf("Array of structures retrieved from Redis:\n");
    char *jsonString = reply->str;
    size_t jsonStringLen = strlen(jsonString);

    // Verify that the string starts with '[' and ends with ']'
    if (jsonStringLen >= 2 && jsonString[0] == '[' && jsonString[jsonStringLen - 1] == ']') {
        jsonString++;  // Skip the '['
        jsonString[jsonStringLen - 2] = '\0';  // Remove the ']'
    }

    // Tokenize the string based on ','
    char *token = strtok(jsonString, ",");
    while (token != NULL) {
        // Extract name and age from the token
        char name[50];
        int age;
        if (sscanf(token, "{\"name\":\"%49[^\"]\",\"age\":%d}", name, &age) == 2) {
            printf("Name: %s, Age: %d\n", name, age);
        }

        // Move to the next token
        token = strtok(NULL, ",");
    }

    // Free Redis reply
    freeReplyObject(reply);
}

int main() {
    // Connect to Redis
    redisContext *context = redisConnect("127.0.0.1", 6379);
    if (context == NULL || context->err) {
        if (context) {
            printf("Connection error: %s\n", context->errstr);
            redisFree(context);
        } else {
            printf("Connection error: Can't allocate redis context\n");
        }
        exit(EXIT_FAILURE);
    }

    // Fetch and print the array of structures from Redis
    fetchAndPrintStructArray(context, "people_array");

    // Disconnect from Redis
    redisFree(context);

    return 0;
}
