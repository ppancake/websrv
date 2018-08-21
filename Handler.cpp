#include "Handler.h"

char *path = "/home/zwk/CLionProjects/websrv/cmake-build-debug";
const int BUFFER_SIZE=65536;
Handler::Handler(const int connfd)
{
    m_connfd=connfd;
    m_isClosed=false;

}

Handler::~Handler()
{
    //没关的话就关掉
    if(!m_isClosed)
        close(m_connfd);
}



//数据读取
int readFd(const int fd,string &s)
{

    char buf[65535];
    char *ptr =buf;
    int nleft = 65535;
    int nread;
    while((nread = read(fd, ptr, nleft)) < 0)
    {
        if(errno == EINTR)
            nread = 0;
        else
            return 0;
    }
    s.append(buf, nread);
    return nread;
}
bool Handler::receiveRequest()
{
    string s;
    if(readFd(m_connfd,s)==0)
        return false;
    cout<<"request:  "<<endl<<s<<endl;
    Parser p(s);
    m_request=p.getParseResult();
    return true;


}


//发送数据,结果还是sendfile好用
void sendFd(const int fd,string s)
{
    size_t bytesSent = 0;
    size_t bytesLeft = s.size();
    char buf[bytesLeft];
    strcpy(buf,s.c_str());
    char *ptr =buf;
    while(bytesLeft)
    {
        if((bytesSent = write(fd, ptr, bytesLeft)) < 0)
        {
            if(bytesSent < 0 && errno == EINTR)
                bytesSent = 0;
            else
                return;
        }
        bytesLeft -= bytesSent;
        ptr += bytesSent;
    }

}
void Handler::sendErrorMsg(const std::string &errorNum, const std::string &shortMsg, const std::string &longMsg)
{
    string msg = "HTTP /1.0 " + errorNum + " " + shortMsg + "\r\n";
    msg += "Content-type: text/html\r\n";
    msg += "<html><title>Error</title>";
    msg += "<body bgcolor=""ffffff"">\r\n";
    msg += errorNum + ": " + shortMsg + "\r\n";
    msg += "<p>" + longMsg + ": " + m_fileName + "\r\n";
    msg += "<hr><em>The Tiny Web server</em>\r\n";
    sendFd(m_connfd,msg);
    close(m_connfd);
    m_isClosed = true;
}

/*
//读取后进行处理,后续添加POST方法
void Handler::handle()
{
    if(!receiveRequest())
    {
        close(m_connfd);
        m_isClosed=true;
        return;
    }

    if(m_request.method=="GET")
    {
        responseGet();
    }
    else if(m_request.method=="POST")
    {
        responsePost();
    }
    else
    {
        sendErrorMsg("501", "Not Implemented", "Server doesn't implement this method");
        return;
    }
}
*/
void Handler::responseGet()
{
    parseURI();
    struct stat fileInfo;
    //文件路径绑定
    if(stat(m_fileName.c_str(), &fileInfo) < 0)
    {
        std::cout << "404 Not found: Server couldn't find this file." << std::endl;
        sendErrorMsg("404", "Not Found", "Server couldn't find this file");
        return;
    }
    //S_ISREG一般文件 S_IRUSR可读
    if(!(S_ISREG(fileInfo.st_mode)) || !(S_IRUSR & fileInfo.st_mode))
    {
        std::cout << "403 Forbidden: Server couldn't read this file." << std::endl;
        sendErrorMsg("403", "Forbidden", "Server couldn't read this file");
        return;
    }
    getFileType();
    std::string msg = "HTTP/1.1 200 OK\r\n";
    msg += "Server: Tiny Web Server\r\n";
    msg += "Content-length: " + std::to_string(fileInfo.st_size) + "\r\n";
    msg += "Content_type: " + m_fileType + "\r\n\r\n";
    int fd = open(m_fileName.c_str(), O_RDONLY, 0);
    readFd(fd,msg);
    sendFd(m_connfd,msg);
    close(m_connfd);
    m_isClosed = true;
}

//目前只能针对指定的网页格式
void Handler::responsePost()
{

    char file[100];
    strcpy(file, path);
    strcat(file, m_fileName.c_str());
    parseURI();
    struct stat fileInfo;
    //文件路径绑定
    if(stat(m_fileName.c_str(), &fileInfo) < 0)
    {
        std::cout << "404 Not found: Server couldn't find this file." << std::endl;
        sendErrorMsg("404", "Not Found", "Server couldn't find this file");
        return;
    }
    //S_ISREG一般文件 S_IRUSR可读
    if(!(S_ISREG(fileInfo.st_mode)) || !(S_IRUSR & fileInfo.st_mode))
    {
        std::cout << "403 Forbidden: Server couldn't read this file." << std::endl;
        sendErrorMsg("403", "Forbidden", "Server couldn't read this file");
        return;
    }


    char argv[20];
    int a, b;
    int ret = sscanf(m_request.content.c_str(),"a=%d&b=%d", &a, &b);
    if(ret < 0 || ret != 2)
    {
        std::cout << "403 Forbidden: SParameter error." << std::endl;
        sendErrorMsg("403", "Forbidden", "Parameter error");
        return;
    }
    sprintf(argv, "%d&%d", a, b);
    if(fork() == 0)
        /*创建子进程执行对应的子程序，多线程中，创建子进程，
        只有当前线程会被复制，其他线程就“不见了”，这正符合我们的要求，
        而且fork后执行execl，程序被进程被重新加载*/
    {
        dup2(m_connfd, STDOUT_FILENO);
        //将标准输出重定向到sockfd,将子程序输出内容写到客户端去。
        execl(file, argv); //执行子程序
    }
    close(m_connfd);
    m_isClosed = true;

}


void Handler::parseURI()
{
    m_fileName = ".";
    if(m_request.uri == "/")
        m_fileName += "/index.html";
}

void Handler::getFileType()
{
    const char *name = m_fileName.c_str();
    if(strstr(name, ".html"))
        m_fileType = "text/html";
    else if(strstr(name, ".pdf"))
        m_fileType = "application/pdf";
    else if(strstr(name, ".png"))
        m_fileType = "image/png";
    else if(strstr(name, ".gif"))
        m_fileType = "image/gif";
    else if(strstr(name, ".jpg"))
        m_fileType = "image/jpg";
    else if(strstr(name, ".jpeg"))
        m_fileType = "image/jpeg";
    else if(strstr(name, ".css"))
        m_fileType = "test/css";
    else
        m_fileType = "text/plain";
}


void Handler::response(char *message, int status)
{
    char buf[512];
    sprintf(buf, "HTTP/1.1 %d OK\r\nConnection: Close\r\n"
                 "content-length:%d\r\n\r\n", status, strlen(message));

    sprintf(buf, "%s%s", buf, message);
    write(m_connfd, buf, strlen(buf));

}
void Handler::response_file(int size, int status)
{
    char buf[128];
    sprintf(buf, "HTTP/1.1 %d OK\r\nConnection: Close\r\n"
                 "content-length:%d\r\n\r\n", status, size);
    write(m_connfd, buf, strlen(buf));
}




void Handler::handle()
{
    char buffer[BUFFER_SIZE];
    int size;
    read:	size = read(m_connfd, buffer, BUFFER_SIZE - 1);
    //printf("%s", buffer);
    if(size > 0)
    {
        char method[5];
        char filename[50];
        int i, j;
        i = j = 0;
        while(buffer[j] != ' ' && buffer[j] != '\0')
        {
            method[i++] = buffer[j++];
        }
        ++j;
        method[i] = '\0';
        i = 0;
        while(buffer[j] != ' ' && buffer[j] != '\0')
        {
            filename[i++] = buffer[j++];
        }
        filename[i] = '\0';

        if(strcasecmp(method, "GET") == 0)  //get method
        {
            response_get(filename);
        }
        else if(strcasecmp(method, "POST") == 0)  //post method
        {
            //printf("Begin\n");
            char argvs[100];
            memset(argvs, 0, sizeof(argvs));
            int k = 0;
            char *ch = NULL;
            ++j;
            while((ch = strstr(argvs, "Content-Length")) == NULL)
            {
                k = 0;
                memset(argvs, 0, sizeof(argvs));
                while(buffer[j] != '\r' && buffer[j] != '\0')
                {
                    argvs[k++] = buffer[j++];
                }
                ++j;
                //printf("%s\n", argvs);
            }
            int length;
            char *str = strchr(argvs, ':');
            //printf("%s\n", str);
            ++str;
            sscanf(str, "%d", &length);
            //printf("length:%d\n", length);
            j = strlen(buffer) - length;
            k = 0;
            memset(argvs, 0, sizeof(argvs));
            while(buffer[j] != '\r' && buffer[j] != '\0')
                argvs[k++] = buffer[j++];

            argvs[k] = '\0';
            //printf("%s\n", argvs);
            response_post(filename, argvs);
        }
        else
        {
            char message[512];
            sprintf(message, "<html><title>Tinyhttpd Error</title>");
            sprintf(message, "%s<body>\r\n", message);
            sprintf(message, "%s 501\r\n", message);
            sprintf(message, "%s <p>%s: Httpd does not implement this method",
                    message, method);
            sprintf(message, "%s<hr><h3>The Tiny Web Server<h3></body>", message);
            response(message, 501);
        }


        //response_error("404");
    }
    else if(size < 0)
        goto read;

    sleep(3);  //wait for client close, avoid TIME_WAIT
    close(m_connfd);
}

void Handler::response_get(char *filename)
{
    char file[1000];
    strcpy(file, path);

    int i = 0;
    bool is_dynamic = false;
    char argv[20];
    //查找是否有？号
    while(filename[i] != '?' && filename[i] != '\0')
        ++i;
    if(filename[i] == '?')
    {	//有?号，则是动态请求
        int j = i;
        ++i;
        int k = 0;
        while(filename[i] != '\0')  //分离参数和文件名
            argv[k++] = filename[i++];
        argv[k] = '\0';
        filename[j] = '\0';
        is_dynamic = true;
    }
    if(strcmp(filename, "/") == 0)

        strcat(file, "/index.html");

    else
        strcat(file, filename);


    //printf("%s\n", file);
    struct stat filestat;
    int ret = stat(file, &filestat);

    if(ret < 0 || S_ISDIR(filestat.st_mode)) //file doesn't exits
    {
        char message[512];
        sprintf(message, "<html><title>Tinyhttpd Error</title>");
        sprintf(message, "%s<body>\r\n", message);
        sprintf(message, "%s 404\r\n", message);
        sprintf(message, "%s <p>GET: Can't find the file", message);
        sprintf(message, "%s<hr><h3>The Tiny Web Server<h3></body>",
                message);
        response(message, 404);
        return;
    }

    if(is_dynamic)
    {
        if(fork() == 0)
            /*创建子进程执行对应的子程序，多线程中，创建子进程，
            只有当前线程会被复制，其他线程就“不见了”，这正符合我们的要求，
            而且fork后执行execl，程序被进程被重新加载*/
        {
            dup2(m_connfd, STDOUT_FILENO);
            //将标准输出重定向到sockfd,将子程序输出内容写到客户端去。
            execl(file, argv); //执行子程序
        }
        wait(NULL);
    }
    else
    {
        int filefd = open(file, O_RDONLY);
        response_file(filestat.st_size, 200);
        //使用“零拷贝”发送文件
        sendfile(m_connfd, filefd, 0, filestat.st_size);
    }
}


void Handler::response_post(char *filename, char *argvs)
{
    char file[100];
    strcpy(file, path);

    strcat(file, filename);

    struct stat filestat;
    int ret = stat(file, &filestat);
    printf("%s\n", file);
    if(ret < 0 || S_ISDIR(filestat.st_mode)) //file doesn't exits
    {
        char message[512];
        sprintf(message, "<html><title>Tinyhttpd Error</title>");
        sprintf(message, "%s<body>\r\n", message);
        sprintf(message, "%s 404\r\n", message);
        sprintf(message, "%s <p>GET: Can't find the file", message);
        sprintf(message, "%s<hr><h3>The Tiny Web Server<h3></body>",
                message);
        response(message, 404);
        return;
    }

    char argv[20];
    int a, b;
    ret = sscanf(argvs, "a=%d&b=%d", &a, &b);
    if(ret < 0 || ret != 2)
    {
        char message[512];
        sprintf(message, "<html><title>Tinyhttpd Error</title>");
        sprintf(message, "%s<body>\r\n", message);
        sprintf(message, "%s 404\r\n", message);
        sprintf(message, "%s <p>GET: Parameter error", message);
        sprintf(message, "%s<hr><h3>The Tiny Web Server<h3></body>",
                message);
        response(message, 404);
        return;
    }
    sprintf(argv, "%d&%d", a, b);
    if(fork() == 0)
        /*创建子进程执行对应的子程序，多线程中，创建子进程，
        只有当前线程会被复制，其他线程就“不见了”，这正符合我们的要求，
        而且fork后执行execl，程序被进程被重新加载*/
    {
        dup2(m_connfd, STDOUT_FILENO);
        //将标准输出重定向到sockfd,将子程序输出内容写到客户端去。
        execl(file, argv); //执行子程序
    }
    wait(NULL);
}
