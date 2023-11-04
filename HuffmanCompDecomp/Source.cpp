#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>
#include <bitset>

struct HuffmanNode {
    char data; // The character in the node
    int frequency; // frequency of the character
    HuffmanNode* left; 
    HuffmanNode* right;
    int childCount = 0; // used for prioritization of 
   
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

void traverseForCodes(HuffmanNode* currNodePtr, std::map<char, std::string> &compressedBitsMap, std::string pathAsBits)
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
    std::string fileContents;
    fileContents = stringifyFile("Text.txt");
    std::priority_queue<HuffmanNode, std::vector<HuffmanNode>, CompareHuffmanNodes> pq;
    std::map<char, int> freqMap;
    std::map<char, std::string> pathAsBits;

    freqMap = CalculateFrequencies(fileContents);
    pq = makePQ(freqMap);

    HuffmanNode root = buildTree(pq);

    // total number of characters
    ////std::cout << root.frequency << std::endl;

    HuffmanNode* ptr = &root;

    traverseForCodes(ptr, pathAsBits, "");

// OUTPUT FILE TESTING (IN PROGRESS)
    std::ofstream myFile("TestOut.bin", std::ios::binary);
    if (myFile)
    {
        // Write the length of the encoded data in terms of number of total characters
        myFile.write(reinterpret_cast<char*>(&root.frequency), sizeof(int));

        // Write each unique char's real ascii code to the file
        //for()





        /// ENCODING
        // counter to handle number of bits stored
        int bitsSoFar = 0;

        // char 
        unsigned char byte = 0;

        // for each character in the file contents
        for (char indivchar : fileContents)
        {

            // find the huffman code for that char
            // add each character of that huffman code to a new variable 'byte' until it is 8 bits long
            for (char bitFromHuffcode : pathAsBits[indivchar])
            {

                // shift byte left by 1 creating an opening for the bit from the huffmancode of current char
                byte <<= 1;

                // bitwise OR toggles bits in byte based on the operand (bitFromHuffcode)
                    /*
                    * the "- '0' " is done because the bitFromHuffcode is a character within a string
                    * since characters are represented as ASCII values, the bit will either be 48 or 49
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
                    // mostly for debugging to see binary output in console
                    std::bitset<8> c(byte);

                    // Debugging to see binary output in console
                    std::cout << c << std::endl;

                    // output the byte as data to the file
                    myFile << byte;

                    // reset the bit counter and byte's data now that it as been written
                    bitsSoFar = 0;
                    byte = 0;
                }
            }
        }

        // once out of the loop of chars in the fileContents string, check for padding needs
        if (bitsSoFar > 0)
        {
            // shifts the values in byte a number of bits equal to the difference in a full bit and the bits accumulated
            byte <<= (8 - bitsSoFar);

            // write the byte with any necessary padding to the file
            myFile << byte;

            // debugging for output
            std::bitset<8> c(byte);
            std::cout << c << std::endl;
        }

        // we probably need an end of file marker

        // close the file
        myFile.close();
    }
    else {
        std::cout << "No file found" << std::endl;
    }

    // debugging!!
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;



    // debugging output the character and its huffman code for each unique encoded character
    for (auto elem : pathAsBits)
    {
        std::cout << elem.first << "  :  " << elem.second << std::endl;
    }

    // decompression testing
    std::ifstream myFile2("TestOut.bin", std::ios::binary);
    if (myFile2) {

        /*
        * Parameters: the encoded file
        * 
        * local variables: 
        * - std::map<char, char *> - holds the real character ascii per unique char and its huffcode value
        * - char bitBuffer - holds the bits we've collected so far, while be used as a key for the map!
        *
        * GOALS: 
        * 
        * read the head of the file, which will contain: 
        * - true ascii value for each unique character
        * - that unique character's associated huffman code
        * - padding to 8
        * 
        * create an map with this header data of char and <char? char *> revisit....
        * 
        * write to a new file the decompressed associations of the file
        * - a new ofstream("Decompressed.txt")
        * - write each letter as it occurs from the input fil to the output file by reading bit by bit
        * - when we reach eof of input file, stop and close both files
        * 
        */

        int counter = 0;
        std::string testout;

        //std::cout << "HERE" << std::endl;
        myFile2.seekg(0, std::ios::end);
        testout.resize(myFile2.tellg());
        myFile2.seekg(0, std::ios::beg);
        //std::cout << "HERE2" << std::endl;
        myFile2.read(reinterpret_cast<char *>(&testout[0]), testout.size());
            //std::cout << "HERE3" << std::endl;
        std::cout << testout << std::endl;

        unsigned char bit = 0;
        for (int i = 0; i < testout.length(); i++)
        {
            //std::cout << reinterpret_cast<char*>() << std::endl;
            bit |= testout[i] - '0';
            std::cout << testout[i] << "  :  " << bit;
            bit = testout[i] >>= 1;
            //std::cout <<reinterpret_cast<char *>(&bit) << std::endl;
        }
        
    }
    else
    {
        std::cout << "not found";
    }
    return 0;
}