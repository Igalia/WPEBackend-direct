#include "ipc.h"

#include <stdio.h>
#include <sys/socket.h>

using namespace ipc;

void ChannelEndPoint::configureEndPoint(int endPointFd) noexcept
{
    if ((endPointFd == -1) || m_endPointSocket || m_socketSource)
        return;

    m_endPointSocket = g_socket_new_from_fd(endPointFd, nullptr);
    if (!m_endPointSocket)
    {
        close(endPointFd);
        printf("Cannot create underlying IPC end-point socket\n");
        return;
    }

    m_socketSource = g_socket_create_source(m_endPointSocket, G_IO_IN, nullptr);
    if (!m_socketSource)
    {
        g_object_unref(m_endPointSocket);
        m_endPointSocket = nullptr;

        printf("Cannot create underlying IPC end-point socket source\n");
        return;
    }

    g_source_set_callback(
        m_socketSource,
        G_SOURCE_FUNC(+[](GSocket* socket, GIOCondition condition, ChannelEndPoint* endPoint) -> gboolean {
            if ((condition & G_IO_IN) == 0)
                return G_SOURCE_CONTINUE;

            Message msg;
            gssize len = g_socket_receive(socket, msg.exposeAsBuffer(), Message::messageSize, nullptr, nullptr);
            if (len == -1)
            {
                printf("Error while receiving IPC message, removing handler\n");
                return G_SOURCE_REMOVE;
            }

            if (len == Message::messageSize)
                endPoint->m_handler.handleMessage(msg);

            return G_SOURCE_CONTINUE;
        }),
        this, nullptr);
    g_source_attach(m_socketSource, g_main_context_get_thread_default());
}

ChannelEndPoint::~ChannelEndPoint()
{
    if (m_socketSource)
    {
        g_source_destroy(m_socketSource);
        g_source_unref(m_socketSource);
        m_socketSource = nullptr;
    }

    if (m_endPointSocket)
    {
        g_object_unref(m_endPointSocket);
        m_endPointSocket = nullptr;
    }
}

bool ChannelEndPoint::sendMessage(const Message& message) const noexcept
{
    if (m_endPointSocket)
    {
        gssize len = g_socket_send(m_endPointSocket, message.exposeAsBuffer(), Message::messageSize, nullptr, nullptr);
        if (len == Message::messageSize)
            return true;
    }

    return false;
}

int ChannelEndPoint::getEndPointFd() const noexcept
{
    if (m_endPointSocket)
        return g_socket_get_fd(m_endPointSocket);

    return -1;
}

Server::Server(MessageHandler& handler) noexcept : ChannelEndPoint(handler)
{
    int sockets[2] = {};
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, sockets) != 0)
    {
        printf("Cannot create underlying Unix sockets pair\n");
        return;
    }

    configureEndPoint(sockets[0]);
    m_clientFd = sockets[1];
}

Server::~Server()
{
    if (m_clientFd != -1)
    {
        close(m_clientFd);
        m_clientFd = -1;
    }
}

int Server::dupClientFd(bool closeSource) noexcept
{
    if (m_clientFd == -1)
        return -1;

    int fd = dup(m_clientFd);
    if (closeSource)
    {
        close(m_clientFd);
        m_clientFd = -1;
    }

    return fd;
}

Client::Client(MessageHandler& handler, int clientFd) noexcept : ChannelEndPoint(handler)
{
    configureEndPoint(clientFd);
}
