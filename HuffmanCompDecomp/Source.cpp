#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>
#include <bitset>
#include <stack>

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

void getHuffCodes(HuffmanNode* &currNodePtr, std::map<char, std::string> &compressedBitsMap, std::string pathAsBits)
{
    // if node has no children / Leaf node found
    if (currNodePtr->left == nullptr && currNodePtr->right == nullptr)
    {
        // store new bit value in map for char in node
        compressedBitsMap[currNodePtr->data] = pathAsBits;
        currNodePtr->huffCode = pathAsBits;
        // debugging
        //std::cout << compressedBitsMap[currNodePtr->data] << "  :  " << currNodePtr->data << std::endl;

        return;
    }

    if (currNodePtr->left != nullptr)
    {
        // take left path (more frequently occurring char)
        getHuffCodes(currNodePtr->left, compressedBitsMap, pathAsBits + "0");
    }

    if (currNodePtr->right != nullptr)
    {
        // take right path (less frequently occurring char)
        getHuffCodes(currNodePtr->right, compressedBitsMap, pathAsBits + "1");
    }
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
    //  -std::map<char, char*> -holds the real character ascii per unique charand its huffcode value
    //  -char bitBuffer - holds the bits we've collected so far, while also being used as a key for the map
    //  -int uniqueChars - counter for number of unique characters in order to build new table
    std::map<char, char*> decodeMap;
    char bitBuffer = 0;
    int uniqueChars = 0;

    // decompression testing
    std::ifstream myFile2(fileName, std::ios::binary);
    if (myFile2) {
        
        std::map<char, std::string> pathAsBits;
        std::stack<HuffmanNode> stack;

        int i = 0;
        HuffmanNode* ptr;
        char nextChar = 0;
        myFile2.read(&nextChar, sizeof(char));
        HuffmanNode* test = new HuffmanNode(nextChar, 0);
        stack.push(*test);
        myFile2.read(&nextChar, sizeof(char));

        while (i < 14)
        {
            if (nextChar != '\0')
            {
                HuffmanNode* leaf = new HuffmanNode(nextChar, 0);
                stack.push(*leaf);
                myFile2.read(&nextChar, sizeof(char));
                i++;
            }
            else
            {
                HuffmanNode* parent = new HuffmanNode(nextChar, 0);

                parent->left = new HuffmanNode(stack.top());
                stack.pop();
                parent->right = new HuffmanNode(stack.top());
                stack.pop();

                i++;

                stack.push(*parent);
                myFile2.read(&nextChar, sizeof(char));
            }
        }
        // Traversal results with backwards huffcodes likely due to different data structures but will test more
        ptr = &stack.top();

            /*char nextChar = 0;
            myFile2.read(&nextChar, sizeof(char));
            HuffmanNode* left = new HuffmanNode(nextChar, 0);
            HuffmanNode* right;
            if (myFile2.read(&nextChar, sizeof(char) == '\0'))
            {
                right = nullptr;
                HuffmanNode* parent = new HuffmanNode(nextChar, 0);
                parent->left = left;
                parent->right = right;
            }
            else {
                right = new HuffmanNode(nextChar, 0);
                myFile2.read(&nextChar, sizeof(char));
                HuffmanNode* parent = new HuffmanNode(nextChar, 0);
                parent->left = left;
                parent->right = right;
            }*/
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


        //std::vector<HuffmanNode> rebuildTree;

        //int bitsSoFar = 0;
        //char nextChar;
        //while (myFile2.get(nextChar) && nextChar != '\0')
        //{
        //    // new huffnode to add to the rebuildTree vector when we're done remaking it
        //    HuffmanNode* newNode = new HuffmanNode('\0', 0);

        //    char test = 0;
        //    for (int i = 0; i < 8; i++)
        //    {
        //        test <<= 1;
        //    }

        //    // the real ASCII character
        //    newNode->data = nextChar;

        //    // gets the number of bits we will need to examine for each char's huffcode
        //    char codeLen = 0;

        //    // placeholder for node's huffcode to be entered
        //    std::string tempHuffCode = "";

        //    myFile2.get(codeLen);
        //    for (int i = 0; i < static_cast<int>(codeLen); i++)
        //    {
        //        // read the encoded bile bit by bit and add the bits to the placeholder string
        //        char bit;
        //        myFile2.get(bit);
        //        tempHuffCode += bit;
        //       // until we reach the length of this data's huffCode
        //    }

        //    rebuildTree.push_back(*newNode);
        //}

        // finish current byte if necessary



        //std::string testout;

        //int numUniqueChars = 0;

        //myFile2.read(reinterpret_cast<char*>(&numUniqueChars),1);
        ////std::cout << numUniqueChars;
        ////std::cout << std::endl;

        //myFile2.seekg(0, std::ios::end);
        //testout.resize(myFile2.tellg());
        //myFile2.seekg(4, std::ios::beg);

        //// starting after the 4th byte:
        //int counter = 0;
        ////while (counter <= numUniqueChars)
        ////{
        ////    //
        ////}
        //

        ////std::cout << "HERE2" << std::endl;
        //myFile2.read(reinterpret_cast<char*>(&testout[0]), testout.size());
        ////std::bitset<8> t(bitBuffer);
        ////for (int i = 0; i < 8; i++)
        ////{
        ////    std::cout << t[i] << std::endl;
        ////}

        ////std::cout << "HERE3" << std::endl;
        //std::cout << testout << std::endl;

        //unsigned char bit = 0;
        //for (int i = 0; i < testout.length(); i++)
        //{
        //    //std::cout << reinterpret_cast<char*>() << std::endl;
        //    bit |= testout[i] - '0';
        //    //std::cout << testout[i] << "  :  " << bit; 
        //    bit = testout[i] >>= 1;
        //    //std::cout <<reinterpret_cast<char *>(&bit) << std::endl;
        //}


    else
    {
        std::cout << "not found";
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

    getHuffCodes(ptr, pathAsBits, "");

// OUTPUT FILE TESTING (IN PROGRESS)
    //ENCODE
    std::ofstream myFile("TestOut.bin", std::ios::out | std::ios::binary);
    if (myFile)
    {
        //// WRITING TREE DATA
        ////std::vector<char> buffer;
        ////for (auto& entry : pathAsBits)
        ////{
        ////    buffer.push_back(entry.first);

        ////    buffer.push_back(static_cast<char>(entry.second.length()));
        ////    for (char bit : entry.second)
        ////    {
        ////        buffer.push_back(bit);
        ////    }
        ////}
        ////buffer.push_back('\0');

        ////int paddingAmt = (8 - buffer.size() % 8);

        ////myFile.write(buffer.data(), buffer.size());


        ////int bitsSoFar = paddingAmt;

        //// char 
        //int bitsSoFar = 0;
        //unsigned char byte = 0;

        //// output codebook info
        //for (int i = 0; i < pathAsBits.size(); i++)
        //{
        //    //output the letter
        //    while (ptr->left != nullptr && ptr->right != nullptr)
        //    {
        //        // traverse to the leaves
        //        HuffmanNode* childptr;
        //    }

        //    //output the length of its huffcode

        //    //output the huffmancode

        //    if (++bitsSoFar == 8)
        //    {
        //        // mostly for debugging to see binary output in console
        //        std::bitset<8> c(byte);

        //        // Debugging to see binary output in console
        //        std::cout << c << std::endl;

        //        // output the byte as data to the file
        //        myFile << byte;

        //        // reset the bit counter and byte's data now that it as beenwritten
        //        bitsSoFar = 0;
        //        byte = 0;
        //    }
        //}

        DFSpostOrder(ptr, myFile);
        myFile << '\0';

        // bitsSoFar can be zero because all of the data written by DFSpostOrder are in bytes
            // ADDENDUM: so is '\0' because it's sizeofChar
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
                    // mostly for debugging to see binary output in console
                    std::bitset<8> c(byte);

                    // Debugging to see binary output in console
                    std::cout << c << std::endl;

                    // output the byte as data to the file
                    myFile << byte;

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
            myFile << byte;

            // debugging for output
            std::bitset<8> c(byte);
            std::cout << c << std::endl;
        }

        // we probably need an end of file marker? does it need to be encoded?

        // close the file
        myFile.close();
    }
    else {
        std::cout << "No file found" << std::endl;
    }

    // debugging output the character and its huffman code for each unique encoded character
    for (auto elem : pathAsBits)
    {
        std::cout << elem.first << "  :  " << elem.second << std::endl;
    }

    huffDecompress("TestOut.bin");
    return 0;
}