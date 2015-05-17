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

#ifndef DOUBLE_LIST_H
#define DOUBLE_LIST_H

#define nullptr 0

#include "ece250.h"
#include "Double_node.h"
#include "Exception.h"

#define nullptr 0

template <typename Type>
class Double_list {
private:
	Double_node<Type> *list_head;
	Double_node<Type> *list_tail;
	int list_size;

public:
	Double_list();
	Double_list(Double_list const &);
	~Double_list();

	// Accessors

	int size() const;
	bool empty() const;

	Type front() const;
	Type back() const;

	Double_node<Type> *head() const;
	Double_node<Type> *tail() const;

	int count(Type const &) const;

	// Mutators

	void swap(Double_list &);
	Double_list &operator=(Double_list const &);

	void push_front(Type const &);
	void push_back(Type const &);

	Type pop_front();
	Type pop_back();

	int erase(Type const &);

	// Friends

	template <typename T>
	friend std::ostream &operator<<(std::ostream &, Double_list<T> const &);
};

template <typename Type>
Double_list<Type>::Double_list() :
list_head(nullptr),
list_tail(nullptr),
list_size(0) {
	// empty constructor
}

template <typename Type>
Double_list<Type>::Double_list(Double_list<Type> const &list) :
list_head(nullptr),
list_tail(nullptr),
list_size(0) {
	// for loop to step through list provided
	for (Double_node<Type> *ptr = list.list_head; ptr != nullptr; ptr = ptr->next()) {
		// call push back and provide it the value of this node
		push_back(ptr->retrieve());
	}
}

template <typename Type>
Double_list<Type>::~Double_list<Type>() {
	// if list size is not zero keep popping 
	while (list_size != 0){
		pop_front();
	}
}

template <typename Type>
int Double_list<Type>::size() const {
	return list_size;
}

template <typename Type>
bool Double_list<Type>::empty() const {
	// if list size is 0 return true 
	if (list_size == 0){
		return true;
	}
	// return this by default
	return false;
}

template <typename Type>
Type Double_list<Type>::front() const {
	// cannot display front if empty list 
	if (empty()){
		throw underflow();
	}
	// get val at head
	else{
		return list_head->retrieve(); 
	}
}

template <typename Type>
Type Double_list<Type>::back() const {
	// same structure as front()
	if (empty()){
		throw underflow();
	}
	else{
		return list_tail->retrieve();
	}
}

template <typename Type>
Double_node<Type> *Double_list<Type>::head() const {
	return list_head; // returns pointer to head
}

template <typename Type>
Double_node<Type> *Double_list<Type>::tail() const {
	return list_tail; // returns pointer to tail 
}

template <typename Type>
int Double_list<Type>::count(Type const &obj) const {
	// initialize the count
	int count = 0; 
	// go through linked list 
	for (Double_node<Type> *ptr = head(); ptr != nullptr; ptr = ptr->next()) {
		// if val is the object increment count
		if (ptr->retrieve() == obj){
			count++;
		}
	}
	return count;

}

template <typename Type>
void Double_list<Type>::swap(Double_list<Type> &list) {
	std::swap(list_head, list.list_head);
	std::swap(list_tail, list.list_tail);
	std::swap(list_size, list.list_size);
}

template <typename Type>
Double_list<Type> &Double_list<Type>::operator=(Double_list<Type> const &rhs) {
	Double_list<Type> copy(rhs);

	swap(copy);

	return *this;
}

template <typename Type>
void Double_list<Type>::push_front(Type const &obj) {
	// if list empty, create a new node and set tail and head to it. hard set size to 1 (safer) 
	if (empty()){
		Double_node<Type> *first_element = new Double_node<Type>(obj, nullptr, nullptr);
		list_head = first_element;
		list_tail = first_element; 
		list_size = 1;
	}
	// if list not empty, create new node and set its next pointer to the old head
	else{
		Double_node<Type> *first_element = new Double_node<Type>(obj, nullptr, list_head);
		// set the old heads previous to the new 
		list_head->previous_node = first_element;
		// set the new head 
		list_head = first_element;
		// increment
		list_size++;
	}
}

template <typename Type>
void Double_list<Type>::push_back(Type const &obj) {
	// if empty same as push+front's empty case 
	if (empty()){
		Double_node<Type> *first_element = new Double_node<Type>(obj, nullptr, nullptr);
		list_head = first_element;
		list_tail = first_element;
		list_size = 1;
	}
	// if not, then create new node and set its previous to the old tail 
	else{
		Double_node<Type> *last_element = new Double_node<Type>(obj, list_tail, nullptr);
		// set old tails next to the new node 
		list_tail->next_node = last_element;
		// set tail to new one
		list_tail = last_element;
		list_size++; 
	}
}

template <typename Type>
Type Double_list<Type>::pop_front() {
	
	if (empty()){
		throw underflow();
	}
	
	else if (list_size == 1) {
		// get the val there
		Type object = front();
		// create temp variable to point to it
		Double_node<Type> *old_head = head();
		// set both to nulls 
		list_head = nullptr;
		list_tail = nullptr;
		// delete the temp variable and hard set list to 0 size
		list_size = 0;
		delete old_head; 
		return object; 
	}
	// if list has more than 1 object
	else{
		// get val of front node 
		Type object = front();
		// temp variable to point to it
		Double_node<Type> *old_head = head();
		// change head to the next node
		list_head = head()->next_node;
		// set previous to null of the new head
		list_head->previous_node = nullptr; 
		//decrement and delete pointer
		list_size--;
		delete old_head;
		return object; 
	}
}

template <typename Type>
Type Double_list<Type>::pop_back() {
	if (empty()){
		throw underflow();
	}
	// same case as pop front
	else if (list_size == 1) {
		Type object = back();
		Double_node<Type> *old_head = head();
		list_head = nullptr;
		list_tail = nullptr;
		list_size = 0;
		delete old_head;
		return object;
	}
	else{
		// get tail val 
		Type object = back();
		// create temp pointer
		Double_node<Type> *old_tail = list_tail;
		// change tail to previous node 
		list_tail = list_tail->previous_node;
		// set the next node to null
		list_tail->next_node = nullptr;
		// delete tail and decrement size
		delete old_tail;
		list_size--;
		return object;
	}
}

template <typename Type>
int Double_list<Type>::erase(Type const &obj) {
	// set default val of found to 0
	int found = 0; 
	// go through the list
	for (Double_node<Type> *ptr = head(); ptr != nullptr; ptr = ptr->next()) {
		// if the nodes val is equal to the obj
		if (ptr->retrieve() == obj){
			// if we are at head, call pop front
			if (ptr == list_head){
				pop_front();
			}
			// if at tail call pop back
			else if (ptr == list_tail){
				pop_back();
			}
			// if in middle
			else{
				// set the next node of the ptrs previous node to the ptrs next node 
				(ptr->previous_node)->next_node = ptr->next_node;
				// set the previous node of the ptrs next node to the ptrs previous node 
				(ptr->next_node)->previous_node = ptr->previous_node;
				// delete the ptr and decrement size 
				delete ptr;
				list_size--;
			}
			// since we have found something we need to return a val of 1 and break out of the loop 
			return 1;
		}
	}
	// else return 0
	return found;
}

// You can modify this function however you want:  it will not be tested

template <typename T>
std::ostream &operator<<(std::ostream &out, Double_list<T> const &list) {
	out << "head";

	for (Double_node<T> *ptr = list.head(); ptr != nullptr; ptr = ptr->next()) {
		out << "->" << ptr->retrieve();
	}

	out << "->0" << std::endl << "tail";

	for (Double_node<T> *ptr = list.tail(); ptr != nullptr; ptr = ptr->previous()) {
		out << "->" << ptr->retrieve();
	}

	out << "->0";

	return out;
}

// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
