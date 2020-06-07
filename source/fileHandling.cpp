#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>

using namespace std;

class Node
{
public:
    char c;
    int freq;
    bool isLeaf;
    Node *left;
    Node *right;
    Node(char ch, int f, bool leaf = false)
    {
        this->c = ch;
        this->freq = f;
        this->isLeaf = leaf;
        this->left = NULL;
        this->right = NULL;
    }
};

class Comp
{
public:
    bool operator()(Node *a, Node *b)
    {
        return a->freq > b->freq;
    }
};

string getData(string path)
{
    ifstream file(path, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        int size = file.tellg();
        char *memblock = new char[size];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        file.close();

        string data = memblock;

        delete[] memblock;

        return data;
    }
    else
        cout << "Unable to open file";
}

unordered_map<char, int> calcFreq(string input)
{
    unordered_map<char, int> freq;
    for (auto i : input)
        freq[i]++;

    return freq;
}

Node *genTree(unordered_map<char, int> freq)
{
    priority_queue<Node *, vector<Node *>, Comp> minHeap;

    for (auto p : freq)
    {
        minHeap.push(new Node(p.first, p.second, true));
    }

    while (minHeap.size() > 1)
    {
        Node *root, *l, *r;

        l = minHeap.top();
        minHeap.pop();

        r = minHeap.top();
        minHeap.pop();

        root = new Node('`', l->freq + r->freq);
        root->left = l;
        root->right = r;
        minHeap.push(root);
    }
    return minHeap.top();
}

void storeCodes(Node *root, string code, unordered_map<char, string> &codes)
{
    if (root->isLeaf)
    {
        codes[root->c] = code;
    }

    else
    {
        storeCodes(root->left, code + "0", codes);
        storeCodes(root->right, code + "1", codes);
    }
}

void addPadding(string &encoded)
{
    int padding_len = 8 - encoded.length() % 8;

    bitset<8> padding_byte(padding_len);

    string padding_code = padding_byte.to_string();

    encoded = padding_code + encoded;

    for (int i = 0; i < padding_len; i++)
        encoded += "0";
}

string encode(string input, unordered_map<char, string> codes)
{
    string encoded = "";

    for (auto ch : input)
        encoded += codes[ch];

    addPadding(encoded);

    return encoded;
}

void writeMeta(unordered_map<char, string> codes)
{
    ofstream file("compressed.bin", ios::out | ios::binary | ios::trunc);
    if (file.is_open())
    {
        int no_of_codes = codes.size();
        file.write(reinterpret_cast<char *>(&no_of_codes), sizeof(no_of_codes));
        for (auto code : codes)
        {
            char letter = code.first;
            char *ch = new char;
            ch = &letter;

            file.write(ch, 1);
            int code_length = code.second.length();
            file.write(reinterpret_cast<char *>(&code_length), sizeof(code_length));
            file.write(code.second.c_str(), code_length);
        }
    }
    else
        cout << "Unable to create file";
}

void compress()
{
    string path;
    // cin<<path;
    path = "text.txt";
    string input = getData(path);
    unordered_map<char, int> freq = calcFreq(input);
    Node *root = genTree(freq);

    unordered_map<char, string> codes;
    storeCodes(root, "", codes);

    string encoded = encode(input, codes);
    writeMeta(codes);
    // writeCode(encoded);
    cout << encoded;
}

void readMeta()
{
}

void decompress()
{

    unordered_map<string, char> reverse_map_codes;
    readMeta()
}

int main()
{
    char ch;
    cout << "Compress or decompress(C/D)";
    cin >> ch;
    ch == 'C' ? compress() : decompress();
    return 0;
}