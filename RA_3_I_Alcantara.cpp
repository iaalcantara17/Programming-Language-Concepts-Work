#include <iostream>
#include <fstream>
#include <cctype>
#include <sstream>

using namespace std;


int main(int argc, char *argv[])
{
    ifstream inFile;

    string inputFileName = "";
    string currentWord = "";
    string currentLine = "";

    int numberOfLines = 0;

    int characterCount = 0;
    int extraCharacterCount = 0;
    int alphabeticCount = 0;
    int digitCount = 0;

    int totalWordCount = 0;
    int nonBlankLinesCount = 0;
    int nameCount = 0;
    int integerCount = 0;

    int wordMax = 0;
    int wordMin = 0;

    if (argc == 1)
    {
        cerr << "NO SPECIFIED INPUT FILE NAME." << endl;
        exit(1);
    }

    else
    {
        inputFileName = argv[1];
        inFile.open(inputFileName.c_str());

        if (!inFile)
        {
        cout << "CANNOT OPEN THE FILE: " << inputFileName << endl;
        exit(1);
        }
    }

    while (getline(inFile, currentLine))
    {
        numberOfLines++;
     
        istringstream myString(currentLine);

        while (myString >> currentWord)
        {
            totalWordCount++;
            characterCount = 0;
            alphabeticCount = 0;
            digitCount = 0;
            extraCharacterCount = 0;

            for (char i: currentWord)
            {
                characterCount++;
               
                if (isalpha(i))
                {
                    alphabeticCount++;
                }
               
                if (isdigit(i))
                {
                    digitCount++;
                }
                if (i == '_' || i == '@')
                {
                    extraCharacterCount++;
                }
                   
            }

            if (currentWord.length() <= 5)
            {
                wordMin++;
            }
            if (currentWord.length() > 5)
            {
                wordMax++;
            }
            if (isdigit(currentWord[0]) && characterCount - digitCount == 0)
            {
                integerCount++;
            }
            if (isalpha(currentWord[0]) && characterCount - digitCount - extraCharacterCount - alphabeticCount == 0)
            {
                nameCount++;
            }


        }
     
        if (!currentLine.empty())
        {
            nonBlankLinesCount++;
        }

    }

    if (numberOfLines == 0)
    {
        cout << "File is empty." << endl;
        return 0;
    }

    cout << "Total Number of Lines: " << numberOfLines << endl;
    cout << "Number of non-blank lines: " << nonBlankLinesCount << endl;
    cout << "Number of Words: " << totalWordCount << endl;
    cout << "Number of Integers: " << integerCount << endl;
    cout << "Number of Names: " << nameCount << endl;
    cout << "Number of Words Less Than or Equal to 5 Characters: " << wordMin << endl;
    cout << "Number of Words Greater Than 5 Characters: " << wordMax << endl;

    return 0;
}  