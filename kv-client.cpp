// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>
using namespace std;
int main()
{

    httplib::Client cli("http://127.0.0.1:8080");

    auto res1 = cli.Get("/hi");
    // res->status;
    // res->body;
    cout<<res1->status<< endl;
    cout<<res1->body<< endl;

    
    httplib::Params params;
    
    
    string command;
    string key,value;
    while(true)
    {
        cin>>command;
        if(command=="create")
        {
            
            cout << "Enter the key value pair : -" <<endl;
            cin >> key >> value;

            params={
                { "key", key },
                { "value", value}
            };
            auto res2 = cli.Post("/create",params);
            cout<<res2->body<<endl;

        }
        else if(command == "read")
        {
            cout << "Enter the key to read : -" <<endl;
            cin >> key;

            params = { {"key",key}};

            auto res3 = cli.Post("/read",params);
            cout<<res3->body<<endl;
        }
        else if(command == "delete")
        {
            cout << "Enter the key to delete : -" <<endl;
            cin >> key;

            params = { {"key",key}};

            auto res4 = cli.Post("/delete",params);
            cout<<res4->body<<endl;
        }
        else 
        {
            cout<<"Invalid Command!!! Exiting ... "<<endl;
            break;
        }
    }

    return 0;
}
