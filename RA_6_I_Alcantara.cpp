#include <iostream>
#include <string>
#include <fstream>
using namespace std;

bool isVowel(char ch) 
{
    ch = tolower(ch);
    return (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u');
}

int CountVowelsS(const string& istr, int indx) 
{
    if (indx == istr.length())
        return 0; 
    return isVowel(istr[indx]) + CountVowelsS(istr, indx + 1);
}

int CountVowels(string istr) 
{
    return CountVowelsS(istr, 0);
}