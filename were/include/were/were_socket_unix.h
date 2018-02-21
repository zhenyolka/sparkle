#ifndef WERE_SOCKET_H
#define WERE_SOCKET_H

//==================================================================================================

#ifdef __cplusplus

#include "were.h"
#include "were_event_source.h"
#include "were_signal.h"
#include <string>

class WereSocketUnix : public WereEventSource
{
public:
    enum SocketState {
        UnconnectedState,
        ConnectingState,
        ConnectedState,
    };
public:
    ~WereSocketUnix();
    WereSocketUnix(WereEventLoop *loop);
    WereSocketUnix(WereEventLoop *loop, int fd);
    
    void connect(const std::string &path);
    void disconnect();
    
    WereSocketUnix::SocketState state();
    
    int send(const unsigned char *data, unsigned int size);
    int receive(unsigned char *data, unsigned int size);
    int peek(unsigned char *data, unsigned int size);
    
    unsigned int bytesAvailable() const;
    
werethings:
    WereSignal<void ()> signal_connected;
    WereSignal<void ()> signal_disconnected;
    WereSignal<void ()> signal_data;
    
private:
    void event(uint32_t events);
    
private:
    SocketState _state;
};

#endif //__cplusplus

//==================================================================================================

typedef void were_socket_unix_t;

#ifdef __cplusplus
extern "C" {
#endif

were_socket_unix_t *were_socket_unix_create(were_event_loop_t *loop);
void were_socket_unix_destroy(were_socket_unix_t *socket);

void were_socket_unix_connect(were_socket_unix_t *socket, const char *path);
void were_socket_unix_disconnect(were_socket_unix_t *socket);
int were_socket_unix_connected(were_socket_unix_t *socket);

int were_socket_unix_send(were_socket_unix_t *socket, const unsigned *data, unsigned int size);
int were_socket_unix_receive(were_socket_unix_t *socket, unsigned char *data, unsigned int size);
int were_socket_unix_peek(were_socket_unix_t *socket, unsigned char *data, unsigned int size);

unsigned int were_socket_unix_bytes_available(were_socket_unix_t *socket);

void were_socket_unix_set_connection_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user);
void were_socket_unix_set_disconnection_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user);
void were_socket_unix_set_data_callback(were_socket_unix_t *socket, were_event_loop_t *loop, void (*f)(void *user), void *user);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //WERE_SOCKET_H
