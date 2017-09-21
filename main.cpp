//Nicolas Stoian
//This program needs 6 command line arguments
//argv[1] "input1" for Huffman text probability
//argv[2] "input2" for Huffman encode data
//argv[3] "output1" for char-prob pairs
//argv[4] "output2" for linked list, binary tree, and entropy table
//argv[5] "output3" for encoded data
//argv[6] "output4" for decoded data

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

class Node{
private:
    string ch;
    double prob;
    string code;
    Node* left;
    Node* right;
    Node* next;

public:
    Node();
    Node(string ch, double prob);
    Node(string ch, double prob, Node* left, Node* right);
    ~Node();
    string printNode();
    string getCh();
    double getProb();
    string getCode();
    Node* getRight();
    Node* getLeft();
    Node* getNext();
    void setCh(string ch);
    void setProb(double prob);
    void setCode(string code);
    void setRight(Node* right);
    void setLeft(Node* left);
    void setNext(Node* next);
};

class LinkedList{
private:
    Node* listHead;
    Node* walker;

public:
    LinkedList();
    ~LinkedList();
    string printList();
    void insertNode(Node* nodeToInsert);
    string printSortedList();
    Node* getListHead();
};

class EntropyTable{
private:
    string* chArray;
    double* probArray;
    string* codeArray;
    int* numBitsArray;
    double* entropyArray;

public:
    EntropyTable(LinkedList* workspaceList, int size);
    ~EntropyTable();
    string printEntropyTable(int size);
    string* getChArray();
    string* getCodeArray();
};

void ComputeHistogram(ifstream& inFile, ofstream& outFile);
void HuffmanLinkedList(LinkedList* workspaceList, int& numChars, ifstream& inFile, ofstream& outFile1);
Node* HuffmanBinaryTree(LinkedList* workspaceList, ofstream& outFile1);
void HuffmanCode(Node* treeRoot, string code);
void EncodeFile(EntropyTable* workspaceTable, int size, ifstream& inFile, ofstream& outFile);
void DecodeFile(EntropyTable* workspaceTable, int size, ifstream& inFile, ofstream& outFile);

int main(int argc, char* argv[]){
    ifstream inFile;
    inFile.open(argv[1]);
    ofstream outFile1;
    outFile1.open(argv[3]);
    ComputeHistogram(inFile, outFile1);
    inFile.close();
    outFile1.close();
    ifstream inFileHist;
    inFileHist.open(argv[3]);
    ofstream outFile2;
    outFile2.open(argv[4]);
    LinkedList* workspaceList = new LinkedList();
    int numChars = 0;
    HuffmanLinkedList(workspaceList, numChars, inFileHist, outFile2);
    inFileHist.close();
    outFile2 << workspaceList->printList() << "\n\n";
    Node* treeRoot = HuffmanBinaryTree(workspaceList, outFile2);
    outFile2 << workspaceList->printList() << "\n\n";
    HuffmanCode(treeRoot, "");
    EntropyTable* workspaceTable = new EntropyTable(workspaceList, numChars);
    outFile2 << workspaceTable->printEntropyTable(numChars) << "\n\n";
    outFile2.close();
    ifstream inFile2;
    inFile2.open(argv[2]);
    ofstream outFile3;
    outFile3.open(argv[5]);
    EncodeFile(workspaceTable, numChars, inFile2, outFile3);
    inFile2.close();
    outFile3.close();
    ifstream inFileDecode;
    inFileDecode.open(argv[5]);
    ofstream outFile4;
    outFile4.open(argv[6]);
    DecodeFile(workspaceTable, numChars, inFileDecode, outFile4);
    inFileDecode.close();
    outFile4.close();
    delete workspaceList;
    delete treeRoot;
    delete workspaceTable;
    return 0;
}


Node::Node(): ch("dummy"), prob(0), code(""), left(NULL), right(NULL), next(NULL){
}

Node::Node(string ch, double prob): ch(ch), prob(prob), code(""), left(NULL), right(NULL), next(NULL){
}

Node::Node(string ch, double prob, Node* left, Node* right): ch(ch), prob(prob), code(""), left(left), right(right), next(NULL){
}

Node::~Node(){
    left = NULL;
    right = NULL;
    next = NULL;
    delete left;
    delete right;
    delete next;
}

string Node::printNode(){
    stringstream toReturn;
    toReturn << "(" << ch << ", " << prob << ", ";
    if (next == NULL){
        toReturn << "NULL)-->NULL";
    }
    else{
        toReturn << next->ch << ")";
    }
    return toReturn.str();
}

string Node::getCh(){
    return ch;
}

double Node::getProb(){
    return prob;
}

string Node::getCode(){
    return code;
}

Node* Node::getRight(){
    return right;
}

Node* Node::getLeft(){
    return left;
}

Node* Node::getNext(){
    return next;
}

void Node::setCh(string ch){
    this->ch = ch;
}

void Node::setProb(double prob){
    this->prob = prob;
}

void Node::setCode(string code){
    this->code = code;
}

void Node::setRight(Node* right){
    this->right = right;
}

void Node::setLeft(Node* left){
    this->left = left;
}

void Node::setNext(Node* next){
    this->next = next;
}


LinkedList::LinkedList(){
    listHead = new Node();
    walker = listHead;
}

LinkedList::~LinkedList(){
    while (walker->getNext() != NULL){
        Node* prev = walker;
        walker = walker->getNext();
        delete prev;
    }
    delete walker;
    delete listHead;
}

string LinkedList::printList(){
    stringstream toReturn;
    toReturn << "listHead -->";
    while(walker->getNext() != NULL){
        toReturn << walker->printNode();
        toReturn << "-->";
        walker = walker->getNext();
    }
    toReturn << walker->printNode();
    walker = listHead;
    return toReturn.str();
}

void LinkedList::insertNode(Node* nodeToInsert){
    while(walker->getNext() != NULL && walker->getNext()->getProb() < nodeToInsert->getProb()){
        walker = walker->getNext();
    }
    nodeToInsert->setNext(walker->getNext());
    walker->setNext(nodeToInsert);
    walker = listHead;
}

string LinkedList::printSortedList(){
    stringstream toReturn;
    walker = walker->getNext();
    while(walker->getNext() != NULL){
        toReturn << walker->getCh() << " " << walker->getProb() << "\n";
        walker = walker->getNext();
    }
    toReturn << walker->getCh() << " " << walker->getProb() << "\n";
    walker = listHead;
    return toReturn.str();
}

Node* LinkedList::getListHead(){
		return listHead;
	}

EntropyTable::EntropyTable(LinkedList* workspaceList, int size){
    chArray = new string [size + 1];
    probArray = new double [size + 1];
    codeArray = new string [size + 1];
    numBitsArray = new int [size + 1];
    entropyArray = new double [size + 1];
    int i = 0;
    Node* walker = workspaceList->getListHead();
    walker = walker->getNext();
    while(walker->getNext() != NULL){
        if(walker->getLeft() == NULL && walker->getRight() == NULL){
            chArray[i] = walker->getCh();
            probArray[i] = walker->getProb();
            codeArray[i] = walker->getCode();
            numBitsArray[i] = walker->getCode().length();
            entropyArray[i] = walker->getProb() * walker->getCode().length();
            i++;
        }
        walker = walker->getNext();
    }
}

EntropyTable::~EntropyTable(){
    delete[] chArray;
    delete[] probArray;
    delete[] codeArray;
    delete[] numBitsArray;
    delete[] entropyArray;
}

string EntropyTable::printEntropyTable(int size){
    stringstream toReturn;
    toReturn << left << setw(8) << "Char" << setw(15) << "Prob" << setw(15) << "Code" << setw(8) << "#Bits" << setw(10) << "Entropy" << "\n";
    toReturn << "======================================================\n";
    for(int i = 0; i < size; i++){
        toReturn << left << setw(8) << chArray[i] << setw(15) << probArray[i] << setw(15) << codeArray[i] << setw(8) << numBitsArray[i] << setw(10) << entropyArray[i] << "\n";
    }
    return toReturn.str();
}

string* EntropyTable::getChArray(){
    return chArray;
}

string* EntropyTable::getCodeArray(){
    return codeArray;
}

void ComputeHistogram(ifstream& inFile, ofstream& outFile){
    int size = 256;
    int* countArray = new int [size];
    for(int i = 0; i < size; i++){
        countArray[i] = 0;
    }
    int charTotal = 0;
    unsigned char charItem;
    while(inFile >> noskipws >> charItem){
        int index = (int)charItem;
        countArray[index]++;
        charTotal++;
    }
    int index = 0;
    double totalProb = 0;
    while(index < size){
        if(countArray[index] > 0){
            double prob = ((double)countArray[index]/charTotal)*100;
            if((char)index == '\n'){
                outFile << "@" << " " << prob << endl;
                //cout << "@" << " " << prob << endl;
            }
            else if((char)index == ' '){
                outFile << "*" << " " << prob << endl;
                //cout << "*" << " " << prob << endl;
            }
            else{
                outFile << (char)index << " " << prob << endl;
                //cout << (char)index << " " << prob << endl;
            }
        }
        index++;
    }
}

void HuffmanLinkedList(LinkedList* workspaceList, int& numChars, ifstream& inFile, ofstream& outFile){
    int iterationNum = 0;
    //outFile << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
    //cout << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
    string charItem;
    while(inFile >> charItem){
        double probItem;
        inFile >> probItem;
        Node* inNode = new Node(charItem, probItem);
        workspaceList->insertNode(inNode);
        numChars++;
        //outFile << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
        //cout << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
    }
}

Node* HuffmanBinaryTree(LinkedList* workspaceList, ofstream& outFile){
    int iterationNum = 0;
    Node* oldListHead = new Node();
    oldListHead->setNext(workspaceList->getListHead()->getNext());
    //outFile << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
    //cout << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
    while (workspaceList->getListHead()->getNext()->getNext() != NULL){
        Node* newNode = new Node(workspaceList->getListHead()->getNext()->getCh() + workspaceList->getListHead()->getNext()->getNext()->getCh(),
                                 workspaceList->getListHead()->getNext()->getProb() + workspaceList->getListHead()->getNext()->getNext()->getProb(),
                                 workspaceList->getListHead()->getNext(),
                                 workspaceList->getListHead()->getNext()->getNext());
        workspaceList->getListHead()->setNext(workspaceList->getListHead()->getNext()->getNext()->getNext());
        workspaceList->insertNode(newNode);
        //outFile << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
        //cout << "Iteration " << iterationNum++ << ": " << workspaceList->printList() << "\n\n";
    }
    outFile << workspaceList->printList() << "\n\n";
    Node* root = workspaceList->getListHead()->getNext();
    workspaceList->getListHead()->setNext(oldListHead->getNext());
    return root;
}

void HuffmanCode(Node* treeRoot, string code){
    Node* walker = treeRoot;
    if(walker == NULL){
        return;
    }
    if(walker->getLeft() == NULL && walker->getRight() == NULL){
        walker->setCode(code);
        return;
    }
    HuffmanCode(walker->getLeft(), code + "0");
    HuffmanCode(walker->getRight(), code + "1");
}

void EncodeFile(EntropyTable* workspaceTable, int size, ifstream& inFile, ofstream& outFile){
    char charItem;
    string charCmp;
    string* chArray = workspaceTable->getChArray();
    string* codeArray = workspaceTable->getCodeArray();
    while(inFile >> noskipws >> charItem){
        if(charItem == '\n'){
            charCmp = "@";
        }
        else if(charItem == ' '){
            charCmp = "*";
        }
        else{
            charCmp = charItem;
        }
        for(int i = 0; i < size; i++){
            if(charCmp == chArray[i]){
                outFile << codeArray[i];
            }
        }
    }
}

void DecodeFile(EntropyTable* workspaceTable, int size, ifstream& inFile, ofstream& outFile){
    char charItem;
    string charCmp = "";
    string* chArray = workspaceTable->getChArray();
    string* codeArray = workspaceTable->getCodeArray();
    while(inFile >> charItem){
        charCmp += charItem;
        for(int i = 0; i < size; i++){
            if(charCmp == codeArray[i]){
                if(chArray[i] == "*"){
                    outFile << " ";
                }
                else if(chArray[i] == "@"){
                    outFile << "\n";
                }
                else{
                    outFile << chArray[i];
                }
                charCmp = "";
            }
        }
    }
}
