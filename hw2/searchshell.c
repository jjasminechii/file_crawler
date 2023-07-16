/*
 * Copyright ©2023 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

#define MAX_INPUT 1024  // maximum input size

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc
static void Usage(void);

// Using the user query, we process our queries and
// find the document name corresponding to the doc id
// in order to print out our file path and ranking.
static void ProcessQueries(DocTable* dt, MemIndex* mi,
                           int current_num,
                           char** words_query);

// Reads the query from stdin in order to turn into lowercase
// and split query into words, then returns the length of our
// query.
static int GetNextLine(FILE* f, char** ret_str, char *str);


// for freeing linked list payloads
static void FreeSearchResult(LLPayload_t tofree) {
  SearchResult* sr = (SearchResult*) tofree;
  free(sr);
}


//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char** argv) {
  if (argc != 2) {
    Usage();
  }

  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  //
  // Note that you should make sure the fomatting of your
  // searchshell output exactly matches our solution binaries
  // to get full points on this part.

  printf("Indexing \'%s\'\n", argv[1]);

  DocTable* doctable;
  MemIndex* index;
  CrawlFileTree(argv[1], &doctable, &index);

  // get query
  printf("enter query:\n");
  char str[MAX_INPUT];
  // get queries in a loop until ctrl d
  char** words = (char**)malloc(sizeof(char*) * MAX_INPUT);
  while (fgets(str, MAX_INPUT, stdin) != NULL) {
    str[strcspn(str, "\n")] = 0;
    int current_word_num = GetNextLine(stdin, words, str);

    ProcessQueries(doctable, index, current_word_num, words);
    // one round of queries is done
    // get next query back at start of while loop
    printf("enter query:\n");
  }

  // free index and doctable
  free(words);
  MemIndex_Free(index);
  DocTable_Free(doctable);

  // finish
  printf("shutting down...\n");

  return EXIT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}

static void ProcessQueries(DocTable* dt, MemIndex* mi,
                          int current_num,
                          char** words_query) {
  LinkedList* list_of_docs;
  // now, we have an array of the query words
  // time to make a list of documents containing all words
  // use mem index search
  list_of_docs = MemIndex_Search(mi, words_query, current_num);
  if (list_of_docs == NULL) {
    return;
  }
  // now we have the sorted list
  // each element of the list has a doc id and a rank
  // find the document name corresponding to the doc id
  //    - make an iterator to step through the linked list
  //    - each element, need to get the doc id
  // print it out (find it using DocTable get doc name method)
  // print out the rank in parenthesis
  LLIterator* list_docs_itr = LLIterator_Allocate(list_of_docs);
  while (LLIterator_IsValid(list_docs_itr)) {
    SearchResult* sr;
    LLIterator_Get(list_docs_itr, (LLPayload_t)&sr);

    DocID_t docid = sr->doc_id;
    // we have the docid, now get the document
    char* doc_name;
    doc_name = DocTable_GetDocName(dt, docid);
    int rank = sr->rank;

    // we have name and rank, so print them
    printf("  %s (%d) \n", doc_name, rank);

    // move itr to next element
    LLIterator_Next(list_docs_itr);
  }
  LLIterator_Free(list_docs_itr);
  LinkedList_Free(list_of_docs, &FreeSearchResult);
}

static int GetNextLine(FILE *f, char **ret_str, char *str) {
  int lenstr = strlen(str);
  // turn the query to all lowercase
  for (int i = 0; i < lenstr; i++) {
    str[i] = tolower(str[i]);
  }
  char *ptr;
  int word_num = 0;
  char* word;
  word = strtok_r(str, " ", &ptr);
  // while we have not reached the end of query and we have space in words
  while (word != NULL && word_num < MAX_INPUT) {
    ret_str[word_num] = word;
    word = strtok_r(NULL, " ", &ptr);
    word_num++;
  }
  return word_num;
}
