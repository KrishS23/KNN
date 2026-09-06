// < DataVector.h >

/**
 * ASSIGNMENT 5
 * Name : Sahil Asawa
 * Roll No. : 22CS10065
*/
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<iostream>
#include<algorithm>
#include<cmath>
#include<queue>
using namespace std;

/**
 * The DataVector class provides a structure to handle vectors.
 * The vectors contain elements of double data type.
 * Dimension is the number of elements in the vetor.
 * There are two constructors for this class.
 * One initialises the class with given dimension and all the elements as 0.
 * Other initialises the class with same data as the given vector.
 * Copy constructor and copy assignment operator copy the data from one object of this class to other object.
 * Operator+, operator-, operator== are overloaded to perform their addition, subtraction and equality check respectively.
 * operator* gives the dot product of two vectors.
 * setDimension function sets the dimension of the vector and removes all the previous data by making all the elements 0.
 * norm function gives the magnitude of the vector.
 * dist function gives the distance between two vectors.
 * print function prints all the elements of the vector.
*/
class DataVector { 
    vector<double> v; 
    public: 
    DataVector(int dimension=0);                        // Constructor, initialises with the given dimension and all data as 0
    DataVector(const vector<double> &data);             // Constructor, initialises DataVector object with data from the given DataVector
    ~DataVector();                                      // Destructor
    DataVector(const DataVector& other);                // Copy constructor
    DataVector & operator=(const DataVector &other);    // Copy assignment operator
    int getDimension();                                 // Get dimension of the DataVector
    void setDimension(int dimension=0);                 // Set dimension of the DataVector
    double getComponent(int direction);                 // Get the component of the DataVector along the given direction
    void setComponent(int direction, double value);     // Set the component of the DataVector along the given direction
    bool operator==(const DataVector &other);           // Operator to check equality of two DataVectors
    DataVector operator+(const DataVector &other);      // Operator to add two DataVectors
    DataVector operator-(const DataVector &other);      // Operator to subtract two DataVectors
    double operator*(const DataVector &other);          // Operator to find dot product of two DataVectors
    double norm();                                      // Returns magnitude of the DataVector
    double dist(const DataVector &other);               // Returns distance of this DataVector from other DataVector
    void print();                                       // Utility function to print DataVector
};