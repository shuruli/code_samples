/*****************************************
* UW User ID:  uwuserid
* Submitted for ECE 250
* Department of Electrical and Computer Engineering
* University of Waterloo
* Calender Term of Submission:  (Winter|Spring|Fall) 20NN
*
* By submitting this file, I affirm that
* I am the author of all modifications to
* the provided code.
*****************************************/

#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#ifndef nullptr
#define nullptr 0
#endif

#include "ece250.h"
// Replace: 'Single_list' (two changes) with whatever linked-list structure you used in Project 1
//          'Single_node' (two changes) with 'Double_node' if your Project 1 involved doubly linked lists
#include "Double_list.h"
#include "Exception.h"
#include <iostream>

template <typename Type>
class Linked_queue {
private:
	static int const ARRAY_CAPACITY = 8;

	Double_list<Type *> list;
	int ifront;
	int iback;
	int queue_size;

public:
	Linked_queue();
	Linked_queue(Linked_queue const &);
	~Linked_queue();

	bool empty() const;
	int size() const;
	int list_size() const;

	Type front() const;

	void swap(Linked_queue &);
	Linked_queue &operator=(Linked_queue);
	void push(Type const &obj);
	Type pop();

	// Friends

	template <typename T>
	friend std::ostream &operator<<(std::ostream &, Linked_queue<T> const &);
};

template <typename Type>
Linked_queue<Type>::Linked_queue() :
queue_size(0) {
	// Empty constructor
}

template <typename Type>
Linked_queue<Type>::Linked_queue(Linked_queue const &queue) :
ifront(queue.ifront),
iback(queue.iback),
queue_size(queue.queue_size) {
	// enter your implementation here
	
	// get a double node pointer that stores a pointer to list head and traverses it 
	for (Double_node<Type *> *ptr = queue.list.head(); ptr != nullptr; ptr = ptr->next()) {
	
		// create a new temp array to store the arrays gathered as we traverse list 
		Type *temp_array = new Type[ARRAY_CAPACITY];
		Type *stored_array = ptr->retrieve();
		
		// go through 8 times and set our temp array to stored array then push onto list 
		for (int i = 0; i < 8; i++){
			temp_array[i] = stored_array[i];
		}
		list.push_back(temp_array);
	}
}

template <typename Type>
Linked_queue<Type>::~Linked_queue() {

	// get the arrays and delete 
	for (Double_node<Type *> *ptr = list.head(); ptr != nullptr; ptr = ptr->next()) {
		Type *array = ptr->retrieve();
		delete[]array;
	}
	// delete the actual linked list
	while (list.size() != 0){
		list.pop_front();
	}
}

template <typename Type>
bool Linked_queue<Type>::empty() const {
	if (queue_size == 0){
		return true;
	}
	else{
		return false;
	}
}

template <typename Type>
int Linked_queue<Type>::size() const {
	return queue_size;
}

// Do not change this implementation

template <typename Type>
int Linked_queue<Type>::list_size() const {
	return list.size();
}

template <typename Type>
Type Linked_queue<Type>::front() const {
	if (!empty()){
		return list.front()[ifront];
	}
	else{
		throw underflow();
	}
}

template <typename Type>
void Linked_queue<Type>::swap(Linked_queue<Type> &queue) {
	std::swap(list, queue.list);
	std::swap(queue_size, queue.queue_size);
	std::swap(ifront, queue.ifront);
	std::swap(iback, queue.iback);
}

template <typename Type>
Linked_queue<Type> &Linked_queue<Type>::operator=(Linked_queue<Type> rhs) {
	swap(rhs);

	return *this;
}

template <typename Type>
void Linked_queue<Type>::push(Type const &obj) {
	// enter your implementation here
	
	// if empty list then we create a new array and push it onto the node with the new obj in the first 
	// index of that array 
	if (list.empty()){
		Type *array = new Type[ARRAY_CAPACITY];
		list.push_front(array);
		ifront = 0;
		iback = 0;
		queue_size = 1;
		list.front()[ifront] = obj;
	}
	
	// if not then we get the last node in the list 
	else{
		Type *array_in_queue = list.back();
		
		// if iback is maxed out, create a new node with a blank array and add our obj there
		if (iback == 7){
			iback = 0;
			Type *array = new Type[ARRAY_CAPACITY];
			list.push_back(array);
			list.back()[iback] = obj;
		}
		// if not its a simple push onto the array 
		else{
			iback++;
			array_in_queue[iback] = obj;
		}
		
		// always performed
		queue_size++;
	}
}

template <typename Type>
Type Linked_queue<Type>::pop() {
	if (empty()){
		throw underflow();
	}
	// enter your implementation here
	// get the popped obj increment front and decrement overall size 
	Type popped_obj = list.front()[ifront];
	ifront++;
	queue_size--;
	
	// if the size is 0 then just pop of the front and delete the array 
	if (queue_size == 0){
		Type *temp = list.pop_front();
		delete[]temp;
		ifront = 0;
	}
	
	// if we have reached the array capacity after we have deleted we also need 
	// to delete front as outlined 
	if (ifront == ARRAY_CAPACITY){
		ifront = 0;
		Type *temp = list.front();
		delete[]temp;
		list.pop_front();
	}
	
	// return popped obj 
	return popped_obj;
}

// Do not modify this function

template <typename T>
std::ostream &operator<<(std::ostream &out, Linked_queue<T> const &queue) {
	if (queue.list.size() == 0) {
		out << "->0";
	}
	else if (queue.list.size() == 1) {
		out << "->[ ";

		for (int i = queue.ifront; i <= queue.iback; ++i) {
			out << queue.list.front()[i] << " ";
		}

		out << "]->0";
	}
	else {
		out << "->";

		for (Double_node<T *> *ptr = queue.list.head(); ptr != nullptr; ptr = ptr->next()) {
			out << "[ ";

			if (ptr == queue.list.head()) {
				for (int i = queue.ifront; i <= Linked_queue<T>::ARRAY_CAPACITY - 1; ++i) {
					out << ptr->retrieve()[i] << " ";
				}
			}
			else if (ptr == queue.list.tail()) {
				for (int i = 0; i <= queue.iback; ++i) {
					out << ptr->retrieve()[i] << " ";
				}
			}
			else {
				for (int i = 0; i <= Linked_queue<T>::ARRAY_CAPACITY - 1; ++i) {
					out << ptr->retrieve()[i] << " ";
				}
			}

			out << "]->";
		}

		out << "0";
	}

	return out;
}

// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
