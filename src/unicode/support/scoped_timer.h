/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2022 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <chrono>
#include <ostream>
#include <string>

namespace support
{

class scoped_timer
{
  public:
    scoped_timer(std::ostream* output, std::string message):
        _start { std::chrono::steady_clock::now() }, _output { output }, _message { std::move(message) }
    {
        if (_output)
        {
            *_output << _message << " ... ";
            _output->flush();
        }
    }

    ~scoped_timer()
    {
        if (!_output)
            return;

        auto const finish = std::chrono::steady_clock::now();
        auto const diff = finish - _start;
        *_output << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << " ms\n";
    }

  private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::ostream* _output;
    std::string _message;
};

} // namespace support
