

#ifndef WEBSRV_PARSER_H
#define WEBSRV_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;


struct HTTPRequest {
    string method;  //请求的方法
    string uri;
    string version;
    string host;
    string connection;
    int contentLen=0;
    string content;
};
class Parser
{
public:
    Parser(const string request);
    HTTPRequest getParseResult();
private:
    void parseLine();
    void parseRequestLine();
    void parseHeader();
    void parseContent();//post
private:
    string m_request;
    vector<string> m_lines;
    HTTPRequest m_httpRequest;


};

#endif //WEBSRV_PARSER_H
