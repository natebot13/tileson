//
// Created by robin on 13.01.2021.
//

#include "../external_libs/catch.hpp"

//#define POCKETLZMA_LZMA_C_DEFINE //Already defined once in tests_main
#include "../extras/pocketlzma.hpp"

#include "../src/tileson/external/nlohmann.hpp"
#include "../src/tileson/external/picojson.hpp"
#include "../src/tileson/external/gason.hpp"
#include "../src/tileson/tileson.h"

#include "tson_files_mapper.h"

//Must be defined after Tileson
#include "../src/tileson/json/Gason.hpp"

TEST_CASE( "Run benchmarks on all supported json-backends", "[json][benchmarks]" )
{
    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    fs::path pathLzmaLocal {"content/test-maps/ultimate_test.lzma"};
    fs::path pathLzmaTravis {"../content/test-maps/ultimate_test.lzma"};
    fs::path pathLzmaToUse = (fs::exists(pathLzmaLocal)) ? pathLzmaLocal : pathLzmaTravis;

    tson::Tileson nlohmann {std::make_unique<tson::NlohmannJson>()};
    tson::Tileson picojson {std::make_unique<tson::PicoJson>()};
    tson::Tileson j11 {std::make_unique<tson::Json11>()};

    auto startNlohmann = std::chrono::steady_clock::now();
    auto nlohmannMap = nlohmann.parse(pathToUse);
    auto endNlohmann = std::chrono::steady_clock::now();
    std::chrono::duration<double> msNlohmann = (endNlohmann-startNlohmann) * 1000;

    auto startPicoJson = std::chrono::steady_clock::now();
    auto picojsonMap = picojson.parse(pathToUse);
    auto endPicoJson = std::chrono::steady_clock::now();
    std::chrono::duration<double> msPicoJson = (endPicoJson-startPicoJson) * 1000;

    auto startJ11 = std::chrono::steady_clock::now();
    auto j11Map = j11.parse(pathToUse);
    auto endJ11 = std::chrono::steady_clock::now();
    std::chrono::duration<double> msJ11 = (endJ11-startJ11) * 1000;

    auto startJ11Lzma = std::chrono::steady_clock::now();
    auto j11MapLzma = j11.parse(pathLzmaToUse, std::make_unique<tson::Lzma>());
    auto endJ11Lzma = std::chrono::steady_clock::now();
    std::chrono::duration<double> msJ11Lzma = (endJ11Lzma-startJ11Lzma) * 1000;

    auto startJ11LzmaMemory = std::chrono::steady_clock::now();
    std::unique_ptr<tson::Map> j11MapLzmaMemory = j11.parse(tson_files_mapper::_ULTIMATE_TEST_LZMA, tson_files_mapper::_ULTIMATE_TEST_LZMA_SIZE, std::make_unique<tson::Lzma>());
    auto endJ11LzmaMemory = std::chrono::steady_clock::now();
    std::chrono::duration<double> msJ11LzmaMemory = (endJ11LzmaMemory-startJ11LzmaMemory) * 1000;

    std::cout << "Nlohmann parse time:             " << msNlohmann.count() << " ms\n";
    std::cout << "PicoJson parse time:             " << msPicoJson.count() << " ms\n";
    std::cout << "Json11 parse time:               " << msJ11.count() << " ms\n";
    std::cout << "Json11 parse time (LZMA):        " << msJ11Lzma.count() << " ms\n";
    std::cout << "Json11 memory parse time (LZMA): " << msJ11LzmaMemory.count() << " ms\n";

    REQUIRE(nlohmannMap->getStatus() == tson::ParseStatus::OK);
    REQUIRE(picojsonMap->getStatus() == tson::ParseStatus::OK);
    REQUIRE(j11Map->getStatus() == tson::ParseStatus::OK);
    REQUIRE(j11MapLzma->getStatus() == tson::ParseStatus::OK);
    REQUIRE(j11MapLzmaMemory->getStatus() == tson::ParseStatus::OK);
}

//
// Removed due to Gason being unsupported and thus unknown errors during parsing are expected
//
//TEST_CASE( "Run benchmarks on Gason (unsupported)", "[json][benchmarks]" )
//{
//    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
//    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
//    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;
//
//    fs::path pathLzmaLocal {"content/test-maps/ultimate_test.lzma"};
//    fs::path pathLzmaTravis {"../content/test-maps/ultimate_test.lzma"};
//    fs::path pathLzmaToUse = (fs::exists(pathLzmaLocal)) ? pathLzmaLocal : pathLzmaTravis;
//
//    tson::Tileson g {std::make_unique<tson::Gason>()};
//
//    auto startGason = std::chrono::steady_clock::now();
//    auto gasonMap = g.parse(pathToUse);
//    auto endGason = std::chrono::steady_clock::now();
//    std::chrono::duration<double> msGason = (endGason-startGason) * 1000;
//
//    std::cout << "Gason parse time:                " << msGason.count() << " ms\n";
//
//    REQUIRE(gasonMap->getStatus() == tson::ParseStatus::OK);
//}