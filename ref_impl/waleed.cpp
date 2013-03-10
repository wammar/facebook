#include "../include/core.h"
#include <set>
#include <sstream>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

// Computes edit distance between a null-terminated string "a" with length "na"
//  and a null-terminated string "b" with length "nb" 
int EditDistance(char* a, int na, char* b, int nb)
{
	int oo=0x7FFFFFFF;

	static int T[2][MAX_WORD_LENGTH+1];

	int ia, ib;

	int cur=0;
	ia=0;

	for(ib=0;ib<=nb;ib++)
		T[cur][ib]=ib;

	cur=1-cur;

	for(ia=1;ia<=na;ia++)
	{
		for(ib=0;ib<=nb;ib++)
			T[cur][ib]=oo;

		int ib_st=0;
		int ib_en=nb;

		if(ib_st==0)
		{
			ib=0;
			T[cur][ib]=ia;
			ib_st++;
		}

		for(ib=ib_st;ib<=ib_en;ib++)
		{
			int ret=oo;

			int d1=T[1-cur][ib]+1;
			int d2=T[cur][ib-1]+1;
			int d3=T[1-cur][ib-1]; if(a[ia-1]!=b[ib-1]) d3++;

			if(d1<ret) ret=d1;
			if(d2<ret) ret=d2;
			if(d3<ret) ret=d3;

			T[cur][ib]=ret;
		}

		cur=1-cur;
	}

	int ret=T[1-cur][nb];

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Computes Hamming distance between a null-terminated string "a" with length "na"
//  and a null-terminated string "b" with length "nb" 
unsigned int HammingDistance(char* a, int na, char* b, int nb)
{
	int j, oo=0x7FFFFFFF;
	if(na!=nb) return oo;
	
	unsigned int num_mismatches=0;
	for(j=0;j<na;j++) if(a[j]!=b[j]) num_mismatches++;
	
	return num_mismatches;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all information related to an active query
struct Query
{
	QueryID query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int numberWords;
	unsigned int match_dist;
	vector<string> keywords;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all query ID results associated with a dcoument
struct Document
{
	DocID doc_id;
	unsigned int num_res;
	QueryID* query_ids;
	map<QueryID,set<string> > matchedQ;
	vector<QueryID> finalResult;
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct Keyword
{
// I must replace QueryID by the iterator from query map because no queries added or removed from the map while checking documents
// All documents are added after queries but keywords are always there and queries might be added or deleted so the iterator does not work here
// What we need is a mechanism to delete the query without too much processing for the keywords 

	string myWord;
	// Exact Match queries
	set<QueryID> exactQ;
	set<QueryID> hammingQ[3];
	set<QueryID> editQ[3];
};



///////////////////////////////////////////////////////////////////////////////////////////////
// Keeps all currently active queries
map<QueryID, Query> queries;
map<string, Keyword> keywords;

// Keeps all currently available results that has not been returned yet
vector<Document> docs;

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode InitializeIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode DestroyIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

void AddKeyWords(QueryID query_id, MatchType match_type, unsigned int match_dist, vector<string> queryWords)
{
// it returns keywords location 
	map<string, Keyword>::iterator it;
	
//	bool debug = false;
//	if(query_id == -1)
//		debug = true;


	for(unsigned int i=0; i<queryWords.size();i++)
	{

//	if(!queryWords[i].compare("airport"))
//		debug = true;

//	if(debug)
//	{
//		cout << "Exact Type :" << MT_EXACT_MATCH << endl;
//		cout << "Hamming Type :" << MT_HAMMING_DIST <<endl;
//		cout << "Edit Type:" << MT_EDIT_DIST << endl;
//		cout << "keyword : " << queryWords[i] << " inserted with type :  " << match_type << "(" << match_dist << ")"<< endl;
//	}
	  Keyword newWord;
     	  it = keywords.find(queryWords[i]);
	  if(it == keywords.end())  
	{
//		if(debug)
//		cout << "New keyword" << endl;

	    // insert a new keyword to the map
	    newWord.myWord = queryWords[i];
	    if(match_type == MT_EXACT_MATCH)
	//	newWord.exactQ.push_back(query_id);
		newWord.exactQ.insert(query_id);

            else if(match_type == MT_HAMMING_DIST)
		{
// /               if(debug)
//                        cout << "Adding keyword '" << newWord.myWord << "' to Hamming lists size " << newWord.hammingQ[match_dist-1].size();

//		for(unsigned int h=0;h<match_dist;h++)
//		    newWord.hammingQ[match_dist-1].push_back(query_id);
		    newWord.hammingQ[match_dist-1].insert(query_id);

//		if(debug)
//			cout <<" --> " << newWord.hammingQ[match_dist-1].size() << endl;

            	}
	    else if(match_type == MT_EDIT_DIST)
		{
//		if(debug)
//			cout << "Adding keyword '" << newWord.myWord << "' to Edit lists size " << newWord.editQ[match_dist-1].size() ;
//		for(unsigned int h=0;h<match_dist;h++)
//			newWord.editQ[match_dist-1].push_back(query_id);
			newWord.editQ[match_dist-1].insert(query_id);			

//		if(debug)
//			cout << " --> " << newWord.editQ[match_dist-1].size() << endl;
	    	}
            keywords.insert(std::pair<string,Keyword>(newWord.myWord,newWord));
	  }// end if not found
	else
	  {

//		if(debug)
//			cout << "already available keyword" << endl;

	    if(match_type == MT_EXACT_MATCH)
//                it->second.exactQ.push_back(query_id);
		it->second.exactQ.insert(query_id);
            else if(match_type == MT_HAMMING_DIST)
		{
//                    if(debug)
//                        cout << "Adding keyword '" << it->second.myWord << "' to Hamming lists size " << it->second.hammingQ[match_dist-1].size();

           //     for(unsigned int h=0;h<match_dist;h++)
	//		it->second.hammingQ[match_dist-1].push_back(query_id);
			it->second.hammingQ[match_dist-1].insert(query_id);		
//		   if(debug)
//			cout << " --> " << it->second.hammingQ[match_dist-1].size() << endl;
                }
	   else if(match_type == MT_EDIT_DIST)
		{
//                if(debug)
//                        cout << "Adding keyword '" << it->second.myWord << "' to Edit lists " << it->second.editQ[match_dist-1].size();

	//	for(unsigned int h=0;h<match_dist;h++)
        //        	it->second.editQ[match_dist-1].push_back(query_id);
			it->second.editQ[match_dist-1].insert(query_id);	


//		if(debug)	
//			cout << " --> " << it->second.editQ[match_dist-1].size() << endl;
		}
          }

// cout<<"Keyword sanity check" << endl;
// cout<<keywords[queryWords[i]].exactQ.size() << " " << keywords[queryWords[i]].hammingQ.size() << " " << keywords[queryWords[i]].editQ.size() << " " << endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
vector<string> split(string text);

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{

//cout << "Adding query : " << query_id << ", # keywords = " << keywords.size() << endl;

	Query query;
	query.query_id=query_id;
	strcpy(query.str, query_str);
	query.match_type=match_type;
	query.match_dist=match_dist;

	query.keywords = split(string(query.str));
	query.numberWords = query.keywords.size();
	AddKeyWords(query_id, match_type, match_dist, query.keywords);

	// Add this query to the active query set
	queries.insert(std::pair<QueryID,Query>(query.query_id,query));
//	queries.push_back(query);
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode EndQuery(QueryID query_id)
{
	// Remove this query from the active query set


map<QueryID, Query>::iterator qIT = queries.find(query_id);

Query q = qIT->second;
unsigned int index;

if(q.match_type == MT_EXACT_MATCH )
for(unsigned int i=0;i<q.keywords.size(); i++)
{

//for(unsigned int j=0; j < keywords[q.keywords[i]].exactQ.size() ; j++)
//	if(keywords[q.keywords[i]].exactQ[j] == query_id)
//		index = j;

//keywords[q.keywords[i]].exactQ.erase (keywords[q.keywords[i]].exactQ.begin()  + index);
keywords[q.keywords[i]].exactQ.erase(query_id);

}

else if (q.match_type == MT_HAMMING_DIST )
for(unsigned int i=0;i<q.keywords.size(); i++)
{

//for(unsigned int h=0; h < (q.match_dist-1); h++ )
//{
//    for(unsigned int j=0; j < keywords[q.keywords[i]].hammingQ[q.match_dist-1].size() ; j++)
//        if(keywords[q.keywords[i]].hammingQ[q.match_dist-1][j] == query_id)
//                index = j;

//    keywords[q.keywords[i]].hammingQ[q.match_dist-1].erase (keywords[q.keywords[i]].hammingQ[q.match_dist-1].begin()  + index);
      keywords[q.keywords[i]].hammingQ[q.match_dist-1].erase (query_id);	
//}

}
else 
for(unsigned int i=0;i<q.keywords.size(); i++)
{

//for(unsigned int h=0; h < (q.match_dist-1); h++ )
//{
//    for(unsigned int j=0; j < keywords[q.keywords[i]].editQ[q.match_dist-1].size() ; j++)
//        if(keywords[q.keywords[i]].editQ[q.match_dist-1][j] == query_id)
//                index = j;

//    keywords[q.keywords[i]].editQ[q.match_dist-1].erase (keywords[q.keywords[i]].editQ[q.match_dist-1].begin()  + index);
      keywords[q.keywords[i]].editQ[q.match_dist-1].erase (query_id);
//}

}




//cout << "should erase " << query_id << " now. #queries = " << queries.size() << endl;
queries.erase(qIT);
//cout << "query erased, #queries = "<< queries.size() <<endl;

// cout<<" something still missing. I have to remove the query from keywords !"


	return EC_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////////////////////
vector<string> split(string text)
{

vector<string> keywords;

 stringstream ss (stringstream::in | stringstream::out);
 ss << text;
 string s;

 while (ss >> s) 
  keywords.push_back(s);
 
return keywords;

}
///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode MatchDocument(DocID doc_id, const char* doc_str)
{
//	char cur_doc_str[MAX_DOC_LENGTH];
//	strcpy(cur_doc_str, doc_str);

//	bool debug = false;
//	if(doc_id == -1)
//		debug = true;
	
//	if(debug)
//		cout<< doc_id << " : " << doc_str << endl;
	
	Document mydoc;
	mydoc.doc_id = doc_id;
	

//	string documentString;
//	strcpy(documentString, doc_str);

	set<string> localKeywords;
	set<string>::iterator lIT;	
	vector<QueryID> matched;
	map<string, Keyword>::iterator it;

	vector<string> documentWords = split(string(doc_str)) ;

//	if(debug)
//		cout << "# document words: " << documentWords.size() << ", # query words: " << keywords.size() <<endl;	


	set<QueryID>::iterator wordIT;

	for(unsigned int i=0; i<documentWords.size(); i++)
	{
	lIT = localKeywords.find(documentWords[i]);
	if(lIT == localKeywords.end())
{
		
	localKeywords.insert(documentWords[i]);

	for(it = keywords.begin(); it != keywords.end(); it++)
	{
	 if( documentWords[i].compare(it->second.myWord) == 0 && it->second.exactQ.size() > 0)
	 {
		// the keywords are equal and there are some queries waiting
		for(wordIT = it->second.exactQ.begin(); wordIT != it->second.exactQ.end() ; wordIT++)
//		for(unsigned int q=0;q < it->second.exactQ.size() ; q++)
                       // matched.push_back(it->second.exactQ[q]);
			mydoc.matchedQ[*wordIT].insert(it->second.myWord);

	 }

//	if(debug && !it->first.compare("airlines"))
//		cout << "# hamming query for " <<  it->first  <<"  = " << it->second.hammingQ[0].size() + it->second.hammingQ[1].size() + it->second.hammingQ[2].size()  << endl;


// Check if there is any hamming query for this keyword
	 if(it->second.hammingQ[0].size() + it->second.hammingQ[1].size() + it->second.hammingQ[2].size()  > 0) 
	 {
		
		// calculate the hamming betweek keyword and document word	
		int hamming = HammingDistance(const_cast<char *> (it->second.myWord.data()), it->second.myWord.size(),const_cast<char *> (documentWords[i].data()), documentWords[i].size());
		
//		if( debug && !it->second.myWord.compare("airlines")  && !documentWords[i].compare("airlimnes"))
//			cout<< "query word 'airlines' \t hamming distance = " << hamming <<endl ;

// if the hamming 3 or less than three, there is a possibility to find a query
		if(hamming <= 3)
		{
		
//		if(debug && hamming!=0)
//                cout << documentWords[i]<<"\thamming distance = " << hamming << "\t #queries = " << it->second.hammingQ[hamming-1].size() << "\t keyword = " << it->second.myWord <<endl;

		for(unsigned int h=max( hamming -1,0); h < 3 ; h++)
		  for(wordIT = it->second.hammingQ[h].begin() ; wordIT != it->second.hammingQ[h].end() ; wordIT++)
//			matched.push_back(it->second.hammingQ[hamming][q]);
			mydoc.matchedQ[*wordIT].insert(it->second.myWord);
		}	 
	}



//        if(debug && !it->first.compare("airlines"))
//                cout << "# edit query for " <<  it->first  <<" = "<< it->second.editQ[0].size() + it->second.editQ[1].size() + it->second.editQ[2].size()  << endl;


        if((it->second.editQ[0].size() + it->second.editQ[1].size() + it->second.editQ[2].size()) > 0)
         {
                int edit = EditDistance(const_cast<char *>(it->second.myWord.data()), it->second.myWord.size(), const_cast<char *>(documentWords[i].data()), documentWords[i].size());
         	
		
//                if( debug && !it->second.myWord.compare("airlines") ) 
//                        cout<< "query word 'airlines' \t edit distance = " << edit <<endl ;


		if(edit <= 3)
                {

//               if(debug && edit !=0)
//                        cout << documentWords[i]<<"\tedit distance = " << edit << "\t #queries = " << it->second.editQ[edit].size() << "\t keyword = " << it->second.myWord <<endl;

		for(unsigned int e=max(edit-1,0); e<3; e++)		       
                  for(wordIT = it->second.editQ[e].begin() ; wordIT != it->second.editQ[e].end(); wordIT++)
//                        matched.push_back(it->second.editQ[edit][q]);
			mydoc.matchedQ[*wordIT].insert(it->second.myWord);
                }

	 }
	
	}// end loop on all keywords

//	if(debug)
//		cout << "For word : "<< documentWords[i] << ",\t " << mydoc.matchedQ.size() << " queries are matching" <<endl; 
	}// end if the word was not seeb before
	
	}// end loop on all words in the document

	// refine the result now
	map<QueryID, set<string> >::iterator mIT;

//	if(debug)
//		cout<< "# matched queries : " << mydoc.matchedQ.size() << endl;
 
	for(mIT = mydoc.matchedQ.begin() ; mIT != mydoc.matchedQ.end(); mIT++)
	{
//		if(debug)
//			{
//			cout << "Query "<< mIT->first << " has "<< queries[mIT->first].keywords.size() << " words, only "<< mIT->second.size() << " are matching";
//			cout << "\t query words: "<< queries[mIT->first].str ;
//			cout << "\t query type: "<< queries[mIT->first].match_type << "(" << queries[mIT->first].match_dist << ")" << endl;	
			//cout << "\t matching words: "<< mIT->second <<endl;
//			}
		if(mIT->second.size() == queries[mIT->first].numberWords)
			mydoc.finalResult.push_back(mIT->first);
	}
	
	mydoc.matchedQ.clear();


        mydoc.num_res=mydoc.finalResult.size();
        mydoc.query_ids=0;
        if(mydoc.num_res) 
		mydoc.query_ids=(unsigned int*)malloc(mydoc.num_res*sizeof(unsigned int));
        

	for(int i=0;i<mydoc.num_res;i++) 
		mydoc.query_ids[i]=mydoc.finalResult[i];
        
	mydoc.finalResult.clear();

	// Add this result to the set of undelivered results
        docs.push_back(mydoc);


	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids)
{

/*	
	
	// Get the first undeliverd resuilt from "docs" and return it
	*p_doc_id=0; *p_num_res=0; *p_query_ids=0;
	
	if(docs.size()==0) return EC_NO_AVAIL_RES;
	
	Document doc = docs[0]; // docs.back();
       // docs.pop_back();

	*p_doc_id=doc.doc_id; 
	*p_num_res=doc.finalResult.size(); 
	
	// docs[0].num_res; 
	*p_query_ids=  &(doc.finalResult[0]);  //docs[0].query_ids;
//	docs.erase(docs.begin());

	cout<< doc.doc_id << " --> " << doc.finalResult.size() <<endl;
*/


        // Get the first undeliverd resuilt from "docs" and return it
        *p_doc_id=0; *p_num_res=0; *p_query_ids=0;
        if(docs.size()==0) return EC_NO_AVAIL_RES;
        *p_doc_id=docs[0].doc_id; *p_num_res=docs[0].num_res; *p_query_ids=docs[0].query_ids;
        docs.erase(docs.begin());


	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
