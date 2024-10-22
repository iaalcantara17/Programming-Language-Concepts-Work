#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{

    string firstname;
    string lastname;
    string sectionnumber;

    cout << "Please enter your first name >>" << endl;
    cin >> firstname;
    cout << "Please enter your last name >>" << endl;
    cin >> lastname;
    cout << "Please enter your CS 280 section number >>" << endl; 
    cin >> sectionnumber;
    cout << "Hello " << firstname << " " << lastname << ", welcome to CS 280, Section " << sectionnumber << endl;

    return 0;
}