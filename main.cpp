#include <iostream>
#include <string>
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>


typedef long long ID;
typedef long long AID;


using namespace std;

class Customer{
public:
    Customer(ID id, string firstName, string lastName, string email, AID addressID){
        this->id = id;
        this->firstName = firstName;
        this->lastName = lastName;
        this->email = email;
        this->addressID = addressID;
    }
    void printToConsole(){
        cout << endl;
        cout << "Customer:" << endl;
        cout << "ID: " << this->id << endl;
        cout << "First name: " << this->firstName << endl;
        cout << "Last Name: " << this->lastName << endl;
        cout << "Email: " << this->email << endl;
        cout << "Address ID: " << this->addressID << endl;
    }
    ID getId(){
        return this->id;
    }
    string getLastName(){
        return this->lastName;
    }
    string getEmail(){
        return this->email;
    }
    string getFirstName(){
        return this->firstName;
    }
    AID getAddressId(){
        return this->addressID;
    }
    void setFirstName(string _firstName){
        this->firstName = _firstName;
    }
    void setLastName(string _lastName){
        this->lastName = _lastName;
    }
    void setEmail(string _email){
        this->email = _email;
    }
    void setAddress(AID _addressID){
        this->addressID = _addressID;
    }
private:
    ID id;
    string firstName;
    string lastName;
    string email;
    AID addressID;
};



template<typename T, int ORDER>
class BStarNode{
private:
    using GenericBStarNode = BStarNode<T, ORDER>;
    //current number of keys
    int n;
    //maximum number of keys
    int maxKeys;
    bool isRoot;
    int level;
    // keys
    vector<ID> keys;
    // array of pointers to data which is represented by keys
    // keys[i] is representing object that is data[i] pints to
    vector<T*> data;
    //subtrees
    vector<GenericBStarNode*> children;

    GenericBStarNode* parent;

    static constexpr int calculateRootOrder(int m){
        return 2 * static_cast<int>(((2.0/3.0)*(m-1))) + 1;
    }
public:
    BStarNode<T, ORDER>(bool root = false) : n(0), isRoot(root), level(0), parent(nullptr){
        this->maxKeys = root ? (calculateRootOrder(ORDER) - 1) : ORDER - 1;
        keys.resize(maxKeys);
        data.resize(maxKeys);
        children.resize(maxKeys + 1);
        for(int i = 0; i < maxKeys; i++){
            keys[i] = 0;
            data[i] = nullptr;
            children[i] = nullptr;
        }
        children[maxKeys] = nullptr;
    }
    GenericBStarNode* getParent(){
        return this->parent;
    }
    void setParent(GenericBStarNode* p){
        this->parent = p;
    }
    int getN(){
        return this->n;
    }
    void setN(int x){
        this->n = x;
    }
    int getLevel(){
        return this->level;
    }
    int getMaxKeys(){
        return this->maxKeys;
    }
    vector<ID>& getKeys() {
        return this->keys;
    }
    vector<GenericBStarNode*>& getChildren() {
        return this->children;
    }
    vector<T*>& getData() {
        return this->data;
    }
    bool getIsRoot(){
        return this->isRoot;
    }
    void setIsRoot(){
        isRoot = true;
        this->maxKeys = (calculateRootOrder(ORDER) - 1);

    }
    bool isLeaf(){
        return this->children[0] == nullptr;
    }
    bool isFull() {
        return n == maxKeys;
    }
    bool minKeys(){
        int minKeys = isRoot ? 1 : static_cast<int>(ceil((2.0*ORDER - 1)/3.0)) - 1;
        return n == minKeys;
    }
    void setLevel(int lvl){
        this->level = lvl;
    }
    void printNodeToConsole() {
        //cout << "\nmax keys: " << maxKeys << endl;
        for(int i = 0; i < n; i++){
            cout << "|" << this->keys[i];
        }
        cout << "|\t\t";
    }
};



template<typename T, int ORDER>
class BStarTree{
private:
    using GenericBStarNode = BStarNode<T, ORDER>;
    GenericBStarNode* root;

protected:
    //return index of first element that is greater than or equal to x or n if such element doesn't exist in arr
    int lowerBound (ID x, vector <ID>& arr, int n){
        int low = 0, high = n-1, mid;

        if(n == 0 || x > arr[high]) return n;
        if(x <= arr[0]) return 0;

        while(low < high){
            mid = (low + high)/2;

            if(arr[mid] < x){
                low = mid + 1;
            }
            else{
                high = mid;
            }
        }
        return low;
    }
    bool notFound(GenericBStarNode* node, ID key, int& i){
        i = lowerBound(key, node->getKeys(), node->getN());
        return i == node->getN() || node->getKeys()[i] != key;
    }
    GenericBStarNode* findNode(ID key, int* cnt = nullptr){
        GenericBStarNode* p = root;
        GenericBStarNode* q = nullptr;
        int i;
        while(p) {
            if(cnt){
                (*cnt)++;
            }
            q = p;
            bool keyNotFound = notFound(p, key, i);
            if (keyNotFound) {
                p = p->getChildren()[i];
            }
            else{
                return p;
            }
        }
        return q;
    }
    void insertInNode(GenericBStarNode* node, ID key, T* dataAdr, int i){
        int n = node->getN();
        vector<ID>& keys = node->getKeys();
        vector<T*>& data = node->getData();
        vector<GenericBStarNode*> children = node->getChildren();
        for(int j = n; j > i; j--){
            swap(keys[j], keys[j-1]);
            swap(data[j], data[j-1]);
        }
        keys[i] = key;
        data[i] = dataAdr;
        node->setN(n+1);

    }

    void combineTwoNodes(GenericBStarNode* left, GenericBStarNode* right, vector<GenericBStarNode *>& combChildren, vector<ID>& combKeys, vector<T*>& combData, vector<pair<ID,T*>>& combined, ID key, int dividingKeyIndex, T* dataAdr){
        vector<ID>& leftKeys = left->getKeys();
        vector<ID>& rightKeys = right->getKeys();

        //we have to take into account the children pointers because overflow can be done on higher level than leaf level
        // if we have done splitting and propagated key to parent which is also full, so we try overflow everytime this happens before splitting

        vector<GenericBStarNode *>& leftChildren = left->getChildren();
        vector<GenericBStarNode *>& rightChildren = right->getChildren();

        vector<T*>& leftData = left->getData();
        vector<T*>& rightData = right->getData();

        int numKeysLeft = left->getN();
        int numKeysRight = right->getN();
        int combinedSize = numKeysLeft + numKeysRight + 2;
        GenericBStarNode* parent = left->getParent();

        copy(leftKeys.begin(), leftKeys.end(), combKeys.begin());
        copy(rightKeys.begin(), rightKeys.begin() + numKeysRight, combKeys.begin() + numKeysLeft);
        combKeys.at(combinedSize-2) = key;
        combKeys.at(combinedSize-1) = parent->getKeys()[dividingKeyIndex];

        copy(leftData.begin(), leftData.end(), combData.begin());
        copy(rightData.begin(), rightData.begin() + numKeysRight, combData.begin() + numKeysLeft);
        combData.at(combinedSize-2) = dataAdr;
        combData.at(combinedSize-1) = parent->getData()[dividingKeyIndex];

        copy(leftChildren.begin(), leftChildren.begin() + numKeysLeft + 1, combChildren.begin());
        copy(rightChildren.begin(), rightChildren.begin() + numKeysRight + 1, combChildren.begin() + numKeysLeft + 1);

        for(int i = 0; i < combinedSize; i++){
            combined[i].first = combKeys[i];
            combined[i].second = combData[i];
        }

        sort(combKeys.begin(), combKeys.end());
        sort(combined.begin(), combined.end());

    }
    void splitRoot(GenericBStarNode* start, ID key, T* dataAdr, GenericBStarNode* newNode = nullptr){
        int ind, n, dividingIndex;
        n = start->getN();

        vector<ID> keys(n);
        vector<T*> data(n);
        vector<GenericBStarNode*> children(n+1);

        copy(start->getChildren().begin(), start->getChildren().begin() + n + 1, children.begin());
        copy(start->getKeys().begin(), start->getKeys().begin() + n, keys.begin());
        copy(start->getData().begin(), start->getData().begin() + n, data.begin());



        ind = lowerBound(key, keys, n);
        keys.insert(keys.begin() + ind, key);
        data.insert(data.begin() + ind, dataAdr);

        GenericBStarNode* left = new GenericBStarNode();
        GenericBStarNode* right = new GenericBStarNode();



        dividingIndex = ((n+1) % 2 == 0) ? (n+1)/2 -1 : (n+1)/2;
        if(!start->isLeaf()) {
            int index = lowerBound(newNode->getKeys()[newNode->getN() - 1], keys, n+1);

            children.insert(children.begin() + index, newNode);
        }
        for(int i = 0; i <= n; i++){
            if(i < dividingIndex){
                left->getKeys()[i] = keys[i];
                left->getData()[i] = data[i];

                if(!start->isLeaf()){
                    left->getChildren()[i] = children[i];
                    left->getChildren()[i+1] = children[i+1];
                    children[i]->setParent(left);
                    children[i+1]->setParent(left);
                }
            }
            else if(i > dividingIndex){
                int j = i - dividingIndex - 1;
                right->getKeys()[j] = keys[i];
                right->getData()[j] = data[i];

                if(!start->isLeaf()){
                    right->getChildren()[j] = children[i];
                    right->getChildren()[j+1] = children[i+1];
                    children[i]->setParent(right);
                    children[i+1]->setParent(right);
                }

            }
        }

        left->setParent(root);
        right->setParent(root);
        left->setN(dividingIndex);
        right->setN(n - dividingIndex);
        left->setLevel(1);
        right->setLevel(1);
        start->getChildren()[0] = left;
        start->getChildren()[1] = right;
        start->getKeys()[0] = keys[dividingIndex];
        start->getData()[0] = data[dividingIndex];
        start->setN(1);

    }

    bool splitOperation(GenericBStarNode* left, GenericBStarNode* right, ID& key, T*& dataAdr, int dividingKeyIndex, GenericBStarNode*& propagationNode){
        int combinedSize = left->getN() + right->getN() + 2;
        GenericBStarNode* parent = left->getParent();

        //each node has one subtree more than keys, and I am adding one more to point to newly created node on lower level (by splitting 2 to 3)
        vector<GenericBStarNode *> combChildren(combinedSize + 1);
        vector<ID> combKeys(combinedSize);
        vector<T*> combData(combinedSize);
        vector<pair<ID,T*>> combined(combinedSize);

        combineTwoNodes(left, right, combChildren, combKeys, combData, combined, key, dividingKeyIndex, dataAdr);

        int newDividingIndex1 = static_cast<int>(2.0/3.0 * (ORDER - 1));
        int newDividingIndex2 = newDividingIndex1 + static_cast<int>((2.0*ORDER - 1)/3.0) + 1;

        GenericBStarNode* newNode = new GenericBStarNode();

        if(!left->isLeaf()){
            int ind = lowerBound(propagationNode->getKeys()[propagationNode->getN()-1],combKeys, combinedSize);

            combChildren.insert(combChildren.begin() + ind, propagationNode);
        }

        //set new dividing key and address
        parent->getKeys()[dividingKeyIndex] = combined[newDividingIndex1].first;
        parent->getData()[dividingKeyIndex] = combined[newDividingIndex1].second;

        int numOfKeysA = 0, numOfKeysB = 0, numOfKeysC = 0;
        for(int i = 0; i < combinedSize; i++){
            if(i < newDividingIndex1){
                left->getKeys()[i] = combined[i].first;
                left->getData()[i] = combined[i].second;
                left->getChildren()[i] = combChildren[i];
                left->getChildren()[i+1] = combChildren[i+1];
                if(!left->isLeaf()){
                    combChildren[i]->setParent(left);
                    combChildren[i+1]->setParent(left);
                }
                numOfKeysA++;
            }
            else if (i > newDividingIndex1 && i < newDividingIndex2){
                int j = i - newDividingIndex1 - 1;
                right->getKeys()[j] = combined[i].first;
                right->getData()[j] = combined[i].second;
                right->getChildren()[j] = combChildren[i];
                right->getChildren()[j+1] = combChildren[i+1];
                if(!right->isLeaf()){
                    combChildren[i]->setParent(right);
                    combChildren[i+1]->setParent(right);
                }
                numOfKeysB++;
            }
            else if(i > newDividingIndex2){
                int j = i - newDividingIndex2 - 1;
                newNode->getKeys()[j] = combined[i].first;
                newNode->getData()[j] = combined[i].second;
                newNode->getChildren()[j] = combChildren[i];
                newNode->getChildren()[j+1] = combChildren[i+1];
                if(!newNode->isLeaf()){
                    combChildren[i]->setParent(newNode);
                    combChildren[i+1]->setParent(newNode);
                }
                numOfKeysC++;
            }
        }
        left->setN(numOfKeysA);
        right->setN(numOfKeysB);
        newNode->setN(numOfKeysC);

        if(!parent->isFull()){
            newNode->setParent(parent);
            newNode->setLevel(left->getLevel());
            int insertInd = lowerBound(combKeys[newDividingIndex2], parent->getKeys(), parent->getN());
            insertInNode(parent, combKeys[newDividingIndex2], combined[newDividingIndex2].second, insertInd);

            int newNodePointerIndex = insertInd + 1;
            vector<GenericBStarNode *>& parentsChildren = parent->getChildren();

            parentsChildren.insert(parentsChildren.begin() + newNodePointerIndex, newNode);
            return false;
        }
        else{

            //BStarNode p is full, so we try to overflow keys from brother node
            bool overflowDone = overflow(parent, combKeys[newDividingIndex2], combined[newDividingIndex2].second, newNode);
            if(overflowDone){
                //newNode->setParent(parent);
                newNode->setLevel(left->getLevel());
                return false;
            }
            else{
                propagationNode = newNode;
                key = combKeys[newDividingIndex2];
                dataAdr = combined[newDividingIndex2].second;
                return true;
            }

        }


    }
    bool splitRightBrother(GenericBStarNode* p, GenericBStarNode* right, ID& key, T*& dataAdr, int indexOfP, GenericBStarNode*& newNode){
        return splitOperation(p, right, key, dataAdr, indexOfP, newNode);
    }
    bool splitLeftBrother(GenericBStarNode* p, GenericBStarNode* left, ID& key, T*& dataAdr, int indexOfP, GenericBStarNode*& newNode){
        return splitOperation(left, p, key, dataAdr, indexOfP, newNode);
    }

    bool splitNonRoot(GenericBStarNode* p, ID& key, T*& dataAdr, GenericBStarNode*& newNode = nullptr){
        GenericBStarNode* parent = p->getParent();
        //num of keys in p
        int n = p->getN();
        //num of keys in parent of p
        int parentN = parent->getN();
        vector<GenericBStarNode*>& siblings = parent->getChildren();
        //find first key that is greater than p's last (greatest) in parent keys array, and its index is the index of pointer to p node
        int indexOfP = lowerBound(p->getKeys()[n-1], parent->getKeys(), parentN);

        //right overflow is valid if p has a right brother and it is not full
        bool rightSplitValid = indexOfP != parentN;
        if(rightSplitValid){
            GenericBStarNode* rightBrother = siblings[indexOfP+1];
            return splitRightBrother(p, rightBrother, key, dataAdr, indexOfP, newNode);

        }
        else {
            bool leftSplitValid = indexOfP != 0;
            if (leftSplitValid) {
                GenericBStarNode *leftBrother = siblings[indexOfP-1];
                return splitLeftBrother(p, leftBrother, key, dataAdr, indexOfP - 1, newNode);

            }

        }

    }
    void propagatingSplit(GenericBStarNode* p, ID key, T* dataAdr){
        bool propagate = false;
        GenericBStarNode* newNode = nullptr;
        while(true){
            if(p->getIsRoot()){
                splitRoot(p, key, dataAdr, newNode);
                return;
            }
            else{
                //mora vratiti nove key i dataAdr koji se propagiraju na gore, mosta i jos nesto, pa je najlakse da uzme postojece kao reference
                //split node p in order to insert key that points to dataAdr
                propagate = splitNonRoot(p, key, dataAdr, newNode);
                if(!propagate){
                    return;
                }
                p = p->getParent();
            }
        }
    }
    void overflowOperation(GenericBStarNode* left, GenericBStarNode* right, ID key, T* dataAdr, int dividingKeyIndex, GenericBStarNode* newNode = nullptr){

        int combinedSize = left->getN() + right->getN() + 2;
        GenericBStarNode* parent = left->getParent();

        //each node has one subtree more than keys, and I am adding one more to point to newly created node on lower level (by splitting 2 to 3)
        vector<GenericBStarNode *> combChildren(combinedSize + 1);
        vector<ID> combKeys(combinedSize);
        vector<T*> combData(combinedSize);
        vector<pair<ID,T*>> combined(combinedSize);

        combineTwoNodes(left, right, combChildren, combKeys, combData, combined, key, dividingKeyIndex, dataAdr);

        if(!left->isLeaf()){
            int ind = lowerBound(newNode->getKeys()[newNode->getN()-1],combKeys, combinedSize);
            combChildren.insert(combChildren.begin() + ind, newNode);
        }

        int newDividingIndex = (combinedSize % 2 == 0) ? combinedSize/2 - 1: combinedSize/2;
        //set new dividing key and address
        parent->getKeys()[dividingKeyIndex] = combined[newDividingIndex].first;
        parent->getData()[dividingKeyIndex] = combined[newDividingIndex].second;

        int newNumOfKeysLeft = 0, newNumOfKeysRight = 0;
        //set new children, keys and data
        for(int i = 0; i < combinedSize; i++){
            if(i < newDividingIndex){
                left->getKeys()[i] = combined[i].first;
                left->getData()[i] = combined[i].second;
                left->getChildren()[i] = combChildren[i];
                left->getChildren()[i+1] = combChildren[i+1];
                if(!left->isLeaf()){
                    combChildren[i]->setParent(left);
                    combChildren[i+1]->setParent(left);
                }
                newNumOfKeysLeft++;
            }
            else if (i > newDividingIndex){
                int j = i - newDividingIndex - 1;
                right->getKeys()[j] = combined[i].first;
                right->getData()[j] = combined[i].second;
                right->getChildren()[j] = combChildren[i];
                right->getChildren()[j+1] = combChildren[i+1];
                if(!right->isLeaf()) {
                    combChildren[i]->setParent(right);
                    combChildren[i+1]->setParent(right);
                }
                newNumOfKeysRight++;
            }
        }

        left->setN(newNumOfKeysLeft);
        right->setN(newNumOfKeysRight);
    }
    void overflowRightBrother(GenericBStarNode* p, GenericBStarNode* right, ID key, T* dataAdr, int indexOfP, GenericBStarNode* newNode = nullptr){
        overflowOperation(p, right, key, dataAdr, indexOfP, newNode);
    }
    void overflowLeftBrother(GenericBStarNode* p, GenericBStarNode* left, ID key, T* dataAdr, int indexOfP, GenericBStarNode* newNode = nullptr){
        overflowOperation(left, p, key, dataAdr, indexOfP, newNode);
    }
    bool overflow(GenericBStarNode* p, ID key, T* dataAdr, GenericBStarNode* newNode = nullptr){
        //overflow not possible if there is no parent node
        if(p->getParent() == nullptr){
            return false;
        }

        GenericBStarNode* parent = p->getParent();
        //num of keys in p
        int n = p->getN();
        //num of keys in parent of p
        int parentN = parent->getN();
        vector<GenericBStarNode*>& siblings = parent->getChildren();
        //find first key that is greater than p's last (greatest) in parent keys array, and its index is the index of pointer to p node
        int indexOfP = lowerBound(p->getKeys()[n-1], parent->getKeys(), parentN);

        //right overflow is valid if p has a right brother and it is not full
        bool rightOverflowValid = indexOfP != parentN && !siblings[indexOfP+1]->isFull();
        if(rightOverflowValid){
            GenericBStarNode* rightBrother = siblings[indexOfP+1];
            overflowRightBrother(p, rightBrother, key, dataAdr, indexOfP, newNode);
            return true;
        }
        bool leftOverflowValid = indexOfP != 0 && !siblings[indexOfP-1]->isFull();
        if(leftOverflowValid){
            GenericBStarNode* leftBrother = siblings[indexOfP-1];
            overflowLeftBrother(p, leftBrother, key, dataAdr, indexOfP - 1, newNode);
            return true;
        }
        return false;
    }

    GenericBStarNode* getSuccessor(GenericBStarNode* p, int rightInd){
        GenericBStarNode* q = p->getChildren()[rightInd];
        while(q){
            p = q;
            q = q->getChildren()[0];
        }
        return p;

    }
    void basicRemoveKey(GenericBStarNode* node, ID key, int ind){
        vector<ID>& keys = node->getKeys();
        vector<T*>& data = node->getData();
        vector<GenericBStarNode*>& children = node->getChildren();
        children[ind] = nullptr;
        for(int i = ind; i < node->getN()-1; i++){
            swap(keys[i], keys[i+1]);
            swap(data[i], data[i+1]);
            swap(children[i], children[i+1]);
        }
        swap(children[node->getN()-1], children[node->getN()]);
        node->setN(node->getN() - 1);
    }

    void loanLeftOperation(GenericBStarNode* left, GenericBStarNode* right, int indL){
        ID loanedKey, middleKey;
        T *loanedData, *middleData;
        GenericBStarNode* parent = left->getParent();

        loanedKey = left->getKeys()[left->getN()-1];
        loanedData = left->getData()[left->getN()-1];
        vector<GenericBStarNode*>& leftChildren = left->getChildren();

        vector<ID>& parentKeys = parent->getKeys();
        vector<T*>& parentData = parent->getData();
        middleKey = parentKeys[indL];
        middleData = parentData[indL];

        vector<ID>& keys = right->getKeys();
        vector<T*>& data = right->getData();
        vector<GenericBStarNode*>& rightChildren = right->getChildren();

        keys.pop_back();
        data.pop_back();
        //from middle to right
        keys.insert(keys.begin(), middleKey);
        data.insert(data.begin(), middleData);

        //from left to middle
        parentKeys[indL] = loanedKey;
        parentData[indL] = loanedData;

        rightChildren.pop_back();
        //children from left to right, last child from left node becomes the first in right node
        rightChildren.insert(rightChildren.begin(), leftChildren[left->getN()]);

        basicRemoveKey(left, loanedKey, left->getN()-1);
        right->setN(right->getN()+1);
    }
    void loanRightOperation(GenericBStarNode* left, GenericBStarNode* right, int indL){
        ID loanedKey, middleKey;
        T *loanedData, *middleData;
        GenericBStarNode* parent = left->getParent();

        loanedKey = right->getKeys()[0];
        loanedData = right->getData()[0];
        vector<GenericBStarNode*>& rightChildren = right->getChildren();


        vector<ID>& parentKeys = parent->getKeys();
        vector<T*>& parentData = parent->getData();
        middleKey = parentKeys[indL];
        middleData = parentData[indL];

        vector<ID>& keys = left->getKeys();
        vector<T*>& data = left->getData();
        vector<GenericBStarNode*>& leftChildren = left->getChildren();


        //from middle to left
        keys.pop_back();
        data.pop_back();
        keys.insert(keys.begin() + left->getN(), middleKey);
        data.insert(data.begin() + left->getN(), middleData);

        //from right to middle
        parentKeys[indL] = loanedKey;
        parentData[indL] = loanedData;

        leftChildren.pop_back();
        //children from left to right, last child from left node becomes the first in right node
        leftChildren.insert(leftChildren.begin() + left->getN() + 1, rightChildren[0]);
        basicRemoveKey(right, loanedKey, 0);

        left->setN(left->getN()+1);
    }
    void loanFirstRight(GenericBStarNode* deleteNode, GenericBStarNode* brother, int index){
        loanRightOperation(deleteNode, brother, index);
    }
    void loanFirstLeft(GenericBStarNode* deleteNode, GenericBStarNode* brother, int index){
        loanLeftOperation(brother, deleteNode, index-1);
    }
    void loanSecondRight(GenericBStarNode* deleteNode, GenericBStarNode* brother, int index){
        GenericBStarNode* firstRightBrother = deleteNode->getParent()->getChildren()[index+1];
        loanRightOperation(deleteNode, firstRightBrother, index);
        loanRightOperation(firstRightBrother, brother, index+1);
    }
    void loanSecondLeft(GenericBStarNode* deleteNode, GenericBStarNode* brother, int index){
        GenericBStarNode* firstLeftBrother = deleteNode->getParent()->getChildren()[index-1];
        loanLeftOperation(firstLeftBrother, deleteNode, index-1);
        loanLeftOperation(brother, firstLeftBrother, index-2);
    }
    bool loan(GenericBStarNode* deleteNode, ID key, int ind){
        if(deleteNode->getIsRoot()){
            return false;
        }
        bool valid;
        int parentN, nodePos;
        GenericBStarNode* parent = deleteNode->getParent();
        vector<GenericBStarNode*>& children = parent->getChildren();
        parentN = parent->getN();
        //index of deleteNode in parent's children array
        nodePos = lowerBound(deleteNode->getKeys()[0], parent->getKeys(), parentN);


        //try loan with first right brother (node that has more than min keys can loan a key to another node)
        valid = nodePos != parentN && !children[nodePos+1]->minKeys();
        if(valid){
            basicRemoveKey(deleteNode, key, ind);
            loanFirstRight(deleteNode, children[nodePos+1], nodePos);
            return true;
        }
        //try loan with first left brother
        valid = nodePos != 0 && !children[nodePos-1]->minKeys();
        if(valid){
            basicRemoveKey(deleteNode, key, ind);
            loanFirstLeft(deleteNode, children[nodePos-1], nodePos);
            return true;
        }
        //try second right brother
        valid = nodePos < parentN-1 && !children[nodePos+2]->minKeys();
        if(valid){
            basicRemoveKey(deleteNode, key, ind);
            loanSecondRight(deleteNode, children[nodePos+2], nodePos);
            return true;
        }
        //try second left brother
        valid = nodePos > 1 && !children[nodePos-2]->minKeys();
        if(valid){
            basicRemoveKey(deleteNode, key, ind);
            loanSecondLeft(deleteNode, children[nodePos-2], nodePos);
            return true;
        }
        return false;
    }

    bool compress2To1Operation(GenericBStarNode* left, GenericBStarNode* right, GenericBStarNode*& deleteNode,ID& key, bool pseudoDeleteRoot){
        //there are min - 1 keys from deleteNode, 2*min keys from its brothers, and 2 separating keys from parent node
        int leftN, rightN, combinedSize, newNumOfKeys = 0, indL = 0;
        GenericBStarNode* parent = left->getParent();

        vector<ID>& parentKeys = parent->getKeys();
        vector<T*>& parentData = parent->getData();

        vector<ID>& leftKeys = left->getKeys();
        vector<T*>& leftData = left->getData();
        vector<GenericBStarNode*>& leftChildren = left->getChildren();

        vector<ID>& rightKeys = right->getKeys();
        vector<T*>& rightData = right->getData();
        vector<GenericBStarNode*>& rightChildren = right->getChildren();

        basicRemoveKey(deleteNode, key, lowerBound(key, deleteNode->getKeys(), deleteNode->getN()));

        leftN = left->getN();
        rightN = right->getN();
        combinedSize = leftN + rightN + 1;

        //make a vector for all keys, and all pointers
        vector<ID> combKeys(combinedSize);
        vector<T*> combData(combinedSize);
        vector<GenericBStarNode*> combChildren(combinedSize+1);

        copy(leftKeys.begin(), leftKeys.begin() + leftN, combKeys.begin());
        combKeys[leftN] = parentKeys[indL];
        copy(rightKeys.begin(), rightKeys.begin() + rightN, combKeys.begin() + combinedSize-rightN);

        copy(leftData.begin(), leftData.begin() + leftN, combData.begin());
        combData[leftN] = parentData[indL];
        copy(rightData.begin(), rightData.begin() + rightN, combData.begin() + combinedSize - rightN);

        copy(leftChildren.begin(), leftChildren.begin() + leftN+1, combChildren.begin());
        copy(rightChildren.begin(), rightChildren.begin() + rightN+1, combChildren.begin() + combinedSize - rightN);


        if(!pseudoDeleteRoot) {
            for (int i = 0; i < combinedSize; i++) {
                rightKeys[i] = combKeys[i];
                rightData[i] = combData[i];
                rightChildren[i] = combChildren[i];
                rightChildren[i + 1] = combChildren[i + 1];
                if (!right->isLeaf()) {
                    rightChildren[i]->setParent(right);
                    rightChildren[i + 1]->setParent(right);
                }
                newNumOfKeys++;
            }
            deleteNode = right;
            right->setN(newNumOfKeys);

            if (!parent->minKeys()) {
                basicRemoveKey(parent, parentKeys[indL], indL);
                return false;
            } else {
                bool loanDone = loan(parent, parentKeys[indL], indL);
                if (loanDone) {
                    return false;
                } else {
                    key = parentKeys[indL];
                    return true;
                }
            }
        }
        else{

            GenericBStarNode *start = this->root;
            vector<ID>& rootKeys = start->getKeys();
            vector<T*>& rootData = start->getData();
            vector<GenericBStarNode*>& rootChildren = start->getChildren();
            for (int i = 0; i < combinedSize; i++) {
                rootKeys[i] = combKeys[i];
                rootData[i] = combData[i];
                rootChildren[i] = combChildren[i];
                rootChildren[i + 1] = combChildren[i + 1];
                if (!root->isLeaf()) {
                    rootChildren[i]->setParent(start);
                    rootChildren[i + 1]->setParent(start);
                }
                newNumOfKeys++;
            }
            root->setN(newNumOfKeys);
            return false;
        }
    }
    bool compress3To2Operation(GenericBStarNode* left, GenericBStarNode* mid, GenericBStarNode* right, GenericBStarNode*& deleteNode ,ID& key, int indL){
        //there are min - 1 keys from deleteNode, 2*min keys from its brothers, and 2 separating keys from parent node
        int leftN, midN, rightN, combinedSize, newNumOfKeysMid = 0, newNumOfKeysRight = 0;
        GenericBStarNode* parent = left->getParent();

        vector<ID>& parentKeys = parent->getKeys();
        vector<T*>& parentData = parent->getData();

        vector<ID>& leftKeys = left->getKeys();
        vector<T*>& leftData = left->getData();
        vector<GenericBStarNode*>& leftChildren = left->getChildren();

        vector<ID>& midKeys = mid->getKeys();
        vector<T*>& midData = mid->getData();
        vector<GenericBStarNode*>& midChildren = mid->getChildren();

        vector<ID>& rightKeys = right->getKeys();
        vector<T*>& rightData = right->getData();
        vector<GenericBStarNode*>& rightChildren = right->getChildren();

        basicRemoveKey(deleteNode, key, lowerBound(key, deleteNode->getKeys(), deleteNode->getN()));

        leftN = left->getN();
        midN = mid->getN();
        rightN = right->getN();
        combinedSize = leftN + midN + rightN + 2;

        //make a vector for all keys, and all pointers
        vector<ID> combKeys(combinedSize);
        vector<T*> combData(combinedSize);
        vector<GenericBStarNode*> combChildren(combinedSize+1);

        copy(leftKeys.begin(), leftKeys.begin() + leftN, combKeys.begin());
        combKeys[leftN] = parentKeys[indL];
        copy(midKeys.begin(), midKeys.begin() + midN, combKeys.begin() + leftN+1);
        combKeys[leftN+1+midN] = parentKeys[indL+1];
        copy(rightKeys.begin(), rightKeys.begin() + rightN, combKeys.begin() + combinedSize-rightN);

        copy(leftData.begin(), leftData.begin() + leftN, combData.begin());
        combData[leftN] = parentData[indL];
        copy(midData.begin(), midData.begin() + midN, combData.begin() + leftN+1);
        combData[leftN+1+midN] = parentData[indL+1];
        copy(rightData.begin(), rightData.begin() + rightN, combData.begin() + combinedSize - rightN);

        copy(leftChildren.begin(), leftChildren.begin() + leftN+1, combChildren.begin());
        copy(midChildren.begin(), midChildren.begin() + midN+1, combChildren.begin() + leftN+1);
        copy(rightChildren.begin(), rightChildren.begin() + rightN+1, combChildren.begin() + combinedSize - rightN);



        int divisionIndex = (combinedSize % 2 == 0) ? combinedSize/2 - 1: combinedSize / 2;
        for(int i = 0; i < combinedSize; i++){
            if(i < divisionIndex){
                midKeys[i] = combKeys[i];
                midData[i] = combData[i];
                midChildren[i] = combChildren[i];
                midChildren[i+1] = combChildren[i+1];
                if(!mid->isLeaf()){
                    midChildren[i]->setParent(mid) ;
                    midChildren[i+1]->setParent(mid);
                }
                newNumOfKeysMid++;
            }
            else if(i > divisionIndex){
                int j = i - divisionIndex - 1;
                rightKeys[j] = combKeys[i];
                rightData[j] = combData[i];
                rightChildren[j] = combChildren[i];
                rightChildren[j+1] = combChildren[i+1];
                if(!right->isLeaf()){
                    rightChildren[j]->setParent(right);
                    rightChildren[j+1]->setParent(right);
                }
                newNumOfKeysRight++;
            }
            else{
                parentKeys[indL+1] = combKeys[i];
                parentData[indL+1] = combData[i];
            }
        }
        mid->setN(newNumOfKeysMid);
        right->setN(newNumOfKeysRight);
        deleteNode = mid;
        delete left;
        if(!parent->minKeys()){
            basicRemoveKey(parent, parentKeys[indL], indL);
            return false;
        }
        else{
            bool loanDone = loan(parent, parentKeys[indL], indL);
            if(loanDone){
                return false;
            }
            else{
                key = parentKeys[indL];
                return true;
            }
        }
    }
    bool compress(GenericBStarNode*& deleteNode, ID& key, bool pseudoDeleteRoot){
        //we want to do compression of 3 nodes into 2 if possible, so if it is, we have to find the two brother nodes of delete node, and if it is not possible we will do 2 nodes into 1 compression
        int parentN, nodePos, n;
        GenericBStarNode *parent, *brother1, *brother2;
        parent = deleteNode->getParent();
        vector<ID>& keys = deleteNode->getKeys();
        vector<GenericBStarNode*>& siblings = parent->getChildren();
        parentN = parent->getN();
        nodePos = lowerBound(keys[0], parent->getKeys(),parentN);
        n = deleteNode->getN();
        if(parentN >= 2){
            //does our delete node have both left and right brother, so it can be middle in compression
            if(nodePos == 0){
                brother1 = siblings[nodePos+1];
                brother2 = siblings[nodePos+2];
                return compress3To2Operation(deleteNode, brother1, brother2, deleteNode,key, nodePos);
            }
            else if(nodePos == parentN){
                brother1 = siblings[nodePos-1];
                brother2 = siblings[nodePos-2];
                return compress3To2Operation(brother2, brother1, deleteNode, deleteNode,key, nodePos-2);
            }
            else{
                brother1 = siblings[nodePos+1];
                brother2 = siblings[nodePos-1];
                return compress3To2Operation(brother2, deleteNode, brother1, deleteNode,key, nodePos-1);
            }

        }
        else{
            //our node has only one sibling/brother node, and they both have minimum keys so we make one out of two nodes
            if(nodePos == 0){
                brother1 = siblings[1];
                return compress2To1Operation(deleteNode, brother1, deleteNode,key, pseudoDeleteRoot);
            }
            else{
                brother1 = siblings[0];
                return compress2To1Operation(brother1, deleteNode, deleteNode, key, pseudoDeleteRoot);
            }
        }
    }
    void propagatingCompression(GenericBStarNode* deleteNode, ID key){
        bool propagate = false, pseudoDeleteRoot;

        while(true){
            if(deleteNode->getParent() != nullptr) {
                pseudoDeleteRoot = deleteNode->getParent()->getIsRoot() && this->root->getN() == 1;
            }
            propagate = compress(deleteNode, key, pseudoDeleteRoot);
            if(!propagate){
                break;
            }
            deleteNode = deleteNode->getParent();
        }
    }
    void removeFromLeaf(GenericBStarNode* leaf, ID key, int ind){

        //if there is more keys than minimum in leaf just remove the key and shift higher to the left
        if(!leaf->minKeys()){
            basicRemoveKey(leaf, key, ind);
        }
        else{
            bool loanDone = loan(leaf, key, ind);
            if(!loanDone){
                propagatingCompression(leaf, key);
            }
        }
    }
public:
    BStarTree(){
        root = new GenericBStarNode(true);
    }
    void insert(ID key, T* dataAdr){
        int i;
        GenericBStarNode* p = findNode(key);
        bool keyNotFound = notFound(p, key, i);
        if(keyNotFound){

            if(!p->isFull()){
                //the key should be inserted at index i in BSNode p
                insertInNode(p, key, dataAdr, i);
            }
            else{
                //BStarNode p is full, so we try to overflow keys from brother node
                bool overflowDone = overflow(p, key, dataAdr);
                //if overFlow is not possible, then we do split
                if(!overflowDone) {
                    //BStarNode p is full and we can't do overflow, so we split p and its brother into 3 nodes and propagate 1  key upward
                    propagatingSplit(p, key, dataAdr);
                }

            }

        }
        else{
            //error tree already contains this key
            cout << "Tree already contains this key, please try something else" << endl;
        }

    }
    void remove(ID key){
        int i;
        GenericBStarNode *p = findNode(key);
        bool keyNotFound = notFound(p, key, i);
        if(keyNotFound){
            cout << "There is no node with key: " << key << " in B* tree you are working with, please try something else" << endl;
        }
        else{
            //key is found in tree at index i of node p
            if(p->isLeaf()){
                //remove key from leaf p that is at the index i
                removeFromLeaf(p, key, i);
            }
            else{
                GenericBStarNode* q = getSuccessor(p, i+1);
                p->getKeys()[i] = q->getKeys()[0];
                swap(p->getData()[i], q->getData()[0]);
                q->getKeys()[0]++;
                removeFromLeaf(q, q->getKeys()[0], 0);
            }
        }
    }
    T* findData(ID key, int* cnt = nullptr, bool log = true){
        int ind;
        GenericBStarNode* node = findNode(key, cnt);
        bool keyNotFound = notFound(node, key, ind);
        if(keyNotFound){
            cout << "\nCustomer with key " << key << " doesn't exist" << endl;
            return nullptr;
        }
        if(cnt && log) {
            cout << "\nNumber of times the system had to access the external memory to find customer by primary key: " << *cnt << endl;
        }

        return node->getData()[ind];
    }
    vector<T*> findCustomersByName(string name){
        T* x;
        vector<T*> foundData;
        int cnt = 0;
        queue<GenericBStarNode*> q;
        q.push(root);
        while(!q.empty()){
            GenericBStarNode* next = q.front();
            q.pop();
            cnt++;

            for(int i = 0; i < next->getN(); i++){
                x = next->getData()[i];
                if(name == x->getFirstName()){
                    foundData.push_back(x);
                }
            }
            for(int i = 0; i <= next->getN(); i++){
                if(next->getChildren()[i]){
                    q.push(next->getChildren()[i]);
                }
                else{
                    break;
                }
            }
        }
        cout << "\nNumber of times the system had to access the external memory to find customers by name: " << cnt << endl;
        return foundData;

    }
    vector<T*> findMultipleData(vector<ID>& keys){
        int cnt = 0;
        vector<T*> foundData;
        T* d;
        for(ID key : keys){
            d = findData(key, &cnt,false);
            if(d) {
                foundData.push_back(d);
            }
        }
        cout << "\nNumber of times the system had to access the external memory to find customers by primary keys: " << cnt << endl;
        return foundData;
    }
    void printToConsole(){
        int currLvlNodes = 1, nextLvlNodes = 0, currNodePosition = 0;
        cout << "\n-------------B* Tree--------------\n";
        int prevLevel = 0;
        queue<GenericBStarNode*> q;
        q.push(root);
        while(!q.empty()){
            GenericBStarNode* next = q.front();
            q.pop();
            currNodePosition++;
            next->printNodeToConsole();

            for(int i = 0; i <= next->getN(); i++){
                if(next->getChildren()[i]){
                    q.push(next->getChildren()[i]);
                    nextLvlNodes++;
                }
                else{

                    break;
                }
            }
            if(currNodePosition == currLvlNodes){
                cout << endl;
                currNodePosition = 0;
                currLvlNodes = nextLvlNodes;
                nextLvlNodes = 0;
            }
        }
    }


};


vector<string> split(string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
void printMainMenu(){
    cout << "\n------------MAIN MENU-------------\n";
    cout << "1. Print index to console" << endl;
    cout << "2. Add new customer to the table" << endl;
    cout << "3. Delete customer from the table" << endl;
    cout << "4. Get customer by primary key" << endl;
    cout << "5. Get customers by name" << endl;
    cout << "6. Get k customers by primary keys" << endl;
    cout << "7. Close the program" << endl;
}
bool valid(int& input, int low, int high){
    if(input >= low && input <= high){
        return true;
    }
    cout << "Wrong input, please try again" << endl;
    return false;
}
int getInput(int low, int high){
    int input;
    do{
        cout << "Choice: ";
        cin >> input;
    }while(!valid(input, low, high));
    return input;
}
Customer* createCustomerFromConsole(){
    string fName, lName, email;
    ID _id;
    AID _addressId;
    cout << endl;
    cout << "Customer's ID: ";
    cin >> _id;
    cout << "Customer's first name: ";
    cin >> fName;
    cout << "Customer's last name: ";
    cin >> lName;
    cout << "Customer's email: ";
    cin >> email;
    cout << "Customer's address ID: ";
    cin >> _addressId;

    Customer* newCustomer = new Customer(_id, fName, lName, email, _addressId);
    return newCustomer;
}
ID getCustomerKey(){
    ID key;
    cout << "Enter the key of the customer: ";
    cin >> key;
    return key;
}
string getName(){
    string name;
    cout << "Enter the name of the customer: ";
    cin >> name;
    return name;
}
void writeCustomersToFile(string location, vector<Customer*>& customers){
    ofstream outFile(location);
    if (!outFile) {
        cerr << "Error opening file!" << endl;
    }
    for(auto c : customers){
        outFile << c->getId() << "|";
        outFile << c->getFirstName() << "|";
        outFile << c->getLastName() << "|";
        outFile << c->getEmail() << "|";
        outFile << c->getAddressId() << "\n";
    }
    outFile.close();

}
vector<ID> getKeys(){
    int n;
    ID key;
    vector<ID> keys;
    cout << "Choose the number of keys you will enter: ";
    cin >> n;
    for(int i = 0; i < n; i++){
        cin >> key;
        keys.push_back(key);
    }
    return keys;
}
int main() {
    Customer* newCustomer, *foundCustomer;
    string line, name;
    ID deleteKey, findKey;
    int input, counter;
    //this is the order of the B* tree and can be specified based on needs
    const int m = 4;
    bool end = false;
    auto index = new BStarTree<Customer, m>;
    vector<Customer*> customers;
    vector<ID> keys;
    ifstream inputFile("../materials/customer20.txt");
    if (!inputFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    while(getline(inputFile, line)){
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); // Remove carriage return
        }

        vector<string> fields = split(line, '|');
        if(fields.size() == 5){
            newCustomer = new Customer(stoll(fields[0]), fields[1], fields[2], fields[3], stoll(fields[4]));
            index->insert(newCustomer->getId(), newCustomer);
        }
    }
    while(!end){
        printMainMenu();
        input = getInput(1, 7);
        switch(input){
            case 1:
                index->printToConsole();
                break;
            case 2:
                newCustomer = createCustomerFromConsole();
                index->insert(newCustomer->getId(), newCustomer);
                break;
            case 3:
                deleteKey = getCustomerKey();
                index->remove(deleteKey);
                break;
            case 4:
                counter = 0;
                findKey = getCustomerKey();
                foundCustomer = index->findData(findKey, &counter);
                if(foundCustomer) {
                    foundCustomer->printToConsole();
                }
                break;
            case 5:
                name = getName();
                customers = index->findCustomersByName(name);
                for(auto c: customers){
                    c->printToConsole();
                    cout << endl;
                }
                customers.clear();
                break;
            case 6:
                keys = getKeys();
                customers = index->findMultipleData(keys);
                writeCustomersToFile("foundCustomers.txt", customers);
                customers.clear();
                break;
            case 7:

                index = nullptr;
                end = true;
                break;
            default:
                break;
        }
    }

    inputFile.close();
    return 0;
}

