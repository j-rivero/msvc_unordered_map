/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#ifndef IGNITION_GAZEBO_EVENTMANAGER_HH_
#define IGNITION_GAZEBO_EVENTMANAGER_HH_

#include <functional>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#include <ignition/common/Event.hh>

namespace ignition
{
  namespace gazebo
  {
    class EventManager
    {
      /// \brief Constructor
      public: EventManager() = default;

      /// \brief Destructor
      public: ~EventManager() = default;

      /// \brief Convenience type for storing typeinfo references.
      private: using TypeInfoRef = std::reference_wrapper<const std::type_info>;

      /// \brief Hash functor for TypeInfoRef
      private: struct Hasher
               {
                 std::size_t operator()(TypeInfoRef _code) const
                 {
                   return _code.get().hash_code();
                 }
               };

      /// \brief Equality functor for TypeInfoRef
      private: struct EqualTo
               {
                 bool operator()(TypeInfoRef _lhs, TypeInfoRef _rhs) const
                 {
                   return _lhs.get() == _rhs.get();
                 }
               };

      /// \brief Container of used signals.
      private: std::unordered_map<TypeInfoRef,
                                  std::unique_ptr<ignition::common::Event>,
                                  Hasher, EqualTo> events;
    };
  }
}

#endif  // IGNITION_GAZEBO_EVENTMANAGER_HH_
