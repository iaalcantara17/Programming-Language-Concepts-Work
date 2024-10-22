#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

int main(){
    ifstream inFile;
    string filename;
    string line;
    int lines = 0;
    int uncommentedLines = 0;
    int maxLines = 0;
    int numWords = 0;
    string maxWordLine;
    string word;
    int maxWord = 0;
    string maxLen;
    
    cout << "Enter the name of a file to read from: \n" << endl;
    cin >> filename;
    inFile.open(filename.c_str());
    if(!inFile){
        cout << "File cannot be opened " << filename << endl;
        exit(1);
    }
    while(getline(inFile, line)){
        lines++;
        if(line.find("#") != string::npos || line.find("//") != string::npos){
            continue;
        }
        else{
          uncommentedLines++;
          if(line.length() > maxLines){
              maxLines = line.length();
              maxWordLine = line;
          }
          istringstream words(line);

          while( words >> word){
              numWords++;
              if(word.length() > maxWord){
                  maxWord = word.length();
                  maxWordLine = word;
              }
        }
        }
    }
    
    cout << "Total Number of Lines: " << lines << endl;
    cout << "Number of non-commented lines: " << uncommentedLines << endl;
    cout << "Line of Maximum Length: \"" << maxWordLine << "\"" << endl;
    cout << "Number of Words: " << numWords << endl;
    cout << "Word of Maximum Length: \"" << maxLen << "\"" << endl;

    return 0;
    }