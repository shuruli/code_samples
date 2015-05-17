/*****************************************
* UW User ID:  uwuserid
* Submitted for ECE 250
* Semester of Submission:  (Winter|Spring|Fall) 20NN
*
* By submitting this file, I affirm that
* I am the author of all modifications to
* the provided  code.
*****************************************/

#ifndef DYNAMIC_DUAL_STACK_H
#define DYNAMIC_DUAL_STACK_H

#ifndef nullptr
#define nullptr 0
#endif

#include <algorithm>
#include <cassert>
#include "Exception.h"

template <typename Type>
class Dynamic_dual_stack {
private:
	int initial_capacity;
	int array_capacity;
	Type *array;
	int stack_size[2];

public:
	Dynamic_dual_stack(int = 10);
	Dynamic_dual_stack(Dynamic_dual_stack const &);
	~Dynamic_dual_stack();

	int capacity() const;
	bool empty(int) const;
	int size(int) const;
	Type top(int) const;

	void swap(Dynamic_dual_stack &);
	Dynamic_dual_stack &operator=(Dynamic_dual_stack const &);
	void push(int, Type const &);
	Type pop(int);
	void clear();

	// Friends

	template <typename T>
	friend std::ostream &operator<<(std::ostream &, Dynamic_dual_stack<T> const &);
};

template <typename Type>
Dynamic_dual_stack<Type>::Dynamic_dual_stack(int n) :
initial_capacity(std::max(1, n)),
array_capacity(initial_capacity),
array(new Type[initial_capacity]) {
	// Enter your implementation here.
	
	// handles invalid cases of initial capacity 
	if (n <= 0){
		initial_capacity = 1;
	}
	else{
		initial_capacity = n;
	}
	
	// set array cap to initial cap
	array_capacity = initial_capacity;
}

template <typename Type>
Dynamic_dual_stack<Type>::Dynamic_dual_stack(Dynamic_dual_stack const &stack) :
initial_capacity(stack.initial_capacity),
array_capacity(stack.array_capacity),
array(new Type[array_capacity]) {
	stack_size[0] = stack.stack_size[0];
	stack_size[1] = stack.stack_size[1];

	// The above initializations copy the values of the appropriate
	// member variables and allocate memory for the data structure; 
	// however, you must still copy the stored objects.
	
	// Enter your implementation here.
	// copy the provided stack array into our array
	for (int i = 0; i < array_capacity; i++){
		array[i] = stack.array[i];
	}

}

template <typename Type>
Dynamic_dual_stack<Type>::~Dynamic_dual_stack() {
	// Enter your implementation here.
	// delete the array holding the stack 
	delete []array;
}

template <typename Type>
int Dynamic_dual_stack<Type>::capacity() const {
	// Enter your implementation here.
	return array_capacity;
}

template <typename Type>
bool Dynamic_dual_stack<Type>::empty(int m) const {
	// if m is invalid throw invalid exception 
	if (m != 0 && m != 1){
		throw illegal_argument();
	}
	// Enter your implementation here.
	// check if stack_size of the provided m is 0
	if (stack_size[m] == 0){
		return true;
	}
	return false;
}

template <typename Type>
int Dynamic_dual_stack<Type>::size(int m) const {
	// same case as above 
	if (m != 0 && m != 1){
		throw illegal_argument();
	}
	// return the stack_size
	return stack_size[m];
}

template <typename  Type>
Type Dynamic_dual_stack<Type>::top(int m) const {
	// Enter your implementation here.
	if (m != 0 && m != 1){
		throw illegal_argument();
	}
	
	// if the m is valid but the stack is empty then we cannot access top and must throw
	// an error 
	if (stack_size[m] == 0){
		throw underflow();
	}
	
	// if we are stack one, simply access like a normal array 
	if (m == 0){
		int top_element = stack_size[0];
		return array[top_element - 1];
	}
	
	// if its the second one, then we must access the length of our stack 
	// subtract the length of the second stack size 
	else{
		return array[array_capacity - stack_size[1]];
	}
	
}

template <typename Type>
void Dynamic_dual_stack<Type>::swap(Dynamic_dual_stack<Type> &stack) {
	std::swap(initial_capacity, stack.initial_capacity);
	std::swap(array_capacity, stack.array_capacity);
	std::swap(array, stack.array);
	std::swap(stack_size[0], stack.stack_size[0]);
	std::swap(stack_size[1], stack.stack_size[1]);
}

template <typename Type>
Dynamic_dual_stack<Type> &Dynamic_dual_stack<Type>::operator=(Dynamic_dual_stack<Type> const &rhs) {
	Dynamic_dual_stack<Type> copy(rhs);

	swap(copy);

	return *this;
}

template <typename  Type>
void Dynamic_dual_stack<Type>::push(int m, Type const &obj) {

	if (m != 0 && m != 1){
		throw illegal_argument();
	}
	//handles front of array 
	if (m == 0){
	
		// if we haven't reached the capacity just simply add it and increment 
		if ((stack_size[0] + stack_size[1]) != array_capacity){
			array[stack_size[0]] = obj; 
			stack_size[0]++; 
		}
		// we have reached array capacity 
		else{
			// holds new length 
			int new_array_capacity = 2 * array_capacity;
			
			// declares a new array that has this capacity 
			Type *new_array = new Type[new_array_capacity];
			
			// copies the front of the array into our new array 
			for (int i = 0; i < stack_size[0]; i++){
				new_array[i] = array[i];
			}
			
			// copies the back of the array (second stack) into our new array 
			for (int i = 0; i < stack_size[1]; i++){
				new_array[new_array_capacity - 1 - i] = array[array_capacity - 1 - i];
			}
			
			// add the new obj into the front 
			new_array[stack_size[0]] = obj;
			
			// pointer to our array 
			Type *tmp = array;
			
			// make our old array equal to the new one 
			array = new_array;
			
			// new array points to the temp 
			new_array = tmp;
			
			// set array capacity 
			array_capacity = new_array_capacity;
			
			// delete old array pointer and increment stack size 
			delete[]new_array;
			stack_size[0]++;
		}
	}
	
	// if we are adding ot the back, we do exactly the same checks as the front except with 
	// back. The only different code is commented below. 
	else{
		if ((stack_size[0] + stack_size[1]) != array_capacity){
		
			// must use code to access the back now 
			array[array_capacity - (1 + stack_size[1])] = obj;
			stack_size[1]++;
		}
		else{
			int new_array_capacity  = 2 * array_capacity;
			Type *new_array = new Type[new_array_capacity];
			for (int i = 0; i < stack_size[0]; i++){
				new_array[i] = array[i];
			}
			for (int i = 0; i < stack_size[1]; i++){
				new_array[new_array_capacity - 1 - i] = array[array_capacity - 1 - i];
			}
			
			// we are adding to the back of the array now 
			new_array[new_array_capacity -1 - stack_size[m]] = obj;
			Type *tmp = array;
			array = new_array;
			new_array = tmp;
			array_capacity = new_array_capacity;
			delete[]new_array;
			stack_size[1]++;

		}
	}
}

template <typename  Type>
Type Dynamic_dual_stack<Type>::pop(int m) {
	// Enter your implementation here.
	if (m != 0 && m != 1){
		throw illegal_argument();
	}
	if (stack_size[m] == 0){
		throw underflow();
	}
	
	// declare variable to hold our popped value 
	Type object_found;
	
	// if front just grab it 
	if (m == 0){
		object_found = array[stack_size[m] - 1];
	}
	
	// if back create the effective address then access it 
	else{
		int effective_address = array_capacity - stack_size[m];
		object_found = array[effective_address];
	}
	
	// decrement the stack size 
	if (stack_size[m] != 0){
		stack_size[m]--;
	}
	
	// get combined stack size 
	int combined_stack_size = stack_size[0] + stack_size[1];
	
	// if combined is less than quarter of the array cap 
	if (combined_stack_size <= (array_capacity / 4)){
		// half the new array cap but before you create the new array check to see
		// if this new array cap is less than initial 
		int new_array_capacity = array_capacity / 2;
		if (new_array_capacity < initial_capacity){
			new_array_capacity = initial_capacity;
		}
		
		// create new array and copy the values 
		Type *new_array = new Type[new_array_capacity];
		for (int i = 0; i < stack_size[0]; i++){
			new_array[i] = array[i];
		}
		for (int i = 0; i < stack_size[1]; i++){
			new_array[new_array_capacity - 1 - i] = array[array_capacity - 1 - i];
		}
		
		// code to delete the array (handles memory essentially)
		Type *tmp = array;
		array = new_array;
		new_array = tmp;
		array_capacity = new_array_capacity;
		delete[]new_array;
	}
	return object_found;

}

template <typename  Type>
void Dynamic_dual_stack<Type>::clear() {
	// Enter your implementation here.
	
	// set the stack sizes to 0 
	stack_size[0] = 0;
	stack_size[1] = 0;
	// if array cap greater than initial cap then delete the array create a
	// new array and reset array cap
	if (array_capacity > initial_capacity){
		delete []array; 
		array = new Type[initial_capacity];
		array_capacity = initial_capacity;
	}
	//array = new Type[initial_capacity];
}

// You can modify this function however you want:  it will not be tested

template <typename T>
std::ostream &operator<<(std::ostream &out, Dynamic_dual_stack<T> const &stack) {
	for (int i = 0; i < stack.capacity(); ++i) {
		if (i < stack.stack_size[0]) {
			out << stack.array[i] << " ";
		}
		else if (i >= stack.capacity() - stack.stack_size[1]) {
			out << stack.array[i] << " ";
		}
		else {
			out << "- ";
		}
	}

	return out;
}

// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
