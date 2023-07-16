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

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;

namespace hw3 {

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t doc_id;  // The document ID within the index file.
  int     rank;    // The rank of the result so far.
} IdxQueryResult;

<<<<<<< HEAD
// Looks up words in the index and combines the results
// for all words in the query.
// Takes a query and an index reader and returns a
// vector of documents that match the query.
static vector<IdxQueryResult> QueryIndex(IndexTableReader *itr,
                                         const vector<string>& query);

// If the doc_id is in the new_list return true, otherwise return false.
// The rank of the document within the list is stored within the rank
// argument.
static bool FindDocID(DocID_t doc_id, const list<DocIDElementHeader>& new_list,
                      int* rank);
=======
static vector<IdxQueryResult> QueryIndex(IndexTableReader *itr, const vector<string>& query);
static bool FindDocID(DocID_t doc_id, list<DocIDElementHeader>& new_list, int* rank);
>>>>>>> meganaHW4


QueryProcessor::QueryProcessor(const list<string>& index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader* [array_len_];
  itr_array_ = new IndexTableReader* [array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

<<<<<<< HEAD
=======



>>>>>>> meganaHW4
vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string>& query) const {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  // This method processes a query against the indices and returns a
  // vector of QueryResults, sorted in descending order of rank.  If no
  // documents match the query, then a valid but empty vector will be
  // returned.
  vector<QueryProcessor::QueryResult> final_result;

<<<<<<< HEAD
  for (int i = 0; i < array_len_; i++) {
    DocTableReader *dtr = dtr_array_[i];
    IndexTableReader *itr = itr_array_[i];

    // get a list of the results but with the docid instead of filename
    vector<IdxQueryResult> semi_results = QueryIndex(itr, query);

    // go through and make this into a list of query results
    list<QueryResult> add_results;
    for (const auto& iter : semi_results) {
      QueryResult qr;
      string filename;
      dtr->LookupDocID(iter.doc_id, &filename);
      qr.document_name = filename;
      qr.rank = iter.rank;
=======
  
  for (int i = 0; i < array_len_; i++) { 
  
    DocTableReader *dtr = dtr_array_[i];
    IndexTableReader *itr = itr_array_[i];
    
    // get a list of the results but with the docid instead of filename
    vector<IdxQueryResult> semi_results = QueryIndex(itr, query);

    // go through and make this into a list of  query results
    list<QueryResult> add_results;
    for (auto iter = semi_results.begin(); iter != semi_results.end(); iter++) {
      QueryResult qr;
      string filename;

      dtr->LookupDocID(iter->doc_id, &filename);

      qr.document_name = filename;
      qr.rank = iter->rank;
>>>>>>> meganaHW4

      // add the new qr to the final result
      final_result.push_back(qr);
    }
  }

  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

<<<<<<< HEAD
static vector<IdxQueryResult> QueryIndex(IndexTableReader *itr,
                                         const vector<string>& query) {
  // there is a doc_id field and a num_positions
  // field which is rank
  vector<IdxQueryResult> result;
  if (query.empty()) {
    return result;
  }
  // Lookup a word and get back a DocIDTableReader
  // containing the docID-->positions mapping
  DocIDTableReader *wordDtr = itr->LookupWord(query[0]);
  if (wordDtr == nullptr) {
    return result;
  }

  // get a list of doc ids with the ranks that contain word.
  list<DocIDElementHeader> headers;
  headers = wordDtr->GetDocIDList();
  delete wordDtr;

  for (const auto& header : headers) {
    result.push_back({header.doc_id, header.num_positions});
  }

  // OK, there are additional query words.  Handle them one at a time.
  for (int i = 1; i < (int)query.size(); i++) {
    DocIDTableReader *wordDtr = itr->LookupWord(query[i]);

    // If there are no matches, it means the overall query
    // should return no documents, so free result.
    if (wordDtr == nullptr) {
      result.clear();
=======
  
static vector<IdxQueryResult> QueryIndex(IndexTableReader *itr, const vector<string>& query) {
  vector<IdxQueryResult> result; // there is a doc_id field and a num_positions field which is rank

  // Lookup a word and get back a DocIDTableReader containing the docID-->positions mapping
    DocIDTableReader *wordDtr = itr->LookupWord(query[0]);
    if (wordDtr == nullptr) {
>>>>>>> meganaHW4
      return result;
    }

    // get a list of doc ids with the ranks that contain word.
<<<<<<< HEAD
    headers = wordDtr->GetDocIDList();
    delete wordDtr;

    int res_size = result.size();
    for (int i = 0; i < res_size; i++) {
      DocID_t doc_id = result.at(i).doc_id;
      int rank;
      if (FindDocID(doc_id, headers, &rank)) {
        result.at(i).rank += rank;
      } else {
        result.erase(result.begin() + i);
        i--;
        res_size--;
      }
    }
  }
  return result;
}

static bool FindDocID(DocID_t doc_id, const list<DocIDElementHeader>& new_list,
                      int* rank) {
  for (const auto& it : new_list) {
    if (it.doc_id == doc_id) {
      *rank = it.num_positions;
      return true;
    }
  }
  return false;
}
=======
    list<DocIDElementHeader> headers;
    headers = wordDtr->GetDocIDList();

    delete wordDtr;

    for (auto header : headers) {
      IdxQueryResult iqr;
      iqr.doc_id = header.doc_id;
      iqr.rank = header.num_positions;

      result.push_back(iqr);
    }
    
    // after one query, check if that is all there is.
    if (query.size() == 1) {
      return result;
    }

    // OK, there are additional query words.  Handle them one at a time.
    for (int i = 1; i < (int)query.size(); i++) {
      DocIDTableReader *wordDtr = itr->LookupWord(query[i]);

      // If there are no matches, it means the overall query
      // should return no documents, so free result.
      if (wordDtr == nullptr) {
        result.clear();
        return result;
      }

      // get a list of doc ids with the ranks that contain word.
      list<DocIDElementHeader> headers;
      headers = wordDtr->GetDocIDList();

      delete wordDtr;

      int res_size = result.size();
      for (int i = 0; i < res_size; i++) {
        DocID_t doc_id = result.at(i).doc_id;
        int rank;
        if (FindDocID(doc_id, headers, &rank)) {
          result.at(i).rank += rank;
        } else {
          result.erase(result.begin() + i);
          i--;
          res_size--;
        }
      }
    }

    return result;
  }

  static bool FindDocID(DocID_t doc_id, list<DocIDElementHeader>& new_list, int* rank) {
    for (auto it = new_list.begin(); it != new_list.end(); it++) {
      if (it->doc_id == doc_id) {
        *rank = it->num_positions;
        return true;
      }
    }
    return false;
  }
>>>>>>> meganaHW4

}  // namespace hw3



