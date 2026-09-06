// < VectorDataset.h >

/**
 * ASSIGNMENT 5
 * Name : Sahil Asawa
 * Roll No. : 22CS10065
*/

#include"DataVector.h"

/**
 * The VectorDataset class is defined for storing a dataset of vectors.
 * Constructor initialises the dataset with the given vectors if it is given, otherwise as empty.
 * Copy constructor and copy assignment operator copy the data from one object of this class to other object.
 * setVector function sets the given vector as the ith vector of the dataset.
 * getVector function returns the ith vector of the dataset.
 * getSize function gives the size of the vector.
 * insert function adds a new vector at the end of the dataset.
 * readDataset function reads the data from the given csv file and adds the vectors to the dataset.
*/
class VectorDataset {
    vector<DataVector>dataset;
    public:
    // Constructor
    VectorDataset() {}

    // Constructor
    VectorDataset(vector<DataVector> &V): dataset(V) {}

    // Destructor
    ~VectorDataset(){
        dataset.clear();
    }

    // Copy constructor
    VectorDataset(const VectorDataset &other): dataset(other.dataset) {}

    // Copy Assignment operator
    VectorDataset &operator=(const VectorDataset &other) {
        dataset = other.dataset;
        return *this;
    }

    // Sets ith DataVector
    void setVector(int i, DataVector V) {
        dataset[i] = V;
    } 

    // Returns ith DataVector of the Dataset
    // i varies from 0 to size-1
    // Returns the first vector(if present) by default 
    DataVector getVector(int i = 0) {
        if(dataset.size()==0) {
            DataVector V(0);
            return V;
        }
        return dataset[i];
    }

    // Returns size of Dataset
    int getSize() {
        return dataset.size();
    }

    // Inserts the given DataVector at the end of the dataset
    void insert(DataVector V) {
        if(dataset.size() && dataset[0].getDimension()!=V.getDimension()) {
            cout<<"DIMENSION MISMATCH\n";
            exit(1);
        }
        dataset.push_back(V);
        return;
    }

    // Deletes the given DataVector(if found) from the dataset
    bool remove(DataVector V) {
        for(int i=0;i<dataset.size();i++) {
            if(dataset[i]==V) {
                dataset.erase(dataset.begin()+i);
                return 1;       // DataVector found and removed
            }
        }
        return 0;            // DataVector not found
    }

    // Utility function to print the dataset
    void print() {
        if(dataset.size()==0) {
            cout<<"[]\n";
            return;
        }
        cout<<"[";
        for(int i=0;i<dataset.size()-1;i++) {
            dataset[i].print();
            cout<<',';
        }
        dataset[dataset.size()-1].print();
        cout<<" ]\n";
    }

    // Reads the data from the csv file and inserts in the dataset
    void readDataset(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout<<"Error opening file!\n";  // Can't open the file
            exit(2);
        }
        string line;
        getline(file, line);        // Discard the first line containing column indexes
        while (getline(file, line)) {   // Read one line at a time
            stringstream ss(line);
            vector<double>v;
            string value;
            while (getline(ss, value, ',')) {   // Split the line across ','
                double element = stod(value);
                v.push_back(element);           // Insert the elements into the vector
            }
            DataVector V(v);            // Create a DataVector with the given data
            dataset.push_back(V);       // Insert the DataVector into the dataset
        }
        cout<<"Dataset read successfully ("<<dataset.size()<<" data vectors)\n";
        file.close();
    }  
};