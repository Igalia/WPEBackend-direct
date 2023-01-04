#pragma once

#include <cstdint>
#include <gio/gio.h>

namespace IPC
{
class Message
{
  public:
    static constexpr size_t MESSAGE_SIZE = 32;
    static constexpr size_t PAYLOAD_SIZE = MESSAGE_SIZE - sizeof(uint64_t);

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
    uint8_t m_payload[PAYLOAD_SIZE] = {};
};
static_assert(sizeof(Message) == Message::MESSAGE_SIZE, "IPC Message class size is wrong");

class MessageHandler;
class Channel
{
  public:
    Channel(MessageHandler& handler) noexcept;
    Channel(MessageHandler& handler, int peerFd) noexcept;

    Channel(Channel&& other) noexcept;
    Channel& operator=(Channel&& other) noexcept;

    virtual ~Channel()
    {
        closeChannel();
    }

    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    bool sendMessage(const Message& message) const noexcept;
    int detachPeerFd() noexcept;
    void closeChannel() noexcept;

  private:
    bool configureLocalSocketFromFd(int localFd) noexcept;

    MessageHandler& m_handler;
    GSocket* m_localSocket = nullptr;
    GSource* m_socketSource = nullptr;
    int m_peerFd = -1;
};

class MessageHandler
{
  public:
    virtual ~MessageHandler() = default;

    virtual void handleMessage(Channel& channel, const Message& message) noexcept = 0;

    virtual void handleError(Channel& channel, const GError* /*error*/) noexcept
    {
        channel.closeChannel();
    }

    virtual void handlePeerClosed(Channel& channel) noexcept
    {
        channel.closeChannel();
    }
};
} // namespace IPC
