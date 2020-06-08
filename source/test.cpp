#include <bits/stdc++.h>
using namespace std;

int main()
{
    ofstream file("new.bin", ios::out | ios::binary | ios::app);
    if (file.is_open())
    {
        char a = 'a';

        for (long long i = 0; i < 5; i++)
        {
            char *ch = new char;
            ch = &a;
            file.write(ch, sizeof(ch));
            cout << sizeof(ch);
        }

        cout << "compressed end" << file.tellp() << endl;
        file.close();
    }

    cout << "sizeof char:" << sizeof(char);
}