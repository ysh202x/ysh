#ifndef __NETWORK_SOCKET_H__
#define __NETWORK_SOCKET_H__


namespace ysh_toolkit
{

class SockNum{

public:
    using Ptr = std::shared_ptr<SockNum>;

    typedef enum {
        Sock_Invalid = -1,
        Sock_TCP = 0,
        Sock_UDP = 1,
        Sock_TCP_Server = 2,
    }SockType;
    
    SockNum(int fd, SockType type) 
    {
        _fd = fd;
        _type = type;
    }

    ~SockNum()
    {
        std::shutdown(_fd, SHUT_RDWR);
        close(_fd);
    }

    int rawFd() const { return _fd; }
    SockType type() const { return _type; }

private:
    int _fd;
    SockType _type;

};

//socket 文件描述符的包装  [AUTO-TRANSLATED:d6705c7a]
//Socket file descriptor wrapper
//在析构时自动溢出监听并close套接字  [AUTO-TRANSLATED:3d9c96d9]
//Automatically overflow listening and close socket when destructing
//防止描述符溢出  [AUTO-TRANSLATED:17c2f2f0]
//Prevent descriptor overflow
class SockFD: public noncopyable{
public:
    using Ptr = std::shared_ptr<SockFD>;
        /**
     * 创建一个fd对象
     * @param num 文件描述符，int数字
     * @param poller 事件监听器
     * Create an fd object
     * @param num File descriptor, int number
     * @param poller Event listener
     */
};


class SockInfo{
    public:
        SockInfo() = default;

        virtual ~SockInfo() = default;

        virtual std::string get_local_ip() = 0;

        //获取端口
        virtual uint16_t get_local_port() = 0;

        //获取远端ip
        virtual std::string get_peer_ip() = 0;

        //获取远端端口
        virtual uint16_t get_peer_port() = 0;

        // 获取标识符
        virtual std::string getIdentifier() const { return ""; }
};

//异步IO Socket对象，包括tcp客户端、服务器和udp套接字  [AUTO-TRANSLATED:8d4fc5c2]
//Asynchronous IO Socket object, including TCP client, server, and UDP socket
class Socket : public std::enable_shared_from_this<Socket> , public noncopyable{
public:
    using Ptr = std::shared_ptr<Socket>;

    //接受数据回调
    using onReadCB = std::function(void(Buffer::Ptr &buf, struct sockaddr *addr, int addr_len));

    //发生错误回调
    using onErrorCB = std::function(void(int err));

    //tcp监听接收到连接请求  [AUTO-TRANSLATED:c4e1b206]
    //TCP listen receives a connection request
    using onAcceptCB = std::function(void(Socket::Ptr &sock));

    //socket发送缓存清空事件，返回true代表下次继续监听该事件，否则停止  [AUTO-TRANSLATED:2dd1c036]
    //Socket send buffer is cleared event, returns true to continue listening for the event next time, otherwise stops
    using onSendBufferEmptyCB = std::function(bool());

       //在接收到连接请求前，拦截Socket默认生成方式  [AUTO-TRANSLATED:2f07f268]
    //Intercept the default generation method of the Socket before receiving a connection request
    using onInterceptCB = std::function(void(Socket::Ptr &sock));
    
};



}
#endif