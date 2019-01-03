//
// Created by icekylin on 18-12-20.
//

#ifndef DM_NET_H
#define DM_NET_H

#include <event2/event.h>
#include <event2/listener.h>
#include <memory>
#include <cstring>
#include <event.h>

namespace dm {
namespace net {
class net_dispatch {
public:
    net_dispatch(std::uint16_t port) : port_(port) {
        event_base_ = event_base_new();
    }

    void listen() {
        struct sockaddr_in sin;
        std::memset(&sin, 0, sizeof(struct sockaddr_in));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port_);
        listener_ = evconnlistener_new_bind(event_base_, listener_cb, this,
                                          LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
                                          10, (struct sockaddr*)&sin,
                                          sizeof(struct sockaddr_in));

        event_base_dispatch(event_base_);
        evconnlistener_free(listener_);
        event_base_free(event_base_);
    }

protected:
    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg) {
        ((net_dispatch *)arg)->do_listener(fd);
    }

    static void socket_read_cb(struct bufferevent *bev, void *ctx) {
        char msg[4096];

        size_t len = bufferevent_read(bev, msg, sizeof(msg)-1 );

        msg[len] = '\0';
        dlog("server read the data \n%s\n", msg);

        char reply[] = "I has read your data";
        bufferevent_write(bev, reply, strlen(reply) );
    }

    static void socket_event_cb(struct bufferevent *bev, short what, void *ctx) {

    }

    void do_listener(evutil_socket_t fd) {
        bufferevent *bev =  bufferevent_socket_new(event_base_, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, this);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
    }
private:
    std::uint16_t port_;
    struct event_base *event_base_;
    evconnlistener *listener_;
};
}
}
#endif //DM_NET_H
