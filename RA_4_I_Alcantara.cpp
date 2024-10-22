#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include <map>
using namespace std;

int main(int argc, char *argv[]) 
{
    string allFlag = "-all", dFlag = "-D", lFlag = "-L", pFlag = "-P";
    bool argAll = false, argD = false, argL = false, argP = false;
    map<char, int> letterCount, digitCount, punctuationCount;

    string fileName, line, word;
    char letterOccur = '\0', digitOccur = '\0', punctuationOccur = '\0';
    int countOfLO = 0, countOfDO = 0, countOfPO = 0;

    if (argc == 1) 
    {
        cerr << "NO SPECIFIED INPUT FILE NAME." << endl;

        return 1;
    }

    for (int i = 2; i < argc; ++i) 
    {
        if (std::string(argv[i]) == allFlag) 
        {
            argAll = true;
        } 
        else if (std::string(argv[i]) == dFlag) 
        {
            argD = true;
        } 
        else if (std::string(argv[i]) == lFlag) 
        {
            argL = true;
        } 
        else if (std::string(argv[i]) == pFlag) 
        {
            argP = true;
        }
    }

    ifstream inFile(argv[1]);

    if (!inFile.is_open()) 
    {
        cerr << "CANNOT OPEN THE FILE: " << argv[1] << endl;

        return 1;
    }

    if (inFile.peek() == ifstream::traits_type::eof()) 
    {
        cout << "File is empty." << endl;
        inFile.close();

        return 0;
    }

    while (getline(inFile, line)) 
    {
        stringstream ss(line);
        while (ss >> word) 
        {
            for (char ch : word) 
            {
                if (isalpha(ch)) 
                {
                    letterCount[toupper(ch)]++;
                }
                if (isdigit(ch)) 
                {
                    digitCount[ch]++;
                }
                if (ispunct(ch)) 
                {
                    punctuationCount[ch]++;
                }
            }
        }
    }

    for (const auto & pair : letterCount) 
    {
        if (pair.second > countOfLO) 
        {
            letterOccur = pair.first;
            countOfLO = pair.second;
        }
    }

    for (const auto & pair : digitCount) 
    {
        if (pair.second > countOfDO) 
        {
            digitOccur = pair.first;
            countOfDO = pair.second;
        }
    }

    for (const auto & pair : punctuationCount) 
    {
        if (pair.second > countOfPO) 
        {
            punctuationOccur = pair.first;
            countOfPO = pair.second;
        }
    }

    if (!letterCount.empty()) 
    {
        cout << "Letter of Largest number of occurrences: " << letterOccur << " occurred " << countOfLO << " times" << endl;
    }

    if (!digitCount.empty()) 
    {
        cout << "Digits of Largest number of occurrences: " << digitOccur << " occurred " << countOfDO << " times" << endl;
    }

    if (!punctuationCount.empty()) 
    {
        cout << "Punctuations of Largest number of occurrences: " << punctuationOccur << " occurred " << countOfPO << " times" << endl;
    }

    if (argAll) 
    {
        if (!letterCount.empty()) 
        {
            cout << "List of Letters seen in the file and their number of occurrences:" << endl;
            for (const auto & pair : letterCount) 
            {
                cout << pair.first << ": " << pair.second << endl;
                cout << endl;
            }
        }

        if (!digitCount.empty()) 
        {
            cout << "List of Digits seen in the file and their number of occurrences:" << endl;
            for (const auto & pair : digitCount) 
            {
                cout << pair.first << ": " << pair.second << endl;
                cout << endl;
            }
        }

        if (!punctuationCount.empty())
         {
            cout << "List of Punctuations seen in the file and their number of occurrences:" << endl;
            for (const auto & pair : punctuationCount) 
            {
                cout << pair.first << ": " << pair.second << endl;
                cout << endl;
            }
        }
    }

    if (argD && countOfDO != 0) 
    {
        cout << "List of Digits seen in the file and their number of occurrences:" << endl;
        for (const auto & pair : digitCount) 
        {
            cout << pair.first << ": " << pair.second << endl;
            cout << endl;
        }
    }

    if (argL && countOfLO != 0) 
    {
        cout << "List of Letters seen in the file and their number of occurrences:" << endl;
        for (const auto & pair : letterCount) 
        {
            cout << pair.first << ": " << pair.second << endl;
            cout << endl;
        }
    }

    if (argP && countOfPO != 0) 
    {
        cout << "List of Punctuations seen in the file and their number of occurrences:" << endl;
        for (const auto & pair : punctuationCount) 
        {
            cout << pair.first << ": " << pair.second << endl;
            cout << endl;
        }
    }

    inFile.close();
    return 0;
}