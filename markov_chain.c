#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"

/**
 * Get random number between 0 and max_number-1.
 * @param max_number Maximum number (exclusive)
 * @return Random number in [0, max_number)
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr) {
    Node* current = markov_chain->database->first;
    while (current != NULL) {
        if (strcmp(current->data->data, data_ptr) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr) {
    Node* node = get_node_from_database(markov_chain, data_ptr);
    if (node != NULL)  // Word already exists in the database
        return node;
    else {
        MarkovNode* markov_node = malloc(sizeof(MarkovNode));
        if (!markov_node) return NULL;
        markov_node->data = malloc(strlen(data_ptr) + 1);
        if (!markov_node->data) {
            free(markov_node);
            return NULL;
        }
        strcpy(markov_node->data, data_ptr);
        markov_node->frequency_list = NULL;
        markov_node->frequency_list_size = 0;
        int status = add(markov_chain->database, markov_node);
        if (status == 0) { // Node added successfully
            return markov_chain->database->last;
        } else {
            free(markov_node->data);
            free(markov_node);
            return NULL;
        }
    }
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node) {
    if (!first_node || !second_node) return 1;

    // Do not add followers for sentence-ending words.
    int len = strlen(first_node->data);
    if (len > 0 && first_node->data[len - 1] == '.')
        return 0;

    // Check if second_node is already in the frequency list.
    for (int i = 0; i < first_node->frequency_list_size; i++) {
        if (strcmp(second_node->data, first_node->frequency_list[i].markov_node->data) == 0) {
            first_node->frequency_list[i].frequency++;
            return 0;
        }
    }

    // Expand the frequency list by one entry.
    MarkovNodeFrequency *newList = realloc(first_node->frequency_list,
                                           (first_node->frequency_list_size + 1) * sizeof(MarkovNodeFrequency));
    if (!newList)
        return 1; // Memory allocation failed

    first_node->frequency_list = newList;
    first_node->frequency_list[first_node->frequency_list_size].markov_node = second_node;
    first_node->frequency_list[first_node->frequency_list_size].frequency = 1;
    first_node->frequency_list_size++;

    return 0;
}

void free_database(MarkovChain **ptr_chain) {
    if (!ptr_chain || !(*ptr_chain))
        return;
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

/*
 * Revised get_first_random_node: scan the entire linked list and choose a node
 * whose word does not end with a period.
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain) {
    // First, count all nodes with a word that does not end with a period.
    int valid_count = 0;
    Node *current = markov_chain->database->first;
    while (current != NULL) {
        char *word = current->data->data;
        int len = strlen(word);
        if (len > 0 && word[len - 1] != '.')
            valid_count++;
        current = current->next;
    }
    if (valid_count == 0)
        return NULL;  // No valid starting node found.

    // Choose one uniformly at random.
    int target = get_random_number(valid_count);
    current = markov_chain->database->first;
    while (current != NULL) {
        char *word = current->data->data;
        int len = strlen(word);
        if (len > 0 && word[len - 1] != '.') {
            if (target == 0)
                return current->data;
            target--;
        }
        current = current->next;
    }
    return NULL; // Should not reach here.
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node) {
    int size = cur_markov_node->frequency_list_size;
    int total_frequency = 0;
    MarkovNodeFrequency *list = cur_markov_node->frequency_list;
    for (int i = 0; i < size; ++i) {
        total_frequency += list[i].frequency;
    }
    int rand_val = get_random_number(total_frequency);
    int counter = 0;
    for (int i = 0; i < size; ++i) {
        counter += list[i].frequency;
        if (counter > rand_val) {
            return list[i].markov_node;
        }
    }
    fprintf(stderr, "Error: get_next_random_node reached an unexpected state.\n");
    return NULL;
}

void generate_tweet(MarkovNode *first_node, int max_length) {
    if (first_node == NULL) {
        fprintf(stderr, "Error: First node is NULL\n");
        return;
    }

    int count = 0;
    MarkovNode *current = first_node;
    while (count < max_length) {
        printf("%s", current->data);
        count++;

        int len = strlen(current->data);
        // If the word ends with a period and we've printed at least 2 words, stop.
        if (len > 0 && current->data[len - 1] == '.' && count >= 2)
            break;
        if (count == max_length)
            break;
        if (current->frequency_list_size == 0)
            break;

        // Get the next word.
        current = get_next_random_node(current);
        printf(" ");
    }
    printf("\n");
}

