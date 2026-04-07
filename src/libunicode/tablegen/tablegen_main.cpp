/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
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

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

#include "case_norm_generator.h"
#include "enum_generator.h"
#include "multistage_generator.h"
#include "ucd_api_generator.h"
#include "ucd_parser.h"

#if defined(_WIN32)
    #include <Windows.h>
    #include <crtdbg.h>
#endif

namespace
{

/// Suppress Windows GUI dialogs for CRT assertions and errors
/// so that build failures don't block CI with modal popups.
void suppressWindowsDialogs() noexcept
{
#if defined(_WIN32)
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif
}

} // namespace

/// Unified UCD table generator.
///
/// Usage: unicode_tablegen <UCD_DIR> <OUTPUT_DIR> [NAMESPACE]
///
/// Generates all 9 auto-generated source files from Unicode Character Database:
///   - ucd_enums.h, ucd_ostream.h, ucd_fmt.h (enum definitions)
///   - ucd.h, ucd.cpp (range-based lookup tables)
///   - case_normalization_data.h (case mapping + normalization tables)
///   - codepoint_properties_data.h, codepoint_properties_data.cpp (multistage tables)
///   - codepoint_properties_names.cpp (character name tables)
int main(int argc, char const* argv[])
{
    suppressWindowsDialogs();

    if (argc < 3)
    {
        std::cerr << "Usage: unicode_tablegen <UCD_DIR> <OUTPUT_DIR> [NAMESPACE]\n";
        return EXIT_FAILURE;
    }

    auto const ucdDir = std::string(argv[1]);
    auto const outputDir = std::string(argv[2]);
    auto const namespaceName = (argc > 3) ? std::string(argv[3]) : std::string("unicode::precompiled");

    try
    {
        auto const startTime = std::chrono::steady_clock::now();

        std::cout << "[tablegen] Parsing UCD from: " << ucdDir << "\n";
        auto parser = tablegen::UcdParser(ucdDir);
        parser.parseAll();

        std::cout << "[tablegen] Generating enum files...\n";
        tablegen::generateEnumFiles(parser, outputDir);

        std::cout << "[tablegen] Generating UCD API files (ucd.h, ucd.cpp)...\n";
        tablegen::generateUcdApiFiles(parser, outputDir);

        std::cout << "[tablegen] Generating case/normalization tables...\n";
        tablegen::generateCaseNormFile(parser, outputDir);

        std::cout << "[tablegen] Generating multistage property tables...\n";
        tablegen::generateMultistageFiles(parser, outputDir, namespaceName);

        auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
        std::cout << "[tablegen] Done in " << elapsed.count() << " ms.\n";
    }
    catch (std::exception const& e)
    {
        std::cerr << "unicode_tablegen: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
