#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <memory>
#include "utility/types.h"

NS_BEGIN(sk)

class socket;
typedef std::shared_ptr<socket> socket_ptr;

class socket {
public:
    MAKE_NONCOPYABLE(socket);

    static socket_ptr create();
    virtual ~socket();

    int fd() const { return fd_; }

    int connect(const std::string& addr, u16 port);
    int listen(const std::string& addr, u16 port, int backlog);
    socket_ptr accept(std::string& addr, u16& port);

    /**
     * @brief send data to the connected peer
     * @param buf: the data to be sent
     * @param len: the length of the data
     * @return size of sent data, or -1 if there is an error
     * NOTE: it's the caller's duty to handle:
     *       1) error (include may-block condition)
     *       2) partial sending
     */
    ssize_t send(const void *buf, size_t len);

    /**
     * @brief recv some data from the connected peer
     * @param buf: where the data to be stored
     * @param len: the length of the buffer
     * @return size of received data, 0 if EOF reached,
     *         or -1 if there is an error
     */
    ssize_t recv(void *buf, size_t len);

    static int set_reuseaddr(int fd);
    static int set_nodelay(int fd, bool on);
    static int set_cloexec(int fd, bool on);
    static int set_nonblock(int fd, bool on);
    static int set_sndbuf(int fd, int size);
    static int set_rcvbuf(int fd, int size);
    static int set_keepalive(int fd, int idle, int interval, int count);

private:
    int make_socket(int domain, int type, int protocol);

private:
    socket() : fd_(-1) {}

private:
    int fd_;
};

NS_END(sk)

#endif // SOCKET_H