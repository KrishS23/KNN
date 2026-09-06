// <TreeIndex.cpp>

/**
 * ASSIGNMENT 5
 * Name : Sahil Asawa
 * Roll No. : 22CS10065
*/

#include"TreeIndex.h"
#include <chrono>

//TreeIndex

// Initialize the static instance pointer
TreeIndex* TreeIndex::TreeIndexInstance = NULL; 

// GetInstance function
// Creates a new object if it does not exist or returns the existing object
TreeIndex &TreeIndex::GetInstance() {
    if(TreeIndexInstance==NULL) {   // No object of exists
        TreeIndexInstance = new TreeIndex();    // Create a new object
    }
    return *TreeIndexInstance;  
}

// Set the dataset in TreeIndex with the given VectorDataset
void TreeIndex::setDataset(VectorDataset &V) {
    D = V;
    return;
}

// Read the dataset from the given file and set it in TreeIndex
void TreeIndex::readDataset(string filename) {
    D.readDataset(filename);
    return;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ KDTreeIndex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Initialize the static instance pointer
KDTreeIndex* KDTreeIndex::KDTreeIndexInstance = NULL;

// GetInstance function
// Creates a new object if it does not exist or returns the existing object
KDTreeIndex &KDTreeIndex::GetInstance() {
    if(KDTreeIndexInstance==NULL) {  // No object of exists
        KDTreeIndexInstance = new KDTreeIndex();    // Create a new object
    }
    return *KDTreeIndexInstance;
}

// Constructor
KDTreeIndex::KDTreeIndex() {
    root = NULL;
    M = 1;
}

// Destructor
KDTreeIndex::~KDTreeIndex() {
    if(root!=NULL) delete root;
}

// Creates the KDTree from the dataset
void KDTreeIndex::MakeTree() {
    int N = D.getSize();
    vector<int> S(N);
    // Initialize S with indices of the data vectors
    for(int i=0;i<N;i++) S[i] = i;

    // Delete the previous tree
    delete root;
    root = NULL;

    // Build the tree
    MakeSubTree(root, S, 0);
    cout<<"KDTree constructed successfully\n";
    return;
}

// Helper function to create the subtree rooted at the given node from the dataset
Node* KDTreeIndex::MakeSubTree(Node* &node, vector<int> &S, int depth) {
    // Base case
    if(S.size()<1) return NULL;

    // Create a new node
    node = new Node();
    node->S = S;
    node->rule.resize(S.size(), 0);
    node->depth = depth;

    // Choose rule for the all the vectors in the node
    chooseRule(node);

    // Node is a leaf node
    if(S.size()<=M) return node;

    // Split the data vectors into two parts
    vector<int> SL, SR;
    for(int i=0;i<S.size();i++) {
        if(node->rule[i]) SL.push_back(S[i]);
        else SR.push_back(S[i]);
    }
    MakeSubTree(node->left, SL, depth+1);   // Create left subtree
    MakeSubTree(node->right, SR, depth+1);  // Create right subtree
    return node;
}

// Devides the data vectors in the node into two parts based on the median of a coordinate direction
void KDTreeIndex::chooseRule(Node* node) {
    // Number of data vectors in the current node
    int N = node->S.size();

    // Direction for splitting
    int dir = (node->depth)%(D.getVector(node->S[0]).getDimension());

    // Find the median
    vector<pair<double, int>>v;
    for(int i=0;i<N;i++) {
        v.push_back({D.getVector(node->S[i]).getComponent(dir), i});
    }
    sort(v.begin(), v.end());
    node->median = v[(N-1)/2].first;

    // Define the rule
    int cnt = (N+1)/2;
    for(int i=0;i<N;i++) {
        double x = D.getVector(node->S[i]).getComponent(dir);
        if(x < node->median) {
            node->rule[i]=1;
            cnt--;
        }
    }
    // Place the data vectors equal to median such that the node is divided equally
    for(int i=0;i<N;i++) {
        if(cnt==0) break;
        double x = D.getVector(node->S[i]).getComponent(dir);
        if(x == node->median) {
            node->rule[i]=1;
            cnt--;
        }
    }
    return;
}

// Inserts a new data vector into the dataset and rebuilds the tree
void KDTreeIndex::AddData(DataVector v) {
    D.insert(v);
    MakeTree();
    return;
}

// Deletes the given data vector from the dataset and rebuilds the tree
void KDTreeIndex::RemoveData(DataVector v) {
    if(D.remove(v)) // DataVector found and removed
        MakeTree();
    // No need to rebuild the tree if the DataVector is not found
    return;
}

// Searches for the k nearest neighbors of the given data vector
VectorDataset KDTreeIndex::Search(DataVector q, int k) {
    // Invalid k
    if(k<=0) {
        cout<<"Number of neighbors should be POSITIVE\n";
        return VectorDataset();
    }
    // return all the data vectors if k is greater than the size of the dataset
    if(k>D.getSize()) return D;

    // Search for the k nearest neighbors
    SearchSubTree(root,q,k);

    // Create a dataset of the k nearest neighbors
    VectorDataset result;
    while(!knnList.empty()) {
        result.insert(D.getVector(knnList.top().second));
        knnList.pop();
    }
    return result;
}

// Helper function to search for the k nearest neighbors in the subtree rooted at the given node
// Returns the distance of the farthest neighbor in the knn list from the query data vector
double KDTreeIndex::SearchSubTree(Node* node, DataVector &q, int k) {
    // Invalid node
    if(node == NULL) return INFINITY;

    // Base case
    // Children of the node have less than k data vectors or the node is a leaf node
    if((node->S.size()>=k && node->S.size()<2*k) || node->isLeaf()) {
        // Select the k nearest neighbors and insert them into the knn list
        for(int i=0;i<node->S.size();i++) {
            double dist = D.getVector(node->S[i]).dist(q);
            if(i<k) knnList.push({dist, node->S[i]});
            else if(dist<knnList.top().first) {
                knnList.pop();
                knnList.push({dist, node->S[i]});
            }
        }
        return knnList.top().first;
    }

    double maxDist = 0;
    int depth = node->depth;
    double median = node->median;
    int dimension = D.getVector(node->S[0]).getDimension();
    // Direction changes with depth
    int direction = depth%dimension;    // Coordinate direction for splitting
    bool checkRule; // 0 for left, 1 for right

    // Search left subtree
    if(q.getComponent(direction) <= median) {
        maxDist = SearchSubTree(node->left, q, k);
        checkRule = 0;
    }
    // Search right subtree
    else {
        maxDist = SearchSubTree(node->right, q, k);
        checkRule = 1;
    }

    // Distance from boundary
    double boundaryDist = abs(q.getComponent(direction) - median);

    // Compare distance from boundary with maxDist
    if(boundaryDist < maxDist) { // Sibling boundary is closer than the farthest neighbor
        for(int i=0;i<node->rule.size();i++) {
            if(node->rule[i] == checkRule) {
                double dist = D.getVector(node->S[i]).dist(q);
                if(dist<knnList.top().first) {
                    knnList.pop();
                    knnList.push({dist, node->S[i]});
                }
            }
        }
    }

    // Update the distance of the farthest point
    maxDist = knnList.top().first;
    return maxDist;
}


// Prints the tree
void KDTreeIndex::PrintTree(bool printS, bool printRule, bool printDataVectors) {
    printSubTree(root, printS, printRule, printDataVectors);
    return;
}

// Helper function to print the sub tree rooted at the given node
void KDTreeIndex::printSubTree(Node *node, bool printS, bool printRule, bool printDataVectors) {
    if(node==NULL) return;
    cout<<"Depth = "<<node->depth<<'\n';
    if(printS) {
        cout<<"S = [ ";
        for(int i=0;i<node->S.size();i++) {
            cout<<node->S[i]<<' ';
        }
        cout<<"]\n";
    }
    if(printRule) {
        cout<<"Rule = [ ";
        for(int i=0;i<node->rule.size();i++) {
            cout<<node->rule[i]<<' ';
        }
        cout<<"]\n";
    }
    if(printDataVectors) {
        cout<<"DataVectors: [";
        for(int i=0;i<node->S.size();i++) {
            D.getVector(node->S[i]).print();
        }
        cout<<" ]\n";
    }
    cout<<'\n';
    printSubTree(node->left, printS, printRule, printDataVectors);
    printSubTree(node->right, printS, printRule, printDataVectors);
    return;
}


//~~~~~~~~~~~~~~~~~~~~~~~~ RPTreeIndex ~~~~~~~~~~~~~~~~~~~~~~~~//

// Initialize the static instance pointer
RPTreeIndex* RPTreeIndex::RPTreeIndexInstance = NULL;

// GetInstance function
// Creates a new object if it does not exist or returns the existing object
RPTreeIndex &RPTreeIndex::GetInstance() {
    if(RPTreeIndexInstance==NULL) { // No object of exists
        RPTreeIndexInstance = new RPTreeIndex();    // Create a new object
    }
    return *RPTreeIndexInstance;
}

// Constructor
RPTreeIndex::RPTreeIndex() {
    root = NULL;
    M = 1;
}

// Destructor
RPTreeIndex::~RPTreeIndex() {
    if(root!=NULL) delete root;
}

// Creates the RPTree from the dataset
void RPTreeIndex::MakeTree() {
    int N = D.getSize();
    vector<int> S(N);
    // Initialize S with indices of the data vectors
    for(int i=0;i<N;i++) S[i] = i;

    // Delete the previous tree
    if(root!=NULL) delete root;
    root = NULL;

    // Build the tree
    MakeSubTree(root, S, 0);
    cout<<"RPTree constructed successfully\n";
    return;
}

// Helper function to create the subtree rooted at the given node from the dataset
Node *RPTreeIndex::MakeSubTree(Node* &node, vector<int> &S, int depth) {
    // Base case
    if(S.size()<1) return NULL;

    // Create a new node
    node = new Node();
    node->S = S;
    node->rule.resize(S.size(), 0);
    node->depth = depth;

    // Choose rule for the all the vectors in the node
    chooseRule(node);

    // Node is a leaf node
    if(S.size()<=M) return node;

    // Split the data vectors into two parts
    vector<int> SL, SR;
    for(int i=0;i<S.size();i++) {
        if(node->rule[i]) SL.push_back(S[i]);
        else SR.push_back(S[i]);
    }
    MakeSubTree(node->left, SL, depth+1);   // Create left subtree
    MakeSubTree(node->right, SR, depth+1);  // Create right subtree
    return node;
}

// Devides the data vectors in the node into two parts based on the median of projections on a random unit vector
void RPTreeIndex::chooseRule(Node* node) {
    // Number of data vectors in the current node
    int N = node->S.size();
    // Dimension of the data vectors
    int dimension = D.getVector(node->S[0]).getDimension();

    // Random number generator
    random_device rd;
    mt19937 generator(rd());
    uniform_real_distribution<double> random(-1,1);
    uniform_int_distribution<int> randonIndex(0,N-1);

    // Random vector
    DataVector unit(dimension);
    for(int i=0;i<dimension;i++) {
        unit.setComponent(i, random(generator));
    }

    // Normalize the random vector
    double norm = unit.norm();
    for(int i=0;i<dimension;i++) {
        unit.setComponent(i, unit.getComponent(i)/norm);
    }
    node->unit = unit;

    // Random index
    int x_idx = randonIndex(generator);
    // Index of the farthest data vector from the randomly chosen data vector
    int y_idx = 0;

    // Find the farthest data vector from the randomly chosen data vector
    double maxDist = 0;
    for(int i=0;i<N;i++) {
        if(D.getVector(node->S[i]).dist(D.getVector(node->S[x_idx])) > maxDist) {
            maxDist = D.getVector(node->S[i]).dist(D.getVector(node->S[x_idx]));
            y_idx = i;
        }
    }

    // Randomly chosen data vector
    DataVector x = D.getVector(node->S[x_idx]);
    // Farthest data vector from the randomly chosen data vector
    DataVector y = D.getVector(node->S[y_idx]);

    // Random delta
    node->delta = random(generator)*6*(x.dist(y)) / sqrt(dimension);

    // Find the median of the projections of the data vectors on the random unit vector
    vector<pair<double,int>> v(N);
    for(int i=0;i<N;i++) {
        v[i] = {D.getVector(node->S[i])*unit, i};
    }
    sort(v.begin(), v.end());
    node->median = v[(N-1)/2].first;

    // Define the rule
    for(int i=0;i<N;i++) {
        double x = D.getVector(node->S[i])*unit;
        if(x <= node->median+node->delta) {
            node->rule[i]=1;
        }
    }
    return;
}

// Inserts a new data vector into the dataset and rebuilds the tree
void RPTreeIndex::AddData(DataVector v) {
    D.insert(v);
    MakeTree();
    return;
}

// Deletes the given data vector from the dataset and rebuilds the tree
void RPTreeIndex::RemoveData(DataVector v) {
    if(D.remove(v)) // DataVector found and removed
        MakeTree();
    // No need to rebuild the tree if the DataVector is not found
    return;
}

// Searches for the k nearest neighbors of the given data vector
VectorDataset RPTreeIndex::Search(DataVector q, int k) {
    // Invalid k
    if(k<=0) {
        cout<<"Number of neighbors should be POSITIVE\n";
        return VectorDataset();
    }
    // return all the data vectors if k is greater than the size of the dataset
    if(k>D.getSize()) return D;

    // Search for the k nearest neighbors
    SearchSubTree(root,q,k);

    // Create a dataset of the k nearest neighbors
    VectorDataset result;
    while(!knnList.empty()) {
        result.insert(D.getVector(knnList.top().second));
        knnList.pop();
    }
    return result;
}

// Helper function to search for the k nearest neighbors in the subtree rooted at the given node
// Returns the distance of the farthest neighbor in the knn list from the query data vector
double RPTreeIndex::SearchSubTree(Node* node, DataVector &q, int k) {
    // Invalid node
    if(node == NULL) return INFINITY;

    // Check if the query point lies in left region or right region
    bool toLeft = (q*node->unit <= node->median+node->delta);
    // Query point lies in left region but left subtree has less than k points
    bool leftBaseCase = toLeft && ((node->left==NULL) || node->left->S.size()<k);
    // Query point lies in right region but right subtree has less than k points
    bool rightBaseCase = !toLeft && ((node->right==NULL) || node->right->S.size()<k);

    // Base case
    if(node->isLeaf() || leftBaseCase || rightBaseCase) {
        // Select the k nearest neighbors and insert them into the knn list
        for(int i=0;i<node->S.size();i++) {
            double dist = D.getVector(node->S[i]).dist(q);
            if(i<k) knnList.push({dist, node->S[i]});
            else if(dist < knnList.top().first) {
                knnList.pop();
                knnList.push({dist, node->S[i]});
            }
        }
        return knnList.top().first;
    }

    double maxDist = 0;

    // Search left subtree
    if(toLeft) {
        maxDist = SearchSubTree(node->left, q, k);
    }
    // Search right subtree
    else {
        maxDist = SearchSubTree(node->right, q, k);
    }

    // Distance from boundary
    double boundaryDist = abs(q*node->unit - node->median);

    // Compare distance from boundary with maxDist
    if(boundaryDist < maxDist) {    // Sibling boundary is closer than the farthest neighbor
        for(int i=0;i<node->rule.size();i++) {
            if(node->rule[i] == !toLeft) {
                double dist = D.getVector(node->S[i]).dist(q);
                if(dist < knnList.top().first) {
                    knnList.pop();
                    knnList.push({dist, node->S[i]});
                }
            }
        }
    }

    // Update the distance of the farthest point
    maxDist = knnList.top().first;
    return maxDist;
}


// Prints the tree
void RPTreeIndex::PrintTree(bool printS, bool printRule, bool printDataVectors) {
    printSubTree(root, printS, printRule, printDataVectors);
    return;
}

// Helper function to print the sub tree rooted at the given node
void RPTreeIndex::printSubTree(Node *node, bool printS, bool printRule, bool printDataVectors) {
    if(node==NULL) return;
    cout<<"Depth = "<<node->depth<<'\n';
    if(printS) {
        cout<<"S = [ ";
        for(int i=0;i<node->S.size();i++) {
            cout<<node->S[i]<<' ';
        }
        cout<<"]\n";
    }
    if(printRule) {
        cout<<"Rule = [ ";
        for(int i=0;i<node->rule.size();i++) {
            cout<<node->rule[i]<<' ';
        }
        cout<<"]\n";
    }
    if(printDataVectors) {
        cout<<"DataVectors: [";
        for(int i=0;i<node->S.size();i++) {
            D.getVector(node->S[i]).print();
        }
        cout<<" ]\n";
    }
    cout<<'\n';
    printSubTree(node->left, printS, printRule, printDataVectors);
    printSubTree(node->right, printS, printRule, printDataVectors);
    return;
}



// Main function
int main() {
    int k = 5;
    int n = 100;
    string test = "fmnist-test.csv";
    string train = "fmnist-train.csv";
    VectorDataset testDataset, trainDataset;
    cout.precision(10);

    clock_t start = clock();
    testDataset.readDataset(test);
    trainDataset.readDataset(train);
    // n = testDataset.getSize();
    cout << "Time taken to read the data is : " << double(clock() - start)/CLOCKS_PER_SEC  << " sec\n";
    
    start = clock();
    KDTreeIndex &KDTree = KDTreeIndex::GetInstance();
    KDTree.setDataset(trainDataset);
    KDTree.MakeTree();
    clock_t end = clock();
    cout<< "Time taken to build KDTree is : " << fixed << double(end - start)/CLOCKS_PER_SEC << " sec\n";

    start = clock();
    cout<<"Searching using K-D Tree";
    for(int i=0;i<n;i++) {
        VectorDataset D1 = KDTree.Search(testDataset.getVector(i), k);
        // D1.print();
    }
    cout<<'\n';
    end = clock();
    cout << "Number of queries : " << n << '\n';
    cout << "Total time taken is : " << fixed << double(end - start)/CLOCKS_PER_SEC << " sec\n";
    cout << "Time taken per query is : " << fixed << double(end - start)/CLOCKS_PER_SEC/n << " sec\n";
    cout << "Queries per second is : " << fixed << n/(double(end - start)/CLOCKS_PER_SEC) << "\n";

    start = clock();
    RPTreeIndex &RPTree = RPTreeIndex::GetInstance();
    RPTree.setDataset(trainDataset);
    RPTree.MakeTree();
    end = clock();
    cout<< "Time taken to build RP Tree is : " << fixed << double(end - start)/CLOCKS_PER_SEC << " sec\n";

    start = clock();
    cout<<"Searching using RP Tree";
    for(int i=0;i<n;i++) {
        VectorDataset D1 = RPTree.Search(testDataset.getVector(i), k);
        // D1.print();
    }
    cout<<'\n';
    end = clock();
    cout << "Number of queries : " << n << '\n';
    cout << "Total time taken is : " << fixed << double(end - start)/CLOCKS_PER_SEC << " sec\n";
    cout << "Time taken per query is : " << fixed << double(end - start)/CLOCKS_PER_SEC/n << " sec\n";
    cout << "Queries per second is : " << fixed << n/(double(end - start)/CLOCKS_PER_SEC) << "\n";

    return 0;
}