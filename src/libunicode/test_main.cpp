/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2024 Christian Parpart <christian@parpart.family>
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
#include <catch2/catch_session.hpp>

#if defined(_WIN32)
    #include <Windows.h>
#endif

namespace
{

struct setup_teardown
{
#if defined(_WIN32)
    const DWORD stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    const DWORD savedOutputCP = GetConsoleOutputCP();
    const DWORD savedOutputMode = []() -> DWORD {
        DWORD mode;
        GetConsoleMode(stdoutHandle, &mode);
        return mode;
    }();
#endif

    setup_teardown()
    {
#if defined(_WIN32)
        SetConsoleMode(stdoutHandle,
                       savedOutputMode | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT
                           | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        SetConsoleOutputCP(CP_UTF8);
#endif
    }

    ~setup_teardown()
    {
#if defined(_WIN32)
        SetConsoleMode(stdoutHandle, savedOutputMode);
        SetConsoleOutputCP(savedOutputCP);
#endif
    }
};

} // namespace

int main(int argc, char* argv[])
{
    [[maybe_unused]] auto const _ = setup_teardown {};

    int result = Catch::Session().run(argc, argv);

    return result;
}
