#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <iostream>
#include "httplib.h"


using namespace std;

char *conninfo = "dbname=cs744 user=dohil password=25m0836 host=localhost port=5432";


void insert(PGconn *conn,const char *params[],string &answer)
{
    PGresult *res;

    // const char *params[] = {"test_key","test_value"};
    
    res = PQexecParams(conn,"INSERT INTO kvstore(key, value) VALUES($1, $2)"
        "ON CONFLICT (key) DO UPDATE SET value = EXCLUDED.value",2,NULL,params,NULL,NULL,0);

    if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Insert Operation Failed : %s",PQerrorMessage(conn));
        answer = "Insert Operation Failed";
    }
    PQclear(res);
    answer = "Insertion Done Successfully";
    // cout<<answer;

}

void read(PGconn *conn,const char *params[],string &answer)
{
    PGresult *res;
    // const char *params[] = { "test_key" };
    res = PQexecParams(conn,"SELECT value FROM kvstore WHERE key = $1",1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
    {
        printf("Value for key '%s' = %s\n",params[0], PQgetvalue(res, 0, 0));
        answer = PQgetvalue(res, 0, 0);
    } 
    else 
    {
        printf("Key not found or SELECT failed: %s\n", PQerrorMessage(conn));
        answer = "failed to read";
    }
    PQclear(res);
    // cout<<answer;

}

void delete_op(PGconn *conn,const char *params[],string &answer)
{
    PGresult *res;
    // const char *params[] = { "test_key" };
    res = PQexecParams(conn,"DELETE FROM kvstore WHERE key = $1",1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(res) == PGRES_COMMAND_OK)
    {
        printf("Deleted %s row(s)\n", PQcmdTuples(res));
        answer = "Delete Successful";
    }
    else
    {
        fprintf(stderr, "DELETE failed: %s", PQerrorMessage(conn));
        answer = "Delete Operation Failed";
    }
    PQclear(res);
    // cout<<answer;
}


int main() {


    httplib::Server svr;


    svr.Post("/create", [](const httplib::Request &req, httplib::Response &res) 
    {
        auto key = req.get_param_value("key");
        auto value = req.get_param_value("value");
        string answer = "Key = " + key + " Value = " + value;
        const char *params[] = { key.c_str(),value.c_str() };
        PGconn *conn = PQconnectdb(conninfo);

        if (PQstatus(conn) != CONNECTION_OK)
        {
            // If not successful, print the error message and finish the connection
            answer = "Error while connecting to the database server";
        }
        else
        {
            insert(conn,params,answer);
        }

        // Finish the connection
        PQfinish(conn);
  
        res.set_content(answer,"text/plain");
    }
    );

    svr.Post("/read", [](const httplib::Request &req, httplib::Response &res) 
    {
        auto key = req.get_param_value("key");
        string answer;
        const char *params[] = { key.c_str()};
        PGconn *conn = PQconnectdb(conninfo);

        if (PQstatus(conn) != CONNECTION_OK)
        {
            // If not successful, print the error message and finish the connection
            answer = "Error while connecting to the database server";
        }
        else
        {
            read(conn,params,answer);
        }

        // Finish the connection
        PQfinish(conn);
  
        res.set_content(answer,"text/plain");
        
    }
    );

    svr.Post("/delete", [](const httplib::Request &req, httplib::Response &res) 
    {
        auto key = req.get_param_value("key");
        string answer;
        const char *params[] = { key.c_str()};
        PGconn *conn = PQconnectdb(conninfo);

        // Check if the connection is successful
        if (PQstatus(conn) != CONNECTION_OK) 
        {
            answer = "Error while connecting to the database server";
        }
        else
        {
            delete_op(conn,params,answer);
        }
        PQfinish(conn);
        
        res.set_content(answer,"text/plain");
        
    }
    );

    svr.listen("localhost", 8000);

    return 0;
}