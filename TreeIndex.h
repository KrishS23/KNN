// <TreeIndex.h>

/**
 * ASSIGNMENT 5
 * Name : Sahil Asawa
 * Roll No. : 22CS10065
*/

#include"VectorDataset.h"
#include<random>

/**
 * The TreeIndex class provides a structure to handle the vector dataset
 * and quickly search for the k nearest neighbours of a given query data vector.
 * The dataset is stored in the VectorDataset object.
 * The TreeIndex class initially divides all the training data based on different rules.
 * There are two derived classes of this class, KDTreeIndex and RPTreeIndex.
 * KDTreeIndex class uses median of a random direction as the splitting rule.
 * RPTreeIndex class uses random projection on random unit direction as the splitting rule.
*/

// Node class for the tree
class Node {
public:
    // Constructor
    Node(): left(NULL), right(NULL) {}
    // Destructor
    ~Node() {
        // Recursively delete the left and right nodes
        if(left!=NULL) delete left;
        if(right!=NULL) delete right;
    }
    Node *left, *right; // Left and right children
    vector<int>S;       // Stores the indices of the vectors in the dataset
    vector<bool>rule;   // Stores the rule for the node
    DataVector unit;    // Random unit vector for the splitting
    int depth;          // Depth of the node
    double median;      // Median
    double delta;       // Delta
    // Function to check if the node is a leaf node
    bool isLeaf() {
        return (left==NULL && right==NULL);
    }
};

// TreeIndex class
class TreeIndex {
protected:
    TreeIndex() {}                          // Constructor
    ~TreeIndex() {}                         // Destructor
    static TreeIndex *TreeIndexInstance;    // Static Instance of the class
    VectorDataset D;                        // Vector Dataset

public:
    static TreeIndex &GetInstance();        // Function to get the instance of the class
    void setDataset(VectorDataset &V);      // Function to set the dataset
    void readDataset(string filename);      // Function to read the dataset from the given csv file
};

// KDTreeIndex class
class KDTreeIndex : public TreeIndex {
    static KDTreeIndex *KDTreeIndexInstance;     // Static Instance of the class
    priority_queue<pair<double,int>> knnList;    // Max heap to store the k nearest neighbours
    int M;                                       // Number of vectors in leaf node
    Node* root;                                  // Root of the tree

public:
    static KDTreeIndex &GetInstance();                  // Get the instance of the class
    void MakeTree();                                    // Make the tree
    void AddData(DataVector v);                         // Add a vector to the dataset
    void RemoveData(DataVector v);                      // Remove a vector from the dataset
    VectorDataset Search(DataVector q, int k=1);       // Search for the k nearest neighbours of the given vector
    void PrintTree(bool printS=0,bool printRule=0,bool printDataVectors=0); // Print the tree

private:
    KDTreeIndex();           // Constructor
    ~KDTreeIndex();          // Destructor
    Node* MakeSubTree(Node* &node, vector<int> &S, int depth=0);    // Recursively builds the tree
    void chooseRule(Node* node);                                    // Chooses the rule for the node
    double SearchSubTree(Node* node, DataVector &q, int k=1);       // Recursively searches for the k nearest neighbours
    void printSubTree(Node *node, bool printS, bool printRule, bool printDataVectors);  // Recursively prints the tree
};


// RPTreeIndex class
class RPTreeIndex : public TreeIndex {
    static RPTreeIndex *RPTreeIndexInstance;        // Static Instance of the class
    priority_queue<pair<double,int>> knnList;       // Max heap to store the k nearest neighbours
    int M;                                          // Number of vectors in leaf node
    Node* root;                                     // Root of the tree

public:
    static RPTreeIndex &GetInstance();              // Get the instance of the class
    void MakeTree();                                // Make the tree
    void AddData(DataVector v);                     // Add a vector to the dataset
    void RemoveData(DataVector v);                  // Remove a vector from the dataset
    VectorDataset Search(DataVector q, int k=1);   // Search for the k nearest neighbours of the given vector
    void PrintTree(bool printS=0, bool printRule=0, bool printDataVectors=0);   // Print the tree

private:
    RPTreeIndex();          // Constructor
    ~RPTreeIndex();         // Destructor
    Node* MakeSubTree(Node* &node, vector<int> &S, int curr=1);     // Recursively builds the tree
    void chooseRule(Node *node);                                    // Chooses the rule for the node
    double SearchSubTree(Node* node, DataVector &q, int k=1);       // Recursively searches for the k nearest neighbours
    void printSubTree(Node *node, bool printS, bool printRule, bool printDataVectors);  // Recursively prints the tree
};