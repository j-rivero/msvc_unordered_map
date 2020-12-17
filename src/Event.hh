/*
 * Copyright (C) 2016 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef IGNITION_COMMON_EVENT_HH_
#define IGNITION_COMMON_EVENT_HH_

#include <atomic>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <utility>

namespace ignition
{
  namespace common
  {
    /// \class Event Event.hh ignition/common/Event.hh
    /// \brief Base class for all events
    class Event
    {
      /// \brief Constructor
      public: Event();

      /// \brief Destructor
      public: virtual ~Event();

      /// \brief Disconnect
      /// \param[in] _id Integer ID of a connection
      public: virtual void Disconnect(int _id) = 0;

      /// \brief Get whether this event has been signaled.
      /// \return True if the event has been signaled.
      public: bool Signaled() const;

      /// \brief Set whether this event has been signaled.
      /// \param[in] _sig True if the event has been signaled.
      public: void SetSignaled(const bool _sig);

      /// \brief True if the event has been signaled.
      private: bool signaled;
    };

    /// \brief A class that encapsulates a connection.
    class Connection
    {
      /// \brief Constructor.
      /// \param[in] _e Event pointer to connect with.
      /// \param[in] _i Unique id.
      public: Connection(Event *_e, const int _i);

      /// \brief Destructor.
      public: ~Connection();

      /// \brief Get the id of this connection.
      /// \return The id of this connection.
      public: int Id() const;

      /// \brief the event for this connection
      private: Event *event = nullptr;

      /// \brief the id set in the constructor
      private: int id = -1;

#ifdef _WIN32
// Disable warning C4251
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
      /// \brief set during the constructor
      private: std::chrono::time_point<std::chrono::system_clock> creationTime;
#ifdef _WIN32
#pragma warning(pop)
#endif

      /// \brief Friend class.
      public: template<typename T, typename N> friend class EventT;
    };

    /// \brief A class for event processing.
    /// \tparam T function event callback function signature
    /// \tparam N optional additional type to disambiguate events with same
    ///   function signature
    template<typename T, typename N = void>
    class EventT : public Event
    {
      public: using CallbackT = std::function<T>;
      static_assert(std::is_same<typename CallbackT::result_type, void>::value,
          "Event callback must have void return type");

      /// \brief Constructor.
      public: EventT();

      /// \brief Destructor.
      public: virtual ~EventT();

      /// \brief Disconnect a callback to this event.
      /// \param[in] _id The id of the connection to disconnect.
      public: virtual void Disconnect(int _id);

      /// \brief Get the number of connections.
      /// \return Number of connection to this Event.
      public: unsigned int ConnectionCount() const;

      /// \brief Access the signal.
      public: template<typename ... Args>
              void operator()(Args && ... args)
      {
        this->Signal(std::forward<Args>(args)...);
      }

      /// \brief Signal the event for all subscribers.
      public: template <typename ... Args>
              void Signal(Args && ... args)
      {
        this->Cleanup();

        this->SetSignaled(true);
        for (const auto &iter : this->connections)
        {
          if (iter.second->on)
            iter.second->callback(std::forward<Args>(args)...);
        }
      }

      /// \internal
      /// \brief Removes queued connections.
      /// We assume that this function is called from a Signal function.
      private: void Cleanup();

      /// \brief A private helper class used in maintaining connections.
      private: class EventConnection
      {
        /// \brief Constructor
        public: EventConnection(const bool _on, const std::function<T> &_cb)
                : callback(_cb)
        {
          // Windows Visual Studio 2012 does not have atomic_bool constructor,
          // so we have to set "on" using operator=
          this->on = _on;
        }

        /// \brief On/off value for the event callback
        public: std::atomic_bool on;

        /// \brief Callback function
        public: std::function<T> callback;
      };

      /// \def EvtConnectionMap
      /// \brief Event Connection map typedef.
      typedef std::map<int, std::unique_ptr<EventConnection>> EvtConnectionMap;

      /// \brief Array of connection callbacks.
      private: EvtConnectionMap connections;

      /// \brief A thread lock.
      private: std::mutex mutex;

      /// \brief List of connections to remove
      private: std::list<typename EvtConnectionMap::const_iterator>
              connectionsToRemove;
    };

    /// \brief Constructor.
    template<typename T, typename N>
    EventT<T, N>::EventT()
    : Event()
    {
    }

    /// \brief Destructor. Deletes all the associated connections.
    template<typename T, typename N>
    EventT<T, N>::~EventT()
    {
      this->connections.clear();
    }

    /// \brief Get the number of connections.
    /// \return Number of connections.
    template<typename T, typename N>
    unsigned int EventT<T, N>::ConnectionCount() const
    {
      return this->connections.size();
    }

    /// \brief Removes a connection.
    /// \param[in] _id the connection index.
    template<typename T, typename N>
    void EventT<T, N>::Disconnect(int _id)
    {
      // Find the connection
      auto const &it = this->connections.find(_id);

      if (it != this->connections.end())
      {
        it->second->on = false;
        this->connectionsToRemove.push_back(it);
      }
    }

    /////////////////////////////////////////////
    template<typename T, typename N>
    void EventT<T, N>::Cleanup()
    {
      std::lock_guard<std::mutex> lock(this->mutex);
      // Remove all queue connections.
      for (auto &conn : this->connectionsToRemove)
        this->connections.erase(conn);
      this->connectionsToRemove.clear();
    }
  }
}

using namespace ignition;
using namespace common;

//////////////////////////////////////////////////
Event::Event()
  : signaled(false)
{
}

//////////////////////////////////////////////////
Event::~Event()
{
}

//////////////////////////////////////////////////
bool Event::Signaled() const
{
  return this->signaled;
}

//////////////////////////////////////////////////
void Event::SetSignaled(const bool _sig)
{
  this->signaled = _sig;
}

//////////////////////////////////////////////////
Connection::Connection(Event *_e, const int _i)
: event(_e), id(_i)
{
}

//////////////////////////////////////////////////
Connection::~Connection()
{
  if (this->event && this->id >= 0)
  {
    this->event->Disconnect(this->id);
    this->id = -1;
    this->event = nullptr;
  }
}

//////////////////////////////////////////////////
int Connection::Id() const
{
  return this->id;
}
#endif
