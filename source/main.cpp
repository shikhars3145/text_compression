#include <iostream>
#include <queue>
#include <unordered_map>
#include <bitset>
#include <string>

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

void showpq(priority_queue<Node *, vector<Node *>, Comp> gq)
{
    priority_queue<Node *, vector<Node *>, Comp> g = gq;
    while (!g.empty())
    {
        cout << g.top()->c << ":" << g.top()->freq << endl;
        g.pop();
    }
    cout << '\n';
}

void storeCodes(Node *root, string code, unordered_map<string, char> &codeToChar, unordered_map<char, string> &charToCode)
{
    if (root->isLeaf)
    {
        // cout << root->c << ":" << code;
        charToCode[root->c] = code;
        codeToChar[code] = root->c;
    }

    else
    {
        storeCodes(root->left, code + "0", codeToChar, charToCode);
        storeCodes(root->right, code + "1", codeToChar, charToCode);
    }
}

int main()
{

    string input = "dacbecabdbaeccdbdccb";
    unordered_map<char, int> freq;

    for (auto k : input)
    {
        freq[k]++;
    }

    priority_queue<Node *, vector<Node *>, Comp> minHeap;

    for (auto p : freq)
    {
        minHeap.push(new Node(p.first, p.second, true));
        // showpq(minHeap);
    }

    while (minHeap.size() > 1)
    {
        Node *top, *l, *r;

        l = minHeap.top();
        minHeap.pop();
        // cout << "l=" << l->freq << endl;
        r = minHeap.top();
        minHeap.pop();
        // cout << "r=" << r->freq << endl;
        top = new Node('`', l->freq + r->freq);
        top->left = l;
        top->right = r;
        minHeap.push(top);
    }

    unordered_map<string, char> codeToChar;
    unordered_map<char, string> charToCode;

    storeCodes(minHeap.top(), "", codeToChar, charToCode);

    // cout << "hello";
    // for (auto i : codeToChar)
    //     cout << i.first << ":" << i.second << endl;

    for (auto i : charToCode)
        cout << i.first << ":" << i.second << endl;

    string encoded;

    for (auto ch : input)
    {
        encoded += charToCode[ch];
    }

    cout << "\nEncoded:";
    cout << encoded;

    cout << endl;
    cout << encoded.length();

    int padding_len = 8 - encoded.length() % 8;
    cout << endl;
    cout << padding_len;

    bitset<8> padding_byte(padding_len);

    string padding_code = padding_byte.to_string();

    cout << endl
         << padding_code;

    encoded = padding_code + encoded;

    for (int i = 0; i < padding_len; i++)
    {
        encoded += "0";
    }

    cout << "\nPadded Encoded:";
    cout << encoded;

    string output_code = "";
    for (int i = 0; i < encoded.length() - 7; i += 8)
    {
        bitset<8> byte(encoded.substr(i, 8));
        output_code += char(byte.to_ulong());
        cout << "\nSize loop:" << output_code.size();
    }

    cout << "\nOutput Code:" << output_code << "\nSize:" << output_code.size() << "\nIn size" << input.size();

    int pad = int(output_code[0]);

    string decoded = "";
    for (int i = 1; i < output_code.size(); i++)
    {
        int num_code = int(output_code[i]);
        bitset<8> byte(num_code);
        decoded += byte.to_string();
    }

    cout << "\nDecoded:" << decoded;

    // int pad_pos = decoded.size() - pad;

    decoded.erase(decoded.end() - pad, decoded.end());

    cout << "\nDecoded:" << decoded;

    string curr_code = "";
    string decoded_output = "";
    for (auto i : decoded)
    {
        curr_code += i;
        if (codeToChar.find(curr_code) != codeToChar.end())
        {
            decoded_output += codeToChar[curr_code];
            curr_code = "";
        }
    }

    cout << "\nDecoded output:" << decoded_output;

    return 0;
}