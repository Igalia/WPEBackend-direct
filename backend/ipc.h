#pragma once

#include <cstdint>
#include <gio/gio.h>

namespace ipc
{
class Message
{
  public:
    static constexpr size_t messageSize = 32;
    static constexpr size_t payloadSize = messageSize - sizeof(uint64_t);

    Message() noexcept = default;
    Message(uint64_t code) noexcept : m_code(code)
    {
    }

    uint64_t getCode() const noexcept
    {
        return m_code;
    }

    const uint8_t* getPayload() const noexcept
    {
        return m_payload;
    }

    uint8_t* getPayload() noexcept
    {
        return m_payload;
    }

    const char* exposeAsBuffer() const noexcept
    {
        return reinterpret_cast<const char*>(this);
    }

    char* exposeAsBuffer() noexcept
    {
        return reinterpret_cast<char*>(this);
    }

  private:
    uint64_t m_code = 0;
    uint8_t m_payload[payloadSize] = {};
};
static_assert(sizeof(Message) == Message::messageSize, "IPC Message class size is wrong");

class MessageHandler
{
  public:
    virtual ~MessageHandler() = default;

    virtual void handleMessage(const Message& message) noexcept = 0;
};

class ChannelEndPoint
{
  public:
    virtual ~ChannelEndPoint();

    ChannelEndPoint(ChannelEndPoint&&) = delete;
    ChannelEndPoint& operator=(ChannelEndPoint&&) = delete;
    ChannelEndPoint(const ChannelEndPoint&) = delete;
    ChannelEndPoint& operator=(const ChannelEndPoint&) = delete;

    bool sendMessage(const Message& message) const noexcept;
    int getEndPointFd() const noexcept;

  protected:
    ChannelEndPoint(MessageHandler& handler) noexcept : m_handler(handler)
    {
    }

    void configureEndPoint(int endPointFd) noexcept;

  private:
    MessageHandler& m_handler;
    GSocket* m_endPointSocket = nullptr;
    GSource* m_socketSource = nullptr;
};

class Server : public ChannelEndPoint
{
  public:
    Server(MessageHandler& handler) noexcept;
    virtual ~Server();

    int dupClientFd(bool closeSource = false) noexcept;

  private:
    int m_clientFd = -1;
};

class Client : public ChannelEndPoint
{
  public:
    Client(MessageHandler& handler, int clientFd) noexcept;
};
} // namespace ipc
