#include <string.h>
#include "markov_chain.h"

#define DELIMITERS " \n\t\r"

int main() {
    char text[] = "cloudy rainy cloudy rainy sunny cloudy sunny.\n";

    // Allocate memory for MarkovChain
    MarkovChain *chain = (MarkovChain *)malloc(sizeof(MarkovChain));
    if (chain == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for MarkovChain.\n");
        return EXIT_FAILURE;
    }


    // Allocate memory for LinkedList
    chain->database = (LinkedList *)malloc(sizeof(LinkedList));
    if (chain->database == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for LinkedList.\n");
        free(chain); // Free previously allocated memory
        return EXIT_FAILURE;
    }

    chain->database->first=NULL;
    chain->database->size=0;

    char *word1 = NULL, *word2 = NULL;
    Node *node1 = NULL, *node2 = NULL;

    // Use strtok to split the text into words
    word1 = strtok(text, DELIMITERS);
    node1 = add_to_database(chain, word1);

    if (node1 == NULL) {
        fprintf(stderr, "Error: Failed to add the first word to the database.\n");
        free_database(&chain); // Free all allocated memory
        return EXIT_FAILURE;
    }

    // Iterate over the words in the text and add them to the database
    while ((word2 = strtok(NULL, DELIMITERS)) != NULL) {
        node2 = add_to_database(chain, word2);
        if (node2 == NULL) {
            fprintf(stderr, "Error: Failed to add word %s to the database.\n", word2);
            free_database(&chain); // Free all allocated memory
            return EXIT_FAILURE;
        }

        int stat = add_node_to_frequency_list(node1->data, node2->data);
        if (stat != 0) {
            fprintf(stderr, "Error: Failed to add word '%s' to the frequency list.\n", word2);
            free_database(&chain); // Free all allocated memory
            return EXIT_FAILURE;
        }
        word1 = word2;
        node1 = node2;
    }
    print_database(chain);
    for (int i = 0; i < 5; ++i) {
        MarkovNode *markov = get_first_random_node(chain);
        printf("%s\n",markov->data);
    }
    free_database(&chain);
    return 0;
}

