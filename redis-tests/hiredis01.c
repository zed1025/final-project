#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<hiredis/hiredis.h>

typedef struct {
	int id;
	char data[50];
} Checkpoint;


// // Function to store a list of structures in Redis
// void storeStructList(redisContext *context, const char *key, Checkpoint *people, size_t numPeople) {
//     redisReply *reply;

//     // Start a transaction
//     reply = redisCommand(context, "MULTI");
//     freeReplyObject(reply);

//     // Remove the existing list (if any)
//     reply = redisCommand(context, "DEL %s", key);
//     freeReplyObject(reply);

//     // Add each person to the list
//     for (size_t i = 0; i < numPeople; ++i) {
//         reply = redisCommand(context, "RPUSH %s %d:%s", key, people[i].id, people[i].data);
//         if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
//             printf("Error storing data in Redis: %s\n", reply ? reply->str : "NULL");
//             freeReplyObject(reply);
//             return;
//         }
//         freeReplyObject(reply);
//     }

//     printf("List stored in Redis.\n");
// }

// Function to store a list of structures in Redis
void storeStructList(redisContext *context, const char *key, Checkpoint *people, size_t numPeople) {
    // Create a buffer to hold the serialized data
    char buffer[512];  // Adjust the buffer size as needed

    // Serialize each person's data and concatenate it into the buffer
    size_t bufferIndex = 0;
    for (size_t i = 0; i < numPeople; ++i) {
        int written = snprintf(buffer + bufferIndex, sizeof(buffer) - bufferIndex, "%d:%s", people[i].id, people[i].data);
        if (written < 0 || written >= (sizeof(buffer) - bufferIndex)) {
            printf("Error: Buffer overflow while serializing data.\n");
            return;
        }
        bufferIndex += written;
        if (i < numPeople - 1) {
            // Add a separator between person entries (adjust as needed)
            written = snprintf(buffer + bufferIndex, sizeof(buffer) - bufferIndex, ",");
            if (written < 0 || written >= (sizeof(buffer) - bufferIndex)) {
                printf("Error: Buffer overflow while serializing data.\n");
                return;
            }
            bufferIndex += written;
        }
    }

    // Use LPUSH to push the serialized data onto the list in Redis
    redisReply *reply = redisCommand(context, "LPUSH %s %s", key, buffer);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
        printf("Error storing data in Redis: %s\n", reply ? reply->str : "NULL");
        freeReplyObject(reply);
        return;
    }

    printf("List stored in Redis.\n");
    freeReplyObject(reply);
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
    printf("Size of the list retrieved from server is: %ld\n", reply->elements);
    printf("List contents for key '%s':\n", key);
    for (size_t i = 0; i < reply->elements; ++i) {
        // Split the string into name and age
        char *token = strtok(reply->element[i]->str, ":");
        if (token != NULL) {
            printf("%zu: Name: %s, Age: %s\n", i + 1, token, strtok(NULL, ":"));
        }
    }

    freeReplyObject(reply);
}


int main(int argc, char **argv) {

	const char *hostname = (argc > 1) ? argv[1] : "127.0.0.1";
	int port = (argc > 2) ? (long)argv[2] : 6379;

	unsigned isUnix = 0;

	redisContext *c = redisConnect(hostname, port);

	struct timeval timeout = {1, 50000};
	if(isUnix) {
		c = redisConnectUnixWithTimeout(hostname, timeout);
	} else {
		c = redisConnectWithTimeout(hostname, port, timeout);
	}

	if(c == NULL || c->err) {
		if(c) {
			printf("Connection ERROR: %s\n", c->errstr);
			redisFree(c);
		} else {
			printf("Connection ERROR: Cannot allocate redis context\n");
		}
		exit(1);
	}

	printf("Connected to redis\n");


	redisReply *reply;


	// // PING Server
	// reply = redisCommand(c, "PING");
	// printf("%s\n", reply->str);
	// freeReplyObject(reply);

	// // Set a key
	// reply = redisCommand(c, "SET %s %s", "name", "Amit Kumar");
	// printf("%s\n", reply->str);
	// freeReplyObject(reply);

	// // Get a key
	// reply = redisCommand(c, "GET %s", "name");
	// printf("%s\n", reply->str);
	// freeReplyObject(reply);

	// /* Create a list of numbers, from 0 to 9 */
	// unsigned int j;
    // reply = redisCommand(c,"DEL mylist");
    // freeReplyObject(reply);
    // for (j = 0; j < 10; j++) {
    //     char buf[64];

    //     snprintf(buf,64,"%u",j);
    //     reply = redisCommand(c,"LPUSH mylist element-%s", buf);
    //     freeReplyObject(reply);
    // }

    // /* Let's check what we have inside the list */
    // reply = redisCommand(c,"LRANGE mylist 0 -1");
    // if (reply->type == REDIS_REPLY_ARRAY) {
    //     for (j = 0; j < reply->elements; j++) {
    //         printf("%u) %s\n", j, reply->element[j]->str);
    //     }
    // }
    // freeReplyObject(reply);



    /** ************************************************************************************************ */

    // definining the data to store on the redis server
    Checkpoint checkpoints[] = {
    	{1, "Data01"},
    	{2, "Data02"},
    	{3, "Data03"}
    };
    size_t numCheckpoints = sizeof(checkpoints) / sizeof(checkpoints[0]);
    char *key = "my_struct_lists"; // name of the array



    storeStructList(c, key, checkpoints, numCheckpoints);


    /* Create a list of map and store it on redis server */
	// unsigned int j;
    // start a transaction
    // redisAppendCommand(c, "MULTI");
    // remove existing list if any
    // reply = redisCommand(c,"DEL %s", key);
    // Add checkpint struct to the list
    // for (j = 0; j < numCheckpoints; j++) {
    //     redisAppendCommand(c, "RPUSH %s %d:%s", key, checkpoints[j].id, checkpoints[j].data);
    // }
    // Execute the transaction
    // printf("Executing the transaction\n");
    // for (size_t i = 0; i < numCheckpoints; ++i) {
    //     reply = redisCommand(c, "LPUSH %s %d:%s", key, checkpoints[j].id, checkpoints[j].data);
    //     printf("%s\n", reply->str);
    //     freeReplyObject(reply);
    // }


    // print the stored data
    printStructList(c, key);


    /** ************************************************************************************************ */


    /* Disconnects and frees the context */
    redisFree(c);
}