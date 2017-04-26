#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "net/tcp_connection.h"

NS_BEGIN(sk)
NS_BEGIN(net)

class tcp_client {
public:
    MAKE_NONCOPYABLE(tcp_client);
    typedef std::function<void(int, const tcp_connection_ptr&)> fn_on_connection;
    typedef std::function<void(int, const tcp_connection_ptr&, buffer*)> fn_on_read;
    typedef std::function<void(int, const tcp_connection_ptr&)> fn_on_write;

    tcp_client(reactor *r, const std::string& host, u16 port, const fn_on_connection& fn);
    ~tcp_client();

    int connect();

    void on_read_event (const fn_on_read&  fn) { fn_on_read_  = fn; }
    void on_write_event(const fn_on_write& fn) { fn_on_write_ = fn; }

private:
    void remove_connection(const tcp_connection_ptr& conn);
    void on_connect();

private:
    enum state { state_connecting, state_connected, state_disconnected };

    state state_;
    reactor *reactor_;
    inet_address addr_;
    socket_ptr socket_;
    tcp_connection_ptr connection_;
    fn_on_connection fn_on_connection_;
    handler_ptr handler_;

    // optional callbacks
    fn_on_read fn_on_read_;
    fn_on_write fn_on_write_;
};

NS_END(net)
NS_END(sk)

#endif // TCP_CLIENT_H