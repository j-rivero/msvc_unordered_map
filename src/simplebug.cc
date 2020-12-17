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

#include <functional>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#ifdef _WIN32
#define VISIBLE __declspec(dllexport)
#else
#define VISIBLE 
#endif

class VISIBLE EventManager
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
                              std::unique_ptr<std::string>,
                              Hasher, EqualTo> events;
};

int main()
{
    EventManager e;
    return 0;
}
