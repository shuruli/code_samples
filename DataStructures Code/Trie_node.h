/*****************************************
* UW User ID:  sshuruli
* Submitted for ECE 250
* Department of Electrical and Computer Engineering
* University of Waterloo
* Calender Term of Submission:  (Winter) 2014
*
* By submitting this file, I affirm that
* I am the author of all modifications to
* the provided code.
*****************************************/

#ifndef SINGLE_NODE_H
#define SINGLE_NODE_H

#ifndef nullptr
#define nullptr 0
#endif

#include <string>
#include <cctype> 
#include "ece250.h"

class Trie;

class Trie_node {
private:
	Trie_node **children;
	bool is_terminal;
	static int const CHARACTERS = 26;


public:
	Trie_node();
	Trie_node *child(int) const;

	bool member(std::string const &, int) const;

	bool insert(std::string const &, int);
	bool erase(std::string const &, int, Trie_node *&);
	void clear();

	friend class Trie;

	// if ptr is a pointer to a Trie_node object
	// in one of the friendly classes, you should:
	//    use   ptr->next_   to modify it
	//    use   ptr->next()      to access it
};

Trie_node::Trie_node() :
children(nullptr),
is_terminal(false) {
	// empty constructor
}

Trie_node *Trie_node::child(int n) const {
	if (children == nullptr) {
		return nullptr;
	}
	else {
		return children[n];
	}
}

bool Trie_node::member(std::string const &str, int depth) const {

	// if for any reason depth > member just return false. 
	if (depth > str.length()){
		return false;
	}

	// once we reach the words length just return whether it is a word or not 
	if (str.length() == depth){
		return is_terminal;
	}

	// used to convert char to the equivalent integer (equivalent is used loosely here) 
	char stringToCheck = toupper(str[depth]);
	int offset = static_cast<int>(stringToCheck - 'A');

	// if i have children 
	if (children){

		// further, if i have that particular letter in my array 
		if (children[offset]){
			// call member on that 
			return children[offset]->member(str, depth + 1);
		}

		// if not then return false 
		else{
			return false;
		}
	}
	// if no children return false 
	else{
		return false;
	}
}

bool Trie_node::insert(std::string const &str, int depth) {

	// common check as member 
	if (depth > str.length()){
		return false;
	}

	// once we have reached the end of the string
	if (str.length() == depth){

		// if it is a terminal then we cannot insert it clearly 
		if (is_terminal){
			return false;
		}

		// if it is not a terminal, set this node to true and return true 
		else{
			is_terminal = true;
			return true;
		}
	}

	// common code to member (gets the offset) 
	char stringToCheck = toupper(str[depth]);
	int offset = static_cast<int>(stringToCheck - 'A');

	// if we have children 
	if (children){

		// if we do not have that letter already we must create it 
		if (!children[offset]){
			children[offset] = new Trie_node();
		}

		// call insert on the next letter 
		return children[offset]->insert(str, depth + 1);
	}

	// if we have no children, we create an array of pointers
	else{
		children = new Trie_node*[CHARACTERS];

		// ******************************** Credit of this for loop goes to Eric Chu *******************************************
		// Must set individual elements to nullptr in order for them to ACTUALLY be nullptr (otherwise it could just be garbage) 
		for (int i = 0; i < CHARACTERS; i++)
		{
			children[i] = nullptr;
		}

		// create the offset node 
		children[offset] = new Trie_node();

		// call insert on that 
		return children[offset]->insert(str, depth + 1);
	}

}

bool Trie_node::erase(std::string const &str, int depth, Trie_node *&ptr_to_this) {

	// common check 
	if (depth > str.length()){
		return false; 
	}

	// if we have reached the end of that word 
	if (str.length() == depth){

		// if we have further children then we just set terminal to false and return true (we did erase the word) 
		if (children){
			is_terminal = false;
			return true;
		}

		// if not, we must set ptr to this to nullptr and delete this then return true 
		else{
			ptr_to_this = nullptr;
			delete this;
			return true;
		}
	}

	// get the offset 
	char stringToCheck = toupper(str[depth]);
	int offset = static_cast<int>(stringToCheck - 'A');

	// store the result of calling erase on the next letter in a variable 
	bool return_variable = children[offset]->erase(str, depth + 1, children[offset]);

	// once we are done with erase on the next letter we come to this part of the code 

	// if we are a terminal node 
	if (is_terminal){

		// goes through all my children and checks if they are all nullptr 
		bool child_flag = false;
		for (int i = 0; i < CHARACTERS; i++){
			if (children[i]){
				child_flag = true;
			}
		}

		// if i had no children then i must delete the children and set children to nullptr 
		if (child_flag == false){
			delete[]children;
			children = nullptr;
		}

		// i have deleted a letter so return true 
		return true; 
	}

	// if i am not terminal 
	else{

		// same as above 
		bool child_flag = false; 
		for (int i = 0; i < CHARACTERS; i++){
			if (children[i]){
				child_flag = true;
			}
		}

		// if i do have children simply return what the erase call on the letter after me returned 
		if (child_flag){
			return return_variable;
		}

		// if i am not a terminal node and i have all children as nullptr then delete the array, delete myself and then 
		// set ptr_to_this to nullptr 
		else{
			delete[]children; 
			children = nullptr;
			delete this; 
			ptr_to_this = nullptr;
			return true; 
		}
	}

}

void Trie_node::clear() {

	// if i have no children then i am at a leaf node and therefore will delete the node and return 
	if (!children){
		delete this;
		return;
	}

	// if not, then go through all the children of my array and call clear on it 
	for (int i = 0; i < CHARACTERS; i++){
		if (children[i]){
			children[i]->clear();
		}
	}
	
	// once i have called clear on all my children i can delete the array and delete this
	delete[]children;
	delete this;
}


// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
