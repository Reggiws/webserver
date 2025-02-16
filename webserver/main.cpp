#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"

#define MAX_FD 65535           // 最大数量的文件描述符
#define MAX_EVENT_NUMBER 10000 // 最大监听数量

// 添加信号捕捉
void addsig(int sig, void(handler)(int)) // 为啥
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    assert( sigaction( sig, &sa, NULL ) != -1 );
}

// 添加文件描述到epoll里
extern void addfd(int epollfd, int fd, bool one_shot);

// 从epoll中删除文件描述符
extern void removefd(int epollfd, int fd);

// // 修改文件描述符
// extern void modfd(int epollfd, int fd, int ev);

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("请按照以下格式输入 %s port_number\n", basename(argv[0]));
        return 1;
    }

    // 获取端口号
    int port = atoi(argv[1]);

    // 对SIGPIPE信号进行处理
    addsig(SIGPIPE, SIG_IGN);

    threadpool<http_conn> *pool = NULL;
    try
    {
        pool = new threadpool<http_conn>;
    }
    catch (...)
    {
        return 1;
    }

    http_conn *users = new http_conn[MAX_FD]; // 创建一个数组保存所有客户端的信息

    int listenfd = socket(PF_INET, SOCK_STREAM, 0); // 创建监听的socket



    // 绑定
    int ret = 0;
    struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;

    address.sin_port = htons(port);
    // 设置端口复用，需在绑定之前
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));

    // 监听
    ret = listen(listenfd, 5);

    // 创建epoll对象，事件数组，添加
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);

    // 将监听的对象加入到epoll中
    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

    while (1)
    {
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((num < 0) && (errno != EINTR))
        {
            printf("epoll faliure\n");
            break;
        }

        for (int i = 0; i < num; i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(client_address);

                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlen);

                if ( connfd < 0 ) {
                    printf( "errno is: %d\n", errno );
                    continue;
                } 

                if (http_conn::m_user_count > MAX_FD)
                {
                    // 当前连接数满了
                    // 给客户端反回消息
                    close(connfd);
                    continue;
                }
                // 将新客户数据初始化并放入数组中
                users[connfd].init(connfd, client_address);
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) // 发生断联或异常事件
            {
                users[sockfd].close_conn();
            }
            else if(events[i].events & EPOLLIN)
            {
                if(users[sockfd].read())
                {
                    //一次性把数据读完
                    pool->append(users+sockfd);
                } 
                else{
                    users[sockfd].close_conn();
                }
            }
            else if(events[i].events& EPOLLOUT)
            {
                if(!users[sockfd].write()){
                    users[sockfd].close_conn();
                }
                
            }
        }
    }
    close(epollfd);
    close(listenfd);
    delete [] users;
    delete pool;
    return 0;
}