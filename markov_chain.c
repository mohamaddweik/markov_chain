#include <string.h>
#include "markov_chain.h"

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr){
    Node* current = markov_chain->database->first;
    while(current != NULL){
        if(strcmp(current->data->data, data_ptr) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr){
    //printf("data arrived is: %s\n", data_ptr);
    Node* node = get_node_from_database(markov_chain,data_ptr);
    if( node != NULL) // node is already in the linkedList
        return node;
    else{
        MarkovNode* markov_node = malloc(sizeof(MarkovNode)); //duplicate the word
        markov_node->data = malloc(strlen(data_ptr) + 1);
        if (markov_node->data == NULL) {
            return NULL;
        }
        strcpy(markov_node->data, data_ptr);
        markov_node->frequency_list = NULL;
        markov_node->frequency_list_size=0;
        int status = add(markov_chain->database,markov_node);

        if(status == 0){ //node added successfully
            return markov_chain->database->last;
        }
        else {
            return NULL;
        }
    }
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node){
    if(first_node->data[strlen(first_node->data)-1] == '.')
        return 0;

    if(first_node->frequency_list == NULL){
        first_node->frequency_list = malloc(sizeof(MarkovNodeFrequency)*2);
        if(first_node->frequency_list == NULL)
            return 1; //fail
        first_node->frequency_list[0].markov_node = second_node;
        first_node->frequency_list[0].frequency = 1;
        first_node->frequency_list[1].markov_node = NULL;
        first_node->frequency_list[1].frequency = 0;
        first_node->frequency_list_size=1;
        return 0; //success
    }
    // Check if the second node already exists in the frequency list
    for (int i = 0; i < first_node->frequency_list_size; i++) {
        if (strcmp(second_node->data, first_node->frequency_list[i].markov_node->data) == 0) { // word already in the list
            (first_node->frequency_list[i].frequency)++;
            return 0;
        }
    }

    MarkovNodeFrequency* newList = realloc(first_node->frequency_list, (first_node->frequency_list_size + 2) * sizeof(MarkovNodeFrequency));
    if (newList == NULL)
        return 1; // Fail

    first_node->frequency_list = newList;
    first_node->frequency_list[first_node->frequency_list_size].markov_node = second_node;
    first_node->frequency_list[first_node->frequency_list_size].frequency = 1;
    first_node->frequency_list[first_node->frequency_list_size+1].markov_node = NULL;
    first_node->frequency_list[first_node->frequency_list_size+1].frequency = 0;
    first_node->frequency_list_size++;
    return 0;
}

void free_database(MarkovChain ** ptr_chain){
    MarkovChain *markov_chain = *ptr_chain;
    Node* current = markov_chain->database->first;
    while (current != NULL) {
        Node *next = current->next;
        MarkovNode *markov_node = current->data;
        if (markov_node != NULL) {
            free(markov_node->data);
            free(markov_node->frequency_list);
            free(markov_node);
        }
        free(current);
        current = next;
    }
    free(markov_chain->database);
    free(markov_chain);
    *ptr_chain = NULL;
}

void print_database(MarkovChain* markovChain) {
    if (!markovChain || !markovChain->database || !markovChain->database->first) {
        printf("The database is empty or invalid.\n");
        return;
    }

    printf("\n===== Markov Chain Database =====\n");

    Node* node = markovChain->database->first;
    int size = markovChain->database->size;

    for (int i = 0; i < size; i++) {
        if (!node || !node->data) {
            break; // Handle unexpected null node
        }

        printf("Node: %s\n", node->data->data);
        printf("  Frequency List: ");

        if (!node->data->frequency_list) {
            printf("None\n");
        } else {
            int k = 0;
            while (node->data->frequency_list[k].markov_node != NULL) {
                printf("%s (%d)",
                       node->data->frequency_list[k].markov_node->data,
                       node->data->frequency_list[k].frequency);
                k++;
                if (node->data->frequency_list[k].markov_node != NULL) {
                    printf(", ");
                }
            }
            printf("\n");
        }

        node = node->next;
        if (node) {
            printf("  |\n  v\n");
        }
    }

    printf("===== End of Database =====\n\n");
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain){
    for (int j = 0; j < 15; ++j) {
        int rand = get_random_number(markov_chain->database->size);
        int k = 0;
        Node* current = markov_chain->database->first;
        while (k < rand && current != NULL) { //reach the random node
            current = current->next;
            k++;
        }
        if (current != NULL && current->data->frequency_list_size != 0) {
            return current->data;
        }
    }
    return NULL;
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node){
    int size = cur_markov_node->frequency_list_size;
    int frequencies=0;
    MarkovNodeFrequency *list = cur_markov_node->frequency_list;
    for (int i = 0; i < size; ++i) {
        frequencies+= list[i].frequency;
    }
    int rand = get_random_number(frequencies);

    int counter = 0;
    for (int i = 0; i < size; ++i) {
        counter += list[i].frequency;
        if(counter >= rand+1){
            return list[i].markov_node;
        }
    }
    printf("shouldn't be here");
    return NULL;
}

void generate_tweet(MarkovNode *first_node, int max_length){
    if (first_node == NULL) {
        fprintf(stderr, "Error: First node is NULL\n");
        return;
    }
    printf("%s ",first_node->data);
    MarkovNode *next = get_next_random_node(first_node);
    for (int i = 0; i < max_length-1; ++i) {
        printf("%s",next->data);
        if(next->frequency_list_size == 0 || i == max_length-1)
            break;

        if(i != max_length-2) {
            next = get_next_random_node(next);
            printf(" ");
        }
    }
    printf("\n");
}