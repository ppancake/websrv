#include "Parser.h"

Parser::Parser(const string request)
{
    if(request.empty())
        return;
    this->m_request=request;
}



//根据\r\n将其分开解析每行的东西
void Parser::parseLine()
{
    string::size_type begin = 0;   // 正在解析的行的行首索引
    string::size_type idx = 0;  // 正在解析的字符索引

    while(idx<m_request.size())
    {
        //碰到回车
        if(m_request[idx]=='\r')
        {
            //如果整个到头了,却不是\r\n的结局，有问题
            if((idx+1)==m_request.size())
            {
                cout<<"request is not read cpmplete"<<endl;
                return;
            }
            else if(m_request[idx+1]=='\n')
            {
                m_lines.push_back(string(m_request,begin,idx-begin));
                idx+=2;
                begin=idx;
            }
            else
            {
                cout<<"request is error"<<endl;
                return;
            }
        }

        else
            idx++;
    }

}


//GET https://www.baidu.com/ HTTP/1.1
void Parser::parseRequestLine()
{
    if(m_lines.empty())
        return;
    string line=m_lines[0];
    string delims(" \t");
    string::size_type idx=0,idx1=0;
    idx=line.find_first_of(delims);


    if(idx==string::npos)
    {
        cout<<"request error"<<endl;
        return;
    }
    m_httpRequest.method=line.substr(0,idx);


    idx1=line.find_first_not_of(delims,idx);
    if(idx1==string::npos)
    {
        return;
    }
    idx=line.find_first_of(delims,idx1);
    m_httpRequest.uri=line.substr(idx1,idx-idx1);

    idx1=line.find_first_not_of(delims,idx);
    if(idx==string::npos)
    {
        return;
    }
    idx=line.find_first_of(delims,idx1);
    m_httpRequest.version=line.substr(idx1,idx-idx1);


    
}

void Parser::parseHeader()
{
    string delims(" \t");
    if(m_lines.empty())
        return;
    for(int i=1;i<m_lines.size();i++)
    {
        if(m_lines[i].empty())
            return;

        else if(m_lines[i].compare(0,5,"Host:")==0)
        {
            string::size_type idx=5;
            idx=m_lines[i].find_first_not_of(delims,5);
            m_httpRequest.host=m_lines[i].substr(idx);
        }

        else if(m_lines[i].compare(0,11,"Connection:")==0)
        {
            string::size_type idx=11;
            idx=m_lines[i].find_first_not_of(delims,11);
            m_httpRequest.host=m_lines[i].substr(idx);
        }
        else if(m_lines[i].compare(0,15,"Content-Length:")==0)
        {
            string::size_type idx=15;
            idx=m_lines[i].find_first_not_of(delims,15);
            m_httpRequest.contentLen=stoi(m_lines[i].substr(idx));
        }
        else
            continue;
    }
}

void Parser::parseContent()
{
    int i=m_request.size()-m_httpRequest.contentLen;
    string tmp;
    while(m_request[i]!='\r'&&m_request[i]!='\0')
        tmp+=m_request[i++];
    m_httpRequest.content=tmp;
}

HTTPRequest Parser::getParseResult()
{
    parseLine();
    parseRequestLine();
    parseHeader();
    if(m_httpRequest.contentLen!=0)
        parseContent();
    return m_httpRequest;
}