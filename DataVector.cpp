// < DataVector.cpp >

/**
 * ASSIGNMENT 5
 * Name : Sahil Asawa
 * Roll No. : 22CS10065
*/

#include"DataVector.h"
using namespace std;

// Constructor
DataVector::DataVector(int dimension) {
    v.resize(dimension);                    // Change the dimension
    for(int i=0;i<dimension;i++) v[i]=0;    // Initialise all the elements to 0
}

// Function to insert data into the vector
// It takes the input as a vector of numbers
DataVector::DataVector(const vector<double> &data) {
    v = data;
}

// Destructor
DataVector::~DataVector() {
    v.clear();
}

// Copy constructor
DataVector::DataVector(const DataVector& other): v(other.v) {}

// Copy assignment operator
DataVector & DataVector::operator=(const DataVector &other) {
    v = other.v;
    return *this;
}

// Function to get the dimension of the vector
int DataVector::getDimension() {
    return v.size();
}

// Function to set the dimension of the vector
// Removes the existing data
// Dimension is set to 0 by default
void DataVector::setDimension(int dimension) {
    v.resize(dimension);
    for(int i=0;i<dimension;i++) v[i]=0;
}

// Function to get the component of the vector along a given direction
double DataVector::getComponent(int direction) {
    if(direction<0 || direction>=v.size()) {
        cout<<"INVALID DIRECTION\n";
        exit(1);
    }
    return v[direction];
}

// Function to set the component of the vector along a given direction
void DataVector::setComponent(int direction, double value) {
    if(direction<0 || direction>=v.size()) {
        cout<<"INVALID DIRECTION\n";
        exit(1);
    }
    v[direction] = value;
    return;
}

// Operator overloading to check equality of two vectors
bool DataVector::operator==(const DataVector &other) {
    return v==other.v;
}

// Operator overloading to perform addition of two vectors
DataVector DataVector::operator+(const DataVector &other) {
    DataVector sum(v.size());
    if(v.size() != other.v.size()) {
        cout<<"CAN NOT OPERATE ON VECTORS OF DIFFERENT DIMENSIONS\n";
        exit(1);
    }
    for(int i=0;i<v.size();i++) {
        sum.v[i] = v[i] + other.v[i];
    }
    return sum;
}

// Operator overloading to find the difference of two vectors
DataVector DataVector::operator-(const DataVector &other) {
    DataVector diff(v.size());
    if(v.size() != other.v.size()) {
        cout<<"CAN NOT OPERATE ON VECTORS OF DIFFERENT DIMENSIONS\n";
        exit(1);
    }
    for(int i=0;i<v.size();i++) {
        diff.v[i] = v[i] - other.v[i];
    }
    return diff;
}

// Operator overloading to find the dot product of two vectors
double DataVector::operator*(const DataVector &other) {
    if(v.size() != other.v.size()) {
        cout<<"CAN NOT OPERATE ON VECTORS OF DIFFERENT DIMENSIONS\n";
        exit(1);
    }
    double dotProd = 0;
    for(int i=0;i<v.size();i++) {
        dotProd += v[i] * other.v[i];
    }
    return dotProd;
}

// Function to find the magnitude of the vector
double DataVector::norm() {
    double mag = (*this) * (*this);
    return sqrt(mag);
}

// Function to find distance of this vector from a given vector
double DataVector::dist(const DataVector &other) {
    DataVector diff = *this - other;
    double d = diff.norm();
    return d;
}

// Utility function to print the vector
void DataVector::print() {
    cout<<" {";
    for(int i=0;i<v.size()-1;i++) cout<<v[i]<<',';
    cout<<v[v.size()-1]<<"}";
}