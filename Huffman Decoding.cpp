#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

string ENCODED_PATH("D:/Pro Ramming/CPP/Dataset/Huffman/Encoded File.txt");
ifstream HUFFMAN_STREAM("D:/Pro Ramming/CPP/Dataset/Huffman/Huffman Map.txt");
ofstream DECODED_STREAM("D:/Pro Ramming/CPP/Dataset/Huffman/Huffman Decoded.txt", ios::binary);

int ENCODING_CHUNK_LENGTH = 8;

// Read huffman map from file and recreate the map
// Gotta be a better way to do this
void createMap(string s, unordered_map<char, string>* map) {
  int spacePos = -1;
  for (int i=0; i<s.size(); i++) {
    if (s[i] == ' ') spacePos = i;
  }

  int key = stoi(s.substr(0, spacePos));
  string value = s.substr(spacePos+1, s.size() - spacePos - 1);

  (*map)[char(key)] = value;
}

// Doing things in reverse, for each key check if its value equals the input 
bool valuePresent(unordered_map<string, char>* reverseMap, string value, char* c) {
  if (reverseMap->find(value) == reverseMap->end()) {
    return false;
  }
  *c = (*reverseMap)[value];
  return true;
}

bool printKey(unordered_map<string, char>* map, int min, int max, int* index, string* encodedString) {
  char c;
  string temp;
  for (int j=min; j<=max; j++) {
    temp = (*encodedString).substr(*index, j);
    if (valuePresent(map, temp, &c)) {
      // cout << c;
      DECODED_STREAM << c;
      *index = *index + j;
      return true;
    }
  }
  return false;
}

static std::vector<char> ReadAllBytes(string filename)
{
    ifstream ifs(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}


string decoding(char input) {
    int decimal = (int)input;
    string bin = "";
    int mask = 128;

    if (decimal < 0) decimal += 256;

    while (mask > 0) {
      if (decimal & mask) bin += '1';
      else bin += '0';
      mask = mask >> 1;
    }

    return bin;
}

int main() {
  unordered_map<char, string> huffmanMap;
  unordered_map<string, char> reverseHuffmanMap;

  vector<char> encodedString;
  char character;
  char* token;
  string code, lineContents;
  string encodedStringBinary;

  encodedString = ReadAllBytes(ENCODED_PATH);

  while (getline(HUFFMAN_STREAM, lineContents)) {
    if (lineContents != "")
      createMap(lineContents, &huffmanMap);
  }

  for (auto t: huffmanMap) {
    reverseHuffmanMap[t.second] = t.first;
  }
  
  for (int i=0; i<encodedString.size(); i+=1) {
    encodedStringBinary += decoding(encodedString[i]);
  }

  int min = 10e6;
  int max = 0;
  for (auto t:huffmanMap) {
    if (t.second.size() < min) min = t.second.size();
    if (t.second.size() > max) max = t.second.size();
  }

  int index = 0;
  while (true) {
    if (!printKey(&reverseHuffmanMap, min, max, &index, &encodedStringBinary)) break;
  }
}
