#ifndef __SERVER_HPP__
#define __SERVER_HPP__
#include "noncopyable.hpp"
#include <ev++.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <map>
#include <stdint.h>

class server;

class connection: private doufu::noncopyable
{
public:
    connection(uint64_t id_, server& s_, int s)
        : id(id_)
        , s(s_)
    {
        fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
        io.set<connection, &connection::callback>(this);
        io.start(s, ev::READ);
    }

    ~connection()
    {
        io.stop();
    }

    void callback(ev::io &watcher, int revents) {
        if (EV_ERROR & revents)
        {
            return;
        }

        if (revents & EV_READ)
        {
            read_cb(watcher);
        }

        if (revents & EV_WRITE)
        {
            write_cb(watcher);
        }

        io.set(ev::READ|ev::WRITE);
    }

    void write_cb(ev::io &watcher)
    {
    }

    void read_cb(ev::io &watcher)
    {
    }
private:
    uint64_t id;
    server& s;
    ev::io io;
};

class server: private doufu::noncopyable
{
public:
    server(int port)
        : conns(0)
    {
        struct sockaddr_in addr;
        s = socket(PF_INET, SOCK_STREAM, 0);

        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            throw std::runtime_error("bind failed");
        }

        fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);

        listen(s, 5);

        io.set<server, &server::on_accept>(this);
        io.start(s, ev::READ);

        sio.set<server, &server::on_signal>(this);
        sio.start(SIGINT);
    }

    ~server()
    {
        shutdown(s, SHUT_RDWR);
        close(s);
    }

    void on_accept(ev::io& io, int revents)
    {
        if(EV_ERROR & revents)
        {
            return;
        }

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_sd = accept(io.fd, (struct sockaddr *)&client_addr,
                &client_len);

        if(client_sd < 0)
        {
            return;
        }

        uint64_t id = conns++;
        if(conn_map.find(id) == conn_map.end())
        {
            conn_map[conns++] = new connection(id, *this, client_sd);
        }
        else
        {
            close(client_sd);
        }
    }

    void on_signal(ev::sig& signal, int revents)
    {
        signal.loop.break_loop();
    }
private:
    int s;
    uint64_t conns;
    ev::io io;
    ev::sig sio;
    typedef std::map<uint64_t, connection*> map;
    map conn_map;
};

#endif

