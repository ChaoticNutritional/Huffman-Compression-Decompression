#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>
#include <bitset>
#include <stack>
#include <string>

struct HuffmanNode {
    char data; // The character in the node
    int frequency; // frequency of the character
    HuffmanNode* left; 
    HuffmanNode* right;
    int childCount = 0; // used for prioritization of 
    std::string huffCode = "";
    HuffmanNode(char data, int frequency, HuffmanNode* left = nullptr, HuffmanNode* right = nullptr, int children = 0) : data(data), frequency(frequency), left(left), right(right), childCount(children) {}
};

// comparator object specific for nodes (thanks functor!)
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

// Build tree using a priority queue
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
        if (ch != 0)
        {
            freqMap[ch]++;
        }
    }

    return freqMap;
}

std::string stringifyFile(std::ifstream &input)
{
    std::string fileContents;
    if (input) {

        // adjusting the size of the string to return to fit all file contents
        input.seekg(0, std::ios::end);
        fileContents.resize(input.tellg());
        input.seekg(0, std::ios::beg);

        char c;

        while (!input.eof())
        {
            input.get(c);
            if (c == '\0')
            {
                //std::cout << "nulltermchar!!!! >:(" << std::endl;
            }
            else {
                fileContents += c;
            }
        }
        input.close();
    }
    else {
        //std::cout << "No file found" << std::endl;
    }
    return fileContents;
}

void getHuffCodes(HuffmanNode* &currNodePtr, std::map<char, std::string> &pathAsBits, std::string bitsTraversed)
{
    // if node has no children / Leaf node found
    if (currNodePtr->left == nullptr && currNodePtr->right == nullptr)
    {
        // store new bit value in map for char in node
        pathAsBits[currNodePtr->data] = bitsTraversed;
        currNodePtr->huffCode = bitsTraversed;

        return;
    }

    if (currNodePtr->left != nullptr)
    {
        // take left path (more frequently occurring char)
        getHuffCodes(currNodePtr->left, pathAsBits, bitsTraversed + "0");
    }

    if (currNodePtr->right != nullptr)
    {
        // take right path (less frequently occurring char)
        getHuffCodes(currNodePtr->right, pathAsBits, bitsTraversed + "1");
    }
}

int totalNodes(HuffmanNode* root)
{
    if (root == NULL)
        return 0;

    int l = totalNodes(root->left);
    int r = totalNodes(root->right);

    return 1 + l + r;
}

void DFSpostOrder(HuffmanNode* nodePtr, std::ofstream &output)
{
    // the first thing passed here will be the root
    if (nodePtr == nullptr)
    {
        return;
    }
    DFSpostOrder(nodePtr->left, output);
    DFSpostOrder(nodePtr->right, output);
    output << nodePtr->data;
}

void huffDecompress(const char* fileName)
{
    //  Parameters: the encoded file
    //
    //  local variables :
    // CHECK -std::map<char, char*> -holds the real character ascii per unique charand its huffcode value
    // CHECK -char bitBuffer - holds the bits we've collected so far, while also being used as a key for the map
    //  -int uniqueChars - counter for number of unique characters in order to build new table
    std::map<char, char*> pathAsBits;
    char bitBuffer = 0;
    int uniqueChars = 0;

    // rebuilds the tree using a stack this time
    std::ifstream myFile2(fileName, std::ios::binary);
    if (myFile2)
    {
        std::map<char, std::string> pathAsBits;
        std::stack<HuffmanNode> stack;

        // read the first data in the file which will be an integer storing the total number of nodes;
        int numNodes = 0;
        myFile2 >> numNodes;
        int i = 0;

        // ptr to point to the root of the tree
        HuffmanNode* ptr;

        // holds the next char read from the file
        myFile2.read(&bitBuffer, sizeof(char));

        // temporary constant value while I figure out how to dynamically get the total number of nodes
        while (i < numNodes)
        {
            if (bitBuffer != '\0')
            {
                HuffmanNode* leaf = new HuffmanNode(bitBuffer, 0);
                stack.push(*leaf);
                myFile2.read(&bitBuffer, sizeof(char));
                i++;
            }
            else
            {
                HuffmanNode* parent = new HuffmanNode(bitBuffer, 0);

                parent->left = new HuffmanNode(stack.top());
                stack.pop();

                if (!stack.empty())
                {
                    parent->right = new HuffmanNode(stack.top());
                    stack.pop();
                }

                i++;

                stack.push(*parent);
                myFile2.read(&bitBuffer, sizeof(char)); // seems like on the last run of this, it will hold the bit we want to use later
            }
        }

        // ---NOTE---
        // Traversal results with backwards huffcodes likely due to different data structures but will test more
        ptr = &stack.top();

        // generate huff codes for the newly constructed tree
        getHuffCodes(ptr, pathAsBits, "");
        
        // RESTRUCTURE...
        // temporary so that I can fix the inverted binary for the huffman codes
        for (auto& elem : pathAsBits)
        {
            for (char& indivChar : elem.second)
            {
                indivChar = (indivChar == '0') ? '1' : '0';
            }
        }

        /// WRITE TO A NEW FILE!
        std::ofstream decodedFile("Decoded.txt", std::ios::out); 

        char bitMask = 0;

        int loopctr = 0;
        int bitsSoFar = 0;
        char bitsInByte[8];
        
        while (!myFile2.eof())
        {
            if (ptr->left == nullptr && ptr->right == nullptr)
            {
                decodedFile << ptr->data;
                //std::cout << ptr->data;
                ptr = &stack.top();
            }
            bitMask = (bitBuffer >> 7 - bitsSoFar) & 1;
            bitsInByte[bitsSoFar] = bitMask;
            // leaf node

            if (bitMask == 0)
            {
                ptr = ptr->right;
            }
            else if (bitMask == 1)
            {
                ptr = ptr->left;
            }

            if (++bitsSoFar >= 8)
            {
                myFile2.read(&bitBuffer, sizeof(char));
                bitsSoFar = 0;
            }

            loopctr++;
        }

        decodedFile.close();
        myFile2.close();

        //bitMask = bitMask + std::to_string((bitBuffer >> i) & 1);



        //// iterate over bits in the bitbuffer which hold the next byte of information
        //for (int i = 7; i >= 0; i--)
        //{
        //    bitMask = bitMask + std::to_string((bitBuffer >> i) & 1);
        //    for (auto elem : pathAsBits)
        //    {
        //        if (elem.second.compare(bitMask))
        //        {
        //            decodedFile << elem.first;
        //            i -= bitMask.length();
        //            bitBuffer <<= bitMask.length();
        //        }
        //    }
        //}

        // load the first full 8 bits into a variable

    }
        /*
        * Parameters: the encoded file
        *
        * local variables:
        * - map of char int - used to build the priority queue
        * - priority queue - used to rebuild the tree
        * - HuffmanNode = buildTree call on priority queue
        * - map of char, string to hold the huffman codes 
        * 
        * 
        * 
        * GOALS:
        *
        * read the head of the file, which will contain:
        * - the ascii value of a character in the string
        * - the frequency in which it occurs
        * - rebuild the tree using this data as well as several existing functions
        * 
        * iterate through the compressed file bit by bit
        *
        * write to a new file the decompressed associations of the file
        * - a new ofstream("Decompressed.txt")
        * - write each letter as it occurs from the input fil to the output file by reading bit by bit
        * - when we reach eof of input file, stop and close both files
        *
        */

    else
    {
        //std::cout << "not found";
    }
}

int main()
{
    std::string fileContents;
    std::ifstream inFile("Text.txt", std::ios::in);
    fileContents = stringifyFile(inFile);
    std::priority_queue<HuffmanNode, std::vector<HuffmanNode>, CompareHuffmanNodes> pq;
    std::map<char, int> freqMap;
    std::map<char, std::string> pathAsBits;

    freqMap = CalculateFrequencies(fileContents);
    pq = makePQ(freqMap);
    
    HuffmanNode root = buildTree(pq);

    // total number of characters
    //////std::cout << root.frequency << std::endl;

    HuffmanNode* ptr = &root;

    getHuffCodes(ptr, pathAsBits, "");

// OUTPUT FILE TESTING (IN PROGRESS)
    //ENCODE
    std::ofstream encodedFile("TestOut.bin", std::ios::out | std::ios::binary);
    if (encodedFile)
    {
        // writes the tree information to the encoded file
            // total num of nodes
        encodedFile << totalNodes(ptr);
            // each node's data written through post order traversal
        DFSpostOrder(ptr, encodedFile);
        
        // bitsSoFar can be zero because all of the data written by DFSpostOrder are in bytes
        int bitsSoFar = 0;
        unsigned char byte = 0;


        // for each character in the file contents
        for (char indivchar : fileContents)
        {

            // find the huffman code for that char
            // add each character of that huffman code to a new variable 'byte' untilit is 8 bits long
            for (char bitFromHuffcode : pathAsBits[indivchar])
            {

                // shift byte left by 1 creating an opening for the bit from thehuffmancode of current char
                byte <<= 1;

                // bitwise OR toggles bits in byte based on the operand(bitFromHuffcode)
                    /*
                    * the "- '0' " is done because the bitFromHuffcode is a characterwithin a string
                    * since characters are represented as ASCII values, the bit willeither be 48 or 49
                    * (because we know it is either '0' or '1'.
                    * 0 = 110000 = 48
                    * 1 = 110001 = 48
                    * subtracting either will result in:
                    * 110000 - 110000 = 000000
                    *        OR
                    * 110001 - 110000 = 000001
                    * allowing for a seamless bitwise OR operation
                    */
                byte |= bitFromHuffcode - '0';

                if (++bitsSoFar == 8)
                {
                    // output the byte as data to the file
                    encodedFile << byte;

                    // reset the bit counter and byte's data now that it as beenwritten
                    bitsSoFar = 0;
                    byte = 0;
                }
            }
        }
        // once out of the loop of chars in the fileContents string, check for paddin needs
        if (bitsSoFar > 0)
        {
            // shifts the values in byte a number of bits equal to the difference in  full bit and the bits accumulated
            byte <<= (8 - bitsSoFar);

            // write the byte with any necessary padding to the file
            encodedFile << byte;
        }

        // close the file
        encodedFile.close();
    }
    else {
        //std::cout << "No file found" << std::endl;
    }

    //// debugging output the character and its huffman code for each unique encoded character
    for (auto elem : pathAsBits)
    {
        //std::cout << elem.first << "  :  " << elem.second << std::endl;
    }

    huffDecompress("TestOut.bin");
    return 0;
}