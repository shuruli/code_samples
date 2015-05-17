

#ifndef TRIE_H
#define TRIE_H

#ifndef nullptr
#define nullptr 0
#endif

#include "ece250.h"
#include "Trie_node.h"
#include "Exception.h"
#include <iostream>
#include <string>
#include <cctype>

class Trie {
private:
	Trie_node *root_node;
	int trie_size;

public:
	Trie();
	~Trie();

	// Accessors

	int size() const;
	bool empty() const;
	bool member(std::string const &) const;
	Trie_node *root() const;
	// Mutators

	bool insert(std::string const &);
	bool erase(std::string const &);
	void clear();

	// Friends

	friend std::ostream &operator<<(std::ostream &, Trie const &);
};

Trie::Trie() :
root_node(nullptr),
trie_size(0) {
	// empty constructor
}

Trie::~Trie() {
	if (!empty()){
		clear();
	}
}

// simple accessor for a field in this class (size) 
int Trie::size() const {
	return trie_size;
}

// method to determine whether the Trie structure is empty
bool Trie::empty() const {
	if (trie_size == 0){
		return true; 
	}
	return false; 
}

// returns a pointer to the root_node
Trie_node *Trie::root() const {
	return root_node;
}

// accessor that determines if a word exists in the Trie
bool Trie::member(std::string const &str) const {

	// must ensure all the characters are alphabets 
	for (int i = 0; i < str.length(); i++){
		char stringToCheck = toupper(str[i]);
		int offset = static_cast<int>(stringToCheck - 'A');
		if (!isalpha(str[i])){
			throw illegal_argument();
		}
	}

	// make sure Trie isnt empty 
	if (empty()){
		return false; 
	}

	// call member on the root_node (always want to start at root_node so we don't miss any words)
	return root_node->member(str, 0);
}

// mutator that inserts words 
bool Trie::insert(std::string const &str) {

	// must check for alphabet 
	for (int i = 0; i < str.length(); i++){
		char stringToCheck = toupper(str[i]);
		int offset = static_cast<int>(stringToCheck - 'A');
		if (!isalpha(str[i])){
			throw illegal_argument();
		}
	}

	// if the root_node is empty the we must create a new trie node and call insert on that 
	if (empty()){
		root_node = new Trie_node();
	}
	
	// call insert 
	bool isInsert =  root_node->insert(str, 0); 

	// if we successfully inserted then increment the field trie_size 
	if (isInsert){
		trie_size++;
		return true;
	}

	// if not simply return false 
	else{
		return false; 
	}
}

// mutator that will remove words from the Trie 
bool Trie::erase(std::string const &str) {

	// check for alphabets 
	for (int i = 0; i < str.length(); i++){
		char stringToCheck = toupper(str[i]);
		int offset = static_cast<int>(stringToCheck - 'A');
		if (!isalpha(str[i])){
			throw illegal_argument();
		}
	}

	// only execute the mutator if the word is in the Trie 
	if (member(str)){
		
		// store result in a variable 
		bool isErase = root_node->erase(str, 0, root_node);

		// decrement trie_size if its true
		if (isErase){
			trie_size--;
			return true; 
		}
		// else it did not happen 
		else{
			return false;
		}
	}

	// if it is not a member then we are returning false (we could not erase it) 
	return false; 
}

// mutator that is supposed to completely delete all nodes in the tree 
void Trie::clear() {

	// we can proceed as long as we are not empty 
	if (!empty()){
		root_node->clear();
		trie_size = 0;
	}
}

// You can modify this function however you want:  it will not be tested

	std::ostream &operator<<(std::ostream &out, Trie const &trie) {
		/*	for ( Trie_node *ptr = trie.head(); ptr != nullptr; ptr = ptr->next() ) {
		out << "->" << ptr->value();
		} */

		out << "->0";

		return out;
	}


// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
