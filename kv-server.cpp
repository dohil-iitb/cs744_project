// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <unordered_map>

#define CACHESIZE 128000 // change it according to the memory available

using namespace std;

unordered_map<string,string> cache;
bool cache_full=false;
list<string> lru;

int main()
{
  httplib::Server svr;
  // httplib::Client cli("http://127.0.0.1:8080");

  svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("You have connected to the server!!!", "text/plain");
  });

  svr.Post("/create", [](const httplib::Request &req, httplib::Response &res) 
  {
    auto key = req.get_param_value("key");
    auto value = req.get_param_value("value");
    // cout<<key<<" "<<value;
    string answer = "Key = " + key + " Value = " + value;
    if(cache.find(key)==cache.end() && cache_full)
    {
      //eviction code;
      string evicted_key = lru.back();

      httplib::Client cli("http://127.0.0.1:8000");
      httplib::Params params;
      params={
        { "key", evicted_key },
        { "value", cache[evicted_key]}
      };
      auto res_db = cli.Post("/create",params);
      lru.pop_back();
      cache.erase(evicted_key);

      cache[key]=value;
      lru.push_front(key);

    }
    else
    {
      lru.remove(key);
      cache[key]=value;
      lru.push_front(key);
    }
    

    if(cache.size() == CACHESIZE)
    {
      cache_full=true;
    }
    res.set_content(answer,"text/plain");
  }
  );

  svr.Post("/read", [](const httplib::Request &req, httplib::Response &res) 
  {
    auto key = req.get_param_value("key");
    if(cache.find(key) != cache.end())
    {
      res.set_content(cache[key],"text/plain");
      lru.remove(key);
      lru.push_front(key);
    }
    else
    {

      httplib::Client cli("http://127.0.0.1:8000");
      httplib::Params params;
      params = { {"key",key}};
      auto res_db = cli.Post("/read",params);

      if(res_db->body != "failed to read")
      {
        if(cache_full)
        {
          string evicted_key = lru.back();
          httplib::Client cli("http://127.0.0.1:8000");
          httplib::Params params;
          params={
            { "key", evicted_key },
            { "value", cache[evicted_key]}
          };
          auto res_db = cli.Post("/create",params);
          lru.pop_back();
          cache.erase(evicted_key);
        }
        lru.remove(key);
        cache[key]=res_db->body;
        lru.push_front(key);
        
      }
      if(cache.size() == CACHESIZE)
      {
        cache_full=true;
      }

      res.set_content(res_db->body,"text/plain");
    }
    
  }
  );

  svr.Post("/delete", [](const httplib::Request &req, httplib::Response &res) 
  {
    auto key = req.get_param_value("key");
    if(cache.find(key) != cache.end())
    {
      cache.erase(key);
      httplib::Client cli("http://127.0.0.1:8000");
      httplib::Params params;
      params = { {"key",key}};
      auto res_db = cli.Post("/delete",params);
      res.set_content(res_db->body,"text/plain");
    }
    else
    {
      httplib::Client cli("http://127.0.0.1:8000");
      httplib::Params params;
      params = { {"key",key}};
      auto res_db = cli.Post("/delete",params);
      res.set_content(res_db->body,"text/plain");
    }
  }
  );

  svr.listen("localhost", 8080);

}