#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;

const string ENCODED_PATH = "D:/Pro Ramming/CPP/Dataset/Huffman/Encoded File.txt";
const string HUFFMAN_PATH = "D:/Pro Ramming/CPP/Dataset/Huffman/Huffman Map.txt";
const size_t chunkLength = 8;

struct node {
  char character;
  int frequency;
  node* left;
  node* right;
};

struct compare {
  public:
  bool operator()(node* a, node* b) {
    return a->frequency > b->frequency;
  }
};

static std::vector<char> ReadAllBytes(string filename)
{
    ifstream ifs(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}


// Create huffman tree or the codes for the individual letters
void generateHuffmanCodes(priority_queue<node*, vector<node*>, compare> nodes, unordered_map<int, string>* encodings) {
  vector <pair<node*, string>> traverse;
  traverse.push_back({nodes.top(), ""});

  node* currentAddress;
  string currentString;
  pair<node*, string> currentVect;

  while (!traverse.empty()) {
    currentVect = traverse.back();
    currentAddress = currentVect.first;
    currentString = currentVect.second;
    traverse.pop_back();

    if (!currentAddress->left && !currentAddress->right) {
      (*encodings)[currentAddress->character] = currentString;
    }

    if (currentAddress->left) traverse.push_back({currentAddress->left, currentString + "0"});
    if (currentAddress->right) traverse.push_back({currentAddress->right, currentString + "1"});
  }
}


// Take a string consisting of 0s & 1s, find the decimal value and find the corresponding character
// of that value
char binaryEncoding(string input) {
  int num = 0;
  int tempInt;

  for (int i=0; i<chunkLength; i++) {
    tempInt = input[i] - '0';
    num |= (1 << (chunkLength - 1 - i)) * tempInt;
  }

  return char(num);
}

int main(int argc, char** argv) {
  unordered_map<char, int> characterCount;
  priority_queue<node*, vector<node*>, compare> nodes;
  vector <char> fileContents;
  string INPUT_PATH;

  if (argc > 1) {INPUT_PATH = argv[1];}
  else {
    cerr << "No input file provided. Quitting." << endl;
    exit(1);
  }

  fileContents = ReadAllBytes(INPUT_PATH);

  for (int i=0; i<fileContents.size(); i++) {
    characterCount[fileContents[i]] += 1;
  }

  for (auto t: characterCount) {
    node* newNode = new node{t.first, t.second, nullptr, nullptr};
    nodes.push(newNode);
  }

  node* firstMin;
  node* secondMin;
  while (nodes.size() > 1) {
    firstMin = nodes.top();
    nodes.pop();
    secondMin = nodes.top();
    nodes.pop();
    node* mergedNode = new node{'\0', firstMin->frequency + secondMin->frequency, firstMin, secondMin};
    nodes.push(mergedNode);
  }

  unordered_map<int, string> characterEncodings;
  generateHuffmanCodes(nodes, &characterEncodings);

  string encodedString;
  ofstream outFile(ENCODED_PATH, ios::binary);
  ofstream outMap(HUFFMAN_PATH);

  for (int i=0; i<fileContents.size(); i++) {
      encodedString += characterEncodings[fileContents[i]];
  }

  int outputSize = 0;
  string subString;
  for (int i=0; i<encodedString.size(); i=i+chunkLength) {
    subString = encodedString.substr(i, chunkLength);
    outFile << binaryEncoding(subString);
    outputSize += 1;
  }

  for (auto t: characterEncodings) {
    if (t.second.size() > 0) {
      outMap << t.first << " " << t.second << endl;
    }
  }

  cout << endl << "Input file: " << INPUT_PATH << endl;
  cout << "Output file: " << ENCODED_PATH << endl << endl;
  cout << "Input file size: " << fileContents.size() << " Bytes" << endl;
  cout << "Output file size: " << outputSize << " Bytes" << endl;
  cout << "Compression ratio: " << (float)outputSize / (float)fileContents.size() << endl << endl;
}
