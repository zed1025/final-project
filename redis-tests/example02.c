/**
*** gcc -o redis_struct_example redis_struct_example.c -lhiredis
***
***
***
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>

// Define a simple structure representing a person
typedef struct {
    char name[50];
    int age;
} Person;

// Function to store a list of structures in Redis
void storeStructList(redisContext *context, const char *key, Person *people, size_t numPeople) {
    redisReply *reply;

    // Start a transaction
    redisAppendCommand(context, "MULTI");

    // Remove the existing list (if any)
    redisAppendCommand(context, "DEL %s", key);

    // Add each person to the list
    for (size_t i = 0; i < numPeople; ++i) {
        redisAppendCommand(context, "RPUSH %s %s:%d", key, people[i].name, people[i].age);
    }

    // Execute the transaction
    for (size_t i = 0; i < numPeople + 2; ++i) {
        redisGetReply(context, (void**)&reply);
        freeReplyObject(reply);
    }

    printf("List stored in Redis.\n");
}

// Function to print the stored list from Redis
void printStructList(redisContext *context, const char *key) {
    redisReply *reply;

    // Retrieve the list from Redis
    reply = redisCommand(context, "LRANGE %s 0 -1", key);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
        printf("Error retrieving list from Redis: %s\n", reply ? reply->str : "NULL");
        freeReplyObject(reply);
        return;
    }

    // Print the list contents
    printf("List contents for key '%s':\n", key);
    for (size_t i = 0; i < reply->elements; ++i) {
        printf("%zu: %s\n", i + 1, reply->element[i]->str);
    }

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

    // Create an array of Person structures
    Person people[] = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 22}
    };

    size_t numPeople = sizeof(people) / sizeof(people[0]);

    // Store the list in Redis
    storeStructList(context, "people_list", people, numPeople);

    // Disconnect from Redis
    redisFree(context);

    return 0;
}
