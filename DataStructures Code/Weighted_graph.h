/*****************************************
* UW User ID:  sshuruli
* Submitted for ECE 250
* Semester of Submission:  Winter 2014
*
* By submitting this file, I affirm that
* I am the author of all modifications to
*  the provided code.
*
*****************************************/

#ifndef WEIGHTED_GRAPH_H
#define WEIGHTED_GRAPH_H

#ifndef nullptr
#define nullptr 0
#endif

#include <iostream>
#include <limits>


// Weighted Graph Class 
// Sudhanva Huruli 
// 2014-03-29
// Store a weighted graph with no direction 
// and give user access to insert nodes and 
// also query shortest distance between two nodes
class Weighted_graph {
	// all variables implemented underneath are private 
	// and as such can only be accessed with the public accessors
private:
	// Member variables:

	// Array of pointers 
	double **lower_triangular;

	// stores distances from the n to the ith index 
	double *stored_distances;

	int *deg; 

	// used in distance calculation to determine if we have visited the vertex
	bool *visited;

	//number of vertices and number of edges respectively
	int nov;
	int noe;

	//Class Constants:
	//our INF variable. Constant since our INF value should not change
	static const double INF;

	// can be accessed outside of this class
public:

	//Member Functions: Constructor/Destructor 
	Weighted_graph(int = 50);
	~Weighted_graph();

	//Member Accessors: 
	int degree(int) const;
	int edge_count() const;
	double adjacent(int, int) const;

	//Member Mutators:
	double distance(int, int) const;
	void insert(int, int, double);

	// Friends

	friend std::ostream &operator<<(std::ostream &, Weighted_graph const &);

};

// Declare our INF from the limits standard library
const double Weighted_graph::INF = std::numeric_limits<double>::infinity();

/*
* Weighted_graph::Weighted_graph(int n)
*
* Constructor that will initiate our graph with a default
* size of 50. If n <= 0 it will default to a size of 1
*
* Preconditions: N/A.
*
* Postconditions: N/A. The class is now initiated.
*
* Parameters: Size of the graph 'n' desired.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     followed structure of previous projects (very useful).
*/
Weighted_graph::Weighted_graph(int n) {

	// if n is less than or 0 default to 1 
	if (n <= 0){
		n = 1;
	}

	// allocate memory for all the arrays being used
	stored_distances = new double[n];
	visited = new bool[n];
	deg = new int[n];

	// Set degree to 0. This is the safe case where our n could be 1. 
	deg[0] = 0; 

	// if we have more than one vertex we will set it all to 0
	for (int i = 1; i < n; i++){
		deg[i] = 0;
	}

	//set nov to n and noe to 0
	nov = n;
	noe = 0;

	// create our 1st array of pointers 
	lower_triangular = new double *[n];

	// set the first index to 0 since it will NEVER be used. Anything
	// that is connected to 1 will have the adjacency stored in its  pointers. 
	lower_triangular[0] = nullptr;

	// ONLY if n > 1 we will create our second array and ALSO set deg[1] to 0
	if (n > 1){
		lower_triangular[1] = new double[n*(n - 1) / 2];
		deg[1] = 0;
	}

	// set the pointers of our first array to point in the proper location of the second array
	for (int i = 2; i < n; ++i) {
		lower_triangular[i] = lower_triangular[i - 1] + i - 1;
	}

	// set everything in the second array to infinity to begin with
	for (int i = 0; i < (n*(n-1)/2); i++){
		lower_triangular[1][i] = INF;
	}
}

/*
* Weighted_graph::~Weighted_graph()
*
* Desctructor that will deallocate all memory
* used by this class
*
* Preconditions: Class must be initiated of course.
*
* Postconditions: All the memory allocated is destroyed.
* At this point the object cannot be used.
*
* Parameters: N/A.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     followed structure of previous projects (very useful)
*/
Weighted_graph::~Weighted_graph(){

	// if and ONLY if n>1 do we delete the second array 
	if (nov > 1){
		delete[] lower_triangular[1];
	}
	// free all other memory 
	delete[] lower_triangular;
	delete[] stored_distances;
	delete[] deg;
	delete[] visited;
}

/*
* int Weighted_graph::degree(int n) const
*
* Will return the degree of the vertex that is passed in
*
* Preconditions: That vertex should be there
*
* Postconditions: None. This is just an accessor.
*
* Parameters: the nth vertex.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     // if applicable
*/
int Weighted_graph::degree(int n) const{
	// if the vertex at the nth location is 
	// defined then only return the corresponding degree
	// if n is out of range throw illegal argument 
	if (n >= nov || n  < 0){
		throw illegal_argument();
	}

	// return the nth cell in the diagonal
	return deg[n];
}

/*
* int Weighted_graph::edge_count() const
*
* Will return the number of edges currently in the graph
*
* Preconditions: None. Will simply return 0 if nothing is in the tree
*
* Postconditions: None. This is just an accessor.
*
* Parameters: N/A.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     // if applicable
*/
int Weighted_graph::edge_count() const{
	return noe;
}

/*
* double Weighted_graph::adjacent(int n, int m) const
*
* Will return the weight of two adjacent nodes
*
* Preconditions: The arguements passed in cannot be equal and
* must be within the valid ranges for vertex values
*
* Postconditions: None. This is just an accessor.
*
* Parameters: 'n' and 'm' vertex that the weight between we are looking for.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     // if applicable
*/
double Weighted_graph::adjacent(int n, int m) const{

	// if n or m is out of range throw illegal argument 
	if ((n >= nov || n  < 0) || (m >= nov || m < 0)){
		throw illegal_argument();
	}

	// if equal return 0
	if (n == m){
		return 0;
	}

	// our second number and first number are initialized to begin with
	int second_number = m;
	int first_number = n;

	// if n is less than m then we must switch the order or else we go out of bounds
	if (n < m){
		second_number = n;
		first_number = m;
	}
	// return the weightage
	return lower_triangular[first_number][second_number];

}

/*
* void Weighted_graph::insert(int n, int m, double w)
*
* Will insert two nodes with the weight specified
*
* Preconditions: The arguments passed in cannot be equal and
* must be within the valid ranges for vertex values. w must be >= 0
*
* Postconditions: The adjacency matrix will be updated at both the
* n-m cell and m-n cell. Degree of both nodes goes up by 1. Edges will
* go up by 1 and the vertices at both locations is set to 0 to indicate
* we have a vertex for that position.
*
* Parameters: 'n' and 'm' the two vertex values we are adding and weight w.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     // if applicable
*/
void Weighted_graph::insert(int n, int m, double w){

	// if weight is less than 0 throw 
	if (w <= 0){
		throw illegal_argument();

	}

	// if n or m are out of range OR are equal throw 
	if ((n >= nov || n  < 0) || (m >= nov || m < 0) || n == m){
		throw illegal_argument();
	}

	// same idea as adjacent
	int second_number = m;
	int first_number = n;

	if (n < m){
		second_number = n;
		first_number = m;
	}

	if (lower_triangular[first_number][second_number] == INF){
		// increment the degrees of both the nodes 
		deg[n]++;
		deg[m]++;
		noe++;
	}
	// update the second array with the proper weightage
	lower_triangular[first_number][second_number] = w;
}

/*
* double Weighted_graph::distance(int n, int m) const
*
* Will find distance between two nodes
*
* Preconditions: The arguments passed in cannot be equal and
* must be within the valid ranges for vertex values. If either
* vertex is not in the graph then return INF.
*
* Postconditions: Visited array will be changed with true at indexes
* at which we have been through. the Stored distances array will have the value
* of the distance from n to itself.
*
* Parameters: 'n' and 'm' the two vertex values we are finding distance between.
*
* Bugs:           // if applicable
* Todo:           // if applicable
* References:     // if applicable
*/
double Weighted_graph::distance(int n, int m) const{

	// regular error checking.
	if ((n >= nov || n  < 0) || (m >= nov || m < 0)){
		throw illegal_argument();
	}

	if (n == m){
		return 0;
	}

	// if one of the vertices are not in the graph
	// then the two vertices are disconnected. This would occur when the degrees of one or the other 
	// are 0
	if (deg[n] == 0 || deg[m] == 0){
		return INF;
	}


	// reset visited and stored distance to appropriate values
	for (int i = 0; i < nov; i++){
		visited[i] = false;
		stored_distances[i] = INF;

		// since we are starting at 'n' we will leave those at true 
		// and distance is obviously 0.
		if (i == n){
			visited[i] = true;
			stored_distances[i] = 0;
		}
	}

	// we are starting our pivot at n.
	int pivot = n;

	// Keep iterating until the visited array at the location we are finding distance to
	// is set to true at which point we are guaranteed to have the shortest distance to it. 
	while (visited[m] != true){

		// for each pivot we are going to go through all of the vertices to check if 
		// it is neighbours with ANY of them 
		for (int i = 0; i < nov; i++){

			// get the weight of the pivot with the i 
			double adjacent_weight = adjacent(pivot, i);

			// if they are neighbours (pivot and i) then the weight will not be 
			// 0 or infinity at which point we can update the distance from pivot to i
			if (adjacent_weight != INF && adjacent_weight != 0){

				// only update the distance if the sum of the weight and the stored 
				// distances to the pivot is less than the stored distances at i.
				// In other words, if the new calculated distance is shorter than the distance
				// already there we change the stored_distance.
				if (adjacent_weight + stored_distances[pivot] < stored_distances[i]){
					stored_distances[i] = adjacent_weight + stored_distances[pivot];
				}
			}
		}

		// this block of code determines the vertex in the graph with shortest distance
		// that is not visited yet (this determines our NEXT pivot). 
		double nextNode = INF;

		for (int i = 0; i < nov; i++){
			// if the vertex is not visited and its distance is not infinity(meaning its in the graph)
			if (visited[i] != true && stored_distances[i] != INF){
				// if the stored distance is less than the previous value of min it becomes the new
				// pivot.
				if (stored_distances[i] < nextNode){
					nextNode = stored_distances[i];
					pivot = i;
				}
			}
		}

		// set the visited at the pivot we are GOING to be iterating through to be true. 
		// this will be the pivot of the NEXT iteration not this iteration. 
		visited[pivot] = true;
	}
	return stored_distances[m];


}

std::ostream &operator<<(std::ostream &out, Weighted_graph const &graph) {
	for (int i = 0; i < graph.nov; i++){
	}
	return out;
}

// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
