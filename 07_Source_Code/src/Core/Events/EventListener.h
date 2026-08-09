#pragma once

/**
 * @file EventListener.h
 */

#include "Event.h"

namespace velnix::core
{

class EventListener
{
public:

    virtual ~EventListener() = default;

    virtual void OnEvent(const Event& event) = 0;
};

}