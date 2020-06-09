#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <stdio.h>

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
    {
        cout << "Unable to open file";
        exit(0);
    }
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

void writeMeta(string path, unordered_map<char, string> codes)
{
    ofstream file(path, ios::out | ios::binary | ios::trunc);
    if (file.is_open())
    {
        int no_of_codes = codes.size();
        file.write(reinterpret_cast<char *>(&no_of_codes), sizeof(no_of_codes));
        for (auto code : codes)
        {
            char letter = code.first;
            char *ch = new char[1];
            ch = &letter;

            file.write(ch, sizeof(char));
            int code_length = code.second.length();
            file.write(reinterpret_cast<char *>(&code_length), sizeof(code_length));

            file.write(code.second.c_str(), code_length);
        }

        file.close();
    }
    else
    {
        cout << "Unable to create file";
        exit(0);
    }
}

void writeText(string path, string encoded)
{
    string output_code = "";
    for (int i = 0; i < encoded.length() - 7; i += 8)
    {
        bitset<8> byte(encoded.substr(i, 8));
        output_code += char(byte.to_ulong());
    }

    ofstream file(path, ios::out | ios::binary | ios::app);
    if (file.is_open())
    {

        long long OCsize = output_code.size();
        file.write(reinterpret_cast<char *>(&OCsize), sizeof(OCsize));

        for (long long i = 0; i < output_code.size(); i++)
        {
            char *ch = new char;
            ch = &output_code[i];
            file.write(ch, sizeof(char));
        }

        file.close();
    }

    else
    {
        cout << "Unable to write encoded text";
        exit(0);
    }
}

void compress(bool passed, string passed_path)
{
    cout << "Compression" << endl;
    string path;
    if (!passed)
    {
        cout << "Enter Path to file to be compressed:";
        cin >> path;
    }
    else
    {
        path = passed_path;
    }

    cout << "Reading Data..." << endl;
    string input = getData(path);

    cout << "Reading Data Complete..." << endl;

    cout << "Creating Huffman Tree..." << endl;
    unordered_map<char, int> freq = calcFreq(input);
    Node *root = genTree(freq);

    unordered_map<char, string> codes;
    storeCodes(root, "", codes);
    cout << "Huffman Tree created..." << endl;

    cout << "Encoding..." << endl;
    string encoded = encode(input, codes);

    path.erase(path.find_last_of('.'));
    path.append("_compressed.bin");

    cout << "Creating compressed File..." << endl;
    writeMeta(path, codes);
    cout << "Metadata Written..." << endl;
    writeText(path, encoded);
    cout << "Compression Complete..." << endl;
}

int readMeta(string path, unordered_map<string, char> &reverse_map_codes)
{
    ifstream file(path, ios::in | ios::binary);
    if (file.is_open())
    {
        int no_of_codes;
        file.read(reinterpret_cast<char *>(&no_of_codes), sizeof(no_of_codes));

        for (int i = 0; i < no_of_codes; i++)
        {
            char *ch = new char;
            file.read(ch, sizeof(char));

            int code_length;
            file.read(reinterpret_cast<char *>(&code_length), sizeof(code_length));

            char *code = new char[code_length + 1];
            file.read(code, code_length);
            code[code_length] = '\0';

            reverse_map_codes[code] = *ch;

            delete[] ch;
            delete[] code;
        }

        int pos = file.tellg();
        file.close();
        return pos;
    }
    else
    {
        cout << "File does not exists.\n";
        exit(0);
    }

    return -1;
}

string readText(int pos, string path)
{
    ifstream file(path, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        string coded = "";
        file.seekg(pos, ios::beg);

        long long OCsize;
        file.read(reinterpret_cast<char *>(&OCsize), sizeof(OCsize));

        while (OCsize--)
        {
            char *ch = new char;
            file.read(ch, sizeof(char));

            int num = int(*ch);
            delete[] ch;
            bitset<8> byte(num);
            string charCode = byte.to_string();
            coded += charCode;
        }

        coded += '\0';

        file.close();

        return coded;
    }
    else
    {
        cout << "Error reading Encoded Text...\n";
        exit(0);
    }

    return "";
}

string decode(string decoded, unordered_map<string, char> reverse_map_codes)
{

    bitset<8> padByte(decoded.substr(0, 8));
    int pad = padByte.to_ulong();
    decoded.erase(decoded.end() - pad, decoded.end());
    decoded.erase(0, 8);
    string curr_code = "";
    string text = "";
    for (auto i : decoded)
    {
        curr_code += i;
        if (reverse_map_codes.find(curr_code) != reverse_map_codes.end())
        {
            text += reverse_map_codes[curr_code];
            curr_code = "";
        }
    }

    return text;
}

void decompress(bool passed, string passed_path)
{
    cout << "Decompress..." << endl;

    string path;
    if (!passed)
    {
        cout << "Enter Path to file to be decompressed:";
        cin >> path;
    }
    else
    {
        path = passed_path;
    }

    unordered_map<string, char> reverse_map_codes;

    int pos = readMeta(path, reverse_map_codes);

    cout << "Metadata read..." << endl;

    string encoded = readText(pos, path);

    cout << "Coded data read..." << endl;

    string decoded_text = decode(encoded, reverse_map_codes);

    cout << "Data decoded..." << endl;

    path.erase(path.find_last_of('_'));
    path.append("_decompressed.txt");

    ofstream file(path, ios::out | ios::trunc);
    if (file.is_open())
    {
        file << decoded_text;
        file.close();
    }
    else
    {
        cout << "cant create uncompressde file...\n"
             << endl;
        exit(0);
    }

    cout << "Decompression Complete..." << endl;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        char ch;
        cout << "\nCompress or decompress(C/D):";
        cin >> ch;
        while (!(ch == 'C' || ch == 'c' || ch == 'D' || ch == 'd'))
        {
            cout << "\n"
                 << "Enter \'C\' to Compress or \'D\' to decompress:";
            cin >> ch;
        }
        ch == 'C' || ch == 'c' ? compress(false, "") : decompress(false, "");
        return 0;
    }

    if (argc == 3)
    {
        char ch = argv[1][1];
        ch == 'C' || ch == 'c' ? compress(true, argv[2]) : decompress(true, argv[2]);
        return 0;
    }
}
