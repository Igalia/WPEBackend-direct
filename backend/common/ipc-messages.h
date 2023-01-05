#pragma once

#include "ipc.h"

namespace IPC
{
class FrameAvailable final : public Message
{
  public:
    static constexpr uint64_t MESSAGE_CODE = 1;

    FrameAvailable() : Message(MESSAGE_CODE)
    {
    }
};

class FrameComplete final : public Message
{
  public:
    static constexpr uint64_t MESSAGE_CODE = 2;

    FrameComplete() : Message(MESSAGE_CODE)
    {
    }
};
} // namespace IPC
