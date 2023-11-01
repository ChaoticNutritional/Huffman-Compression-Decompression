#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>

struct HuffmanNode {
    char data; // The character in the node
    int frequency; // Frequency of the character
    HuffmanNode* left;
    HuffmanNode* right;
    int childCount = 0;
   
    HuffmanNode(char data, int frequency, HuffmanNode* left = nullptr, HuffmanNode* right = nullptr, int children = 0) : data(data), frequency(frequency), left(left), right(right), childCount(children) {}
};

// comparator struct specific for nodes
struct CompareHuffmanNodes {
    bool operator()(const HuffmanNode& node1, const HuffmanNode& node2) const {
        if (node1.frequency == node2.frequency)
        {
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
        return pq.top();
    }
    else
    {
        // go to the address of pq.top
        HuffmanNode* parent = new HuffmanNode('\0', 0);

        HuffmanNode* right = new HuffmanNode(pq.top()); 

        pq.pop();


        HuffmanNode* left = new HuffmanNode(pq.top());

        parent->left = left;

        parent->right = right;

        parent->frequency = left->frequency + right->frequency;

        parent->childCount = left->childCount + right->childCount + 1;
        
        pq.pop();

        pq.push(*parent);

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
    // if(ptrToHead->data == '\0')
    if (currNodePtr->left == nullptr && currNodePtr->right == nullptr)
    {
        compressedBitsMap[currNodePtr->data] = pathAsBits;
        std::cout << compressedBitsMap[currNodePtr->data] << "  :  " << currNodePtr->data << std::endl;
        return;
    }

    if (currNodePtr->left != nullptr)
    {
        traverseForCodes(currNodePtr->left, compressedBitsMap, pathAsBits + "0");
    }

    if (currNodePtr->right != nullptr)
    {
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

    //// confirming all elements preset in freqmap
    //for (auto& elem : freqMap)
    //{
    //    //std::cout << elem.first << " : " << elem.second << std::endl;
    //}

    HuffmanNode head = buildTree(pq);

    // head
    std::cout << head.frequency << std::endl;

    //1st layer
    //std::cout << head.left->left->frequency << std::endl;
    //std::cout << head.left->right->frequency << std::endl;

    ////2nd layer
    //std::cout << head.right->left->frequency << std::endl;
    //std::cout << head.right->right->frequency << std::endl;

   // std::cout << HuffmanQueue.size();

    HuffmanNode* ptr = &head;

    traverseForCodes(ptr, pathAsBits, "");



    return 0;
}