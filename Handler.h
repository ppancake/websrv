
#ifndef WEBSRV_HANDLER_H
#define WEBSRV_HANDLER_H
#include <string>
#include <algorithm>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Parser.h"
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
class Handler
{
public:
    Handler(const int connfd);
    ~Handler();
    const int getfd()const {return m_connfd;}
    void handle();
private:
    bool receiveRequest();  // 接受客户的请求并解析
    void sendResponse();    // 发送相应



    void sendErrorMsg(const std::string &errorNum,
                      const std::string &shortMsg,
                      const std::string &longMsg);
    void parseURI();
    void getFileType();

    void responseGet();
    void responsePost();


    void response(char *message, int status);
    void response_get(char *filename);
    void response_file(int size, int status);
    void response_post(char *filename, char *argv);



    int m_connfd;
    bool m_isClosed;
    std::string m_fileType;
    std::string m_fileName;
    HTTPRequest m_request;
};

#endif //WEBSRV_HANDLER_H
