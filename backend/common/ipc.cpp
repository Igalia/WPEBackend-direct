#include "ipc.h"

#include <cassert>
#include <sys/socket.h>

using namespace IPC;

Channel::Channel(MessageHandler& handler) noexcept : m_handler(handler)
{
    int sockets[2] = {};
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, sockets) != 0)
    {
        g_critical("Cannot create Unix sockets pair for IPC channel");
        return;
    }

    if (configureLocalSocketFromFd(sockets[0]))
        m_peerFd = sockets[1];
    else
        close(sockets[1]);
}

Channel::Channel(MessageHandler& handler, int peerFd) noexcept : m_handler(handler)
{
    if (peerFd == -1)
    {
        g_critical("Invalid peer file descriptor for IPC channel");
        return;
    }

    configureLocalSocketFromFd(peerFd);
}

Channel::Channel(Channel&& other) noexcept
    : m_handler(other.m_handler), m_localSocket(other.m_localSocket), m_socketSource(other.m_socketSource),
      m_peerFd(other.m_peerFd)
{
    other.m_localSocket = nullptr;
    other.m_socketSource = nullptr;
    other.m_peerFd = -1;
}

Channel& Channel::operator=(Channel&& other) noexcept
{
    m_handler = other.m_handler;
    m_localSocket = other.m_localSocket;
    m_socketSource = other.m_socketSource;
    m_peerFd = other.m_peerFd;

    other.m_localSocket = nullptr;
    other.m_socketSource = nullptr;
    other.m_peerFd = -1;

    return *this;
}

bool Channel::sendMessage(const Message& message) const noexcept
{
    if (m_localSocket)
    {
        gssize len = g_socket_send(m_localSocket, message.exposeAsBuffer(), Message::MESSAGE_SIZE, nullptr, nullptr);
        if (len == Message::MESSAGE_SIZE)
            return true;
    }

    return false;
}

int Channel::detachPeerFd() noexcept
{
    if (m_peerFd == -1)
        return -1;

    int peerFd = dup(m_peerFd);
    close(m_peerFd);
    m_peerFd = -1;

    return peerFd;
}

void Channel::closeChannel() noexcept
{
    if (m_peerFd != -1)
    {
        close(m_peerFd);
        m_peerFd = -1;
    }

    if (m_socketSource)
    {
        g_source_destroy(m_socketSource);
        g_source_unref(m_socketSource);
        m_socketSource = nullptr;
    }

    if (m_localSocket)
    {
        g_object_unref(m_localSocket);
        m_localSocket = nullptr;
    }
}

bool Channel::configureLocalSocketFromFd(int localFd) noexcept
{
    assert(localFd != -1);
    assert(!m_localSocket);
    assert(!m_socketSource);

    m_localSocket = g_socket_new_from_fd(localFd, nullptr);
    if (!m_localSocket)
    {
        close(localFd);
        g_critical("Cannot create local socket for IPC channel");
        return false;
    }

    m_socketSource = g_socket_create_source(m_localSocket, G_IO_IN, nullptr);
    if (!m_socketSource)
    {
        g_object_unref(m_localSocket);
        m_localSocket = nullptr;

        g_critical("Cannot create socket source for IPC channel");
        return false;
    }

    g_source_set_callback(
        m_socketSource, G_SOURCE_FUNC(+[](GSocket* socket, GIOCondition condition, Channel* channel) -> gboolean {
            if ((condition & G_IO_IN) == 0)
                return G_SOURCE_CONTINUE;

            Message msg;
            GError* error = nullptr;
            gssize len = g_socket_receive(socket, msg.exposeAsBuffer(), Message::MESSAGE_SIZE, nullptr, &error);
            if (len == -1)
            {
                channel->m_handler.handleError(*channel, error);
                g_error_free(error);
            }
            else if (len == 0)
            {
                channel->m_handler.handlePeerClosed(*channel);
            }
            else if (len == Message::MESSAGE_SIZE)
            {
                channel->m_handler.handleMessage(*channel, msg);
            }

            return G_SOURCE_CONTINUE;
        }),
        this, nullptr);

    if (!g_source_attach(m_socketSource, g_main_context_get_thread_default()))
    {
        g_source_unref(m_socketSource);
        m_socketSource = nullptr;

        g_object_unref(m_localSocket);
        m_localSocket = nullptr;

        g_critical("Cannot attach socket source for IPC channel");
        return false;
    }

    return true;
}
