#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>

struct HuffmanNode {
    char data; // The character in the node
    int frequency; // frequency of the character
    HuffmanNode* left; 
    HuffmanNode* right;
    int childCount = 0;
   
    HuffmanNode(char data, int frequency, HuffmanNode* left = nullptr, HuffmanNode* right = nullptr, int children = 0) : data(data), frequency(frequency), left(left), right(right), childCount(children) {}
};

// comparator object specific for nodes
struct CompareHuffmanNodes {
    bool operator()(const HuffmanNode& node1, const HuffmanNode& node2) const {
        if (node1.frequency == node2.frequency)
        {
            // prioritize nodes with children to float to the top
            return (node1.childCount < node2.childCount);
        }
        else
        return (node1.frequency > node2.frequency);
    }
};

HuffmanNode buildTree(std::priority_queue<HuffmanNode, std::vector<HuffmanNode>, CompareHuffmanNodes>& pq)
{
    if (pq.size() == 1)
    {
        // output pointer to the root of the tree
        return pq.top();
    }
    else
    {
        // new parent node
        HuffmanNode* parent = new HuffmanNode('\0', 0);

        // create new node ptr for right child based on top of heap
            // should be less frequent of next two nodes
        HuffmanNode* right = new HuffmanNode(pq.top()); 
        pq.pop();

        // create new node ptr for left child based on top of heap
        HuffmanNode* left = new HuffmanNode(pq.top());
        pq.pop();

        // assign values to parent
        parent->left = left;
        parent->right = right;
        parent->frequency = left->frequency + right->frequency;
        // notate increase in children owned by parent
        parent->childCount = left->childCount + right->childCount + 1;
        
        // push parent back into the min-heap priority queue
        pq.push(*parent);

        // recurse
        return buildTree(pq);
    }
}

// translates the gathered frequency map into a min-heap priority queue
std::priority_queue<HuffmanNode, std::vector<HuffmanNode>, CompareHuffmanNodes> makePQ(std::map<char, int> freqMap)
{
    std::priority_queue<HuffmanNode, std::vector<HuffmanNode>, CompareHuffmanNodes> pq;

    for (auto& elem : freqMap)
    {
        HuffmanNode newnode(elem.first, elem.second);
        pq.push(newnode);
    }
    return pq;
}


// assign a value to each character based on its frequency
std::map<char, int> CalculateFrequencies(const std::string& input) {
    // maps values to each character from input
    std::map<char, int> freqMap;

    for (auto ch: input) 
    {
        // increases frquency as character is found
        freqMap[ch]++;
    }

    return freqMap;
}

std::string stringifyFile(const char* fileName)
{
    std::string fileContents;
    std::ifstream myFile(fileName, std::ios::in);
    if (myFile) {

        // adjusting the size of the string to return to fit all file contents
        myFile.seekg(0, std::ios::end);
        fileContents.resize(myFile.tellg());
        myFile.seekg(0, std::ios::beg);

        myFile.read(&fileContents[0], fileContents.size());

        myFile.close();
    }
    else {
        std::cout << "No file found" << std::endl;
    }
    return fileContents;
}

void traverseForCodes(HuffmanNode* currNodePtr, std::map<char, std::string> compressedBitsMap, std::string pathAsBits)
{
    // if node has no children / Leaf node found
    if (currNodePtr->left == nullptr && currNodePtr->right == nullptr)
    {
        // store new bit value in map for char in node
        compressedBitsMap[currNodePtr->data] = pathAsBits;

        // debugging
        //std::cout << compressedBitsMap[currNodePtr->data] << "  :  " << currNodePtr->data << std::endl;

        return;
    }

    if (currNodePtr->left != nullptr)
    {
        // take left path (more frequently occurring char)
        traverseForCodes(currNodePtr->left, compressedBitsMap, pathAsBits + "0");
    }

    if (currNodePtr->right != nullptr)
    {
        // take right path (less frequently occurring char)
        traverseForCodes(currNodePtr->right, compressedBitsMap, pathAsBits + "1");
    }
}

int main()
{
    std::string fileContents = stringifyFile("Text.txt");
    std::priority_queue<HuffmanNode, std::vector<HuffmanNode>, CompareHuffmanNodes> pq;
    std::map<char, int> freqMap;
    std::map<char, std::string> pathAsBits;

    freqMap = CalculateFrequencies(fileContents);
    pq = makePQ(freqMap);

    HuffmanNode root = buildTree(pq);

    // head
    std::cout << root.frequency << std::endl;

    HuffmanNode* ptr = &root;

    traverseForCodes(ptr, pathAsBits, "");

    return 0;
}