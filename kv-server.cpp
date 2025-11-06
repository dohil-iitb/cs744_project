// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <unordered_map>

#define CACHESIZE 128

using namespace std;

unordered_map<string,string> cache;

int main()
{
  httplib::Server svr;

  svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("Hello World!", "text/plain");
  });

  svr.Post("/create", [](const httplib::Request &req, httplib::Response &res) 
  {
    auto key = req.get_param_value("key");
    auto value = req.get_param_value("value");
    string answer = "Key = " + key + " Value = " + value;
    cache[key]=value;
    res.set_content(answer,"text/plain");
  }
  );

  svr.Post("/read", [](const httplib::Request &req, httplib::Response &res) 
  {
    auto key = req.get_param_value("key");
    if(cache.find(key) != cache.end())
    {
      res.set_content(cache[key],"text/plain");
    }
    else
    {
      res.set_content("Key not in Cache","text/plain");
    }
    
  }
  );

  svr.Post("/delete", [](const httplib::Request &req, httplib::Response &res) 
  {
    auto key = req.get_param_value("key");
    if(cache.find(key) != cache.end())
    {
      cache.erase(key);      
    }
    
    res.set_content(key+" has been deleted successfully","text/plain");
    
  }
  );

  svr.listen("localhost", 8080);



}