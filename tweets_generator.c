#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path\n"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments\n"
#define DELIMITERS " \n\t\r"

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    // Allocate the linked list for the database.
    markov_chain->database = malloc(sizeof(LinkedList));
    if (markov_chain->database == NULL)
        return 1;
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;

    char *word1 = NULL, *word2 = NULL;
    Node *node1 = NULL, *node2 = NULL;
    int word_count = 0;
    char buffer[1000];

    while ((words_to_read == -1 || word_count < words_to_read) &&
           fgets(buffer, sizeof(buffer), fp) != NULL) {
        word1 = strtok(buffer, DELIMITERS);
        if (!word1)
            continue;
        node1 = add_to_database(markov_chain, word1);
        if (node1 == NULL) {
            free_database(&markov_chain);
            return 1;
        }
        word_count++;

        while ((words_to_read == -1 || word_count < words_to_read) &&
               (word2 = strtok(NULL, DELIMITERS))) {
            node2 = add_to_database(markov_chain, word2);
            if (node2 == NULL) {
                free_database(&markov_chain);
                return 1;
            }
            word_count++;

            int status = add_node_to_frequency_list(node1->data, node2->data);
            if (status != 0) {
                free_database(&markov_chain);
                return 1;
            }
            // Move to the next pair.
            word1 = word2;
            node1 = node2;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4 && argc != 5) {
        fprintf(stderr, NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    unsigned int seed = atoi(argv[1]);
    int tweets_num = atoi(argv[2]);
    char *file_path = argv[3];
    int words_to_read = (argc == 5) ? atoi(argv[4]) : -1;

    srand(seed);
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }

    MarkovChain *chain = malloc(sizeof(MarkovChain));
    if (chain == NULL) {
        fclose(file);
        return EXIT_FAILURE;
    }

    if (fill_database(file, words_to_read, chain) == 1) {
        free_database(&chain);
        fclose(file);
        return EXIT_FAILURE;
    }
    fclose(file);

    // Generate the tweets.
    for (int i = 1; i <= tweets_num; ++i) {
        MarkovNode *first = get_first_random_node(chain);
        printf("Tweet %d: ", i);
        generate_tweet(first, 20);
    }

    free_database(&chain);
    return EXIT_SUCCESS;
}

