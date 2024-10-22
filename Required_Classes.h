//Israel Alcantara

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream> 
#include <iterator>
#include <algorithm>
#include "NewsPaperSubscriber.h"

using namespace std;

class WeeklySubscriber : public NewsPaperSubscriber 
{
    string service_type = "Weekly Subscriber";
    public:
    WeeklySubscriber(Address addr) 
    {
        st_Address = addr;
        rate = setRate();
    }

    double setRate() override 
    {
        return rate = 10.5;
    } 

    void print() const override 
    {
        cout << st_Address.AddrContents() << endl;
        cout << "Rate: " << rate << endl;
        cout << "Service Type: " << service_type << endl;
    }
};

class WeekdaysSubscriber : public NewsPaperSubscriber 
{
    string service_type = "Week Days Subscriber";
    public:
    WeekdaysSubscriber(Address addr) 
    {
        st_Address = addr;
        rate = setRate();
    }

    double setRate() override 
    {
        return rate = 7.5;
    } 

    void print() const override 
    {
        cout << st_Address.AddrContents() << endl;
        cout << "Rate: " << rate << endl;
        cout << "Service Type: " << service_type << endl;
    }
};

class WeekendSubscriber : public NewsPaperSubscriber 
{
    string service_type = "Weekend Subscriber";
    public:
    WeekendSubscriber(Address addr) 
    {
        st_Address = addr;
        rate = setRate();
    }

    double setRate() override 
    {
        return rate = 4.5;
    } 

    void print() const override 
    {
        cout << st_Address.AddrContents() << endl;
        cout << "Rate: " << rate << endl;
        cout << "Service Type: " << service_type << endl;
    }
};

void CheckMultipleSubscriptions(NewsPaperSubscriber *potentialSubscriber,
                           vector<NewsPaperSubscriber *> &acceptedSubscribers,
                           vector<NewsPaperSubscriber *> &rejectedSubscribers) 
{
    bool alreadySubscribed = false;
    Address potentialAddress = potentialSubscriber->getAddress();

    for (auto existingSubscriber : acceptedSubscribers) 
    {
        if (existingSubscriber->getAddress() == potentialAddress) 
        {
            alreadySubscribed = true;
            break;
        }
    }

    if (alreadySubscribed) 
    {
        rejectedSubscribers.push_back(potentialSubscriber);
    } 
    else 
    {
        acceptedSubscribers.push_back(potentialSubscriber);
    }
}