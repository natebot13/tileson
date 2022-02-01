//
// Created by robin on 31.07.2019.
//

#define CATCH_CONFIG_MAIN
#include "../external_libs/catch.hpp"

#include "../TilesonConfig.h"

//#define TILESON_UNIT_TEST_USE_SINGLE_HEADER

#include "../include/external/nlohmann.hpp"
#include "../include/external/picojson.hpp"

#define POCKETLZMA_LZMA_C_DEFINE
#include "../extras/pocketlzma.hpp"

#ifdef TILESON_UNIT_TEST_USE_SINGLE_HEADER
    #include "../single_include/tileson.hpp"
#else
    #include "../include/tileson.h"
#endif

#include "tson_files_mapper.h"
#include "../TilesonConfig.h"

#include <memory>
#include <map>
#include <functional>


//#include "../include/json/NlohmannJson.hpp"

/*!
 *
 * @param map The parsed map to assert.
 * @param isOldMap Set to true if map is before Tiled v1.5 - false otherwise
 */
void performMainAsserts(tson::Map *map, bool isOldMap = true)
{
    std::map<int, bool> tests;
    auto main = map->getLayer("Main Layer");
    auto tileData = main->getTileData();
    tson::Rect rect = main->getTileData(8,14)->getDrawingRect();

    REQUIRE(map->getLayers().size() == 6);
    REQUIRE(!map->isInfinite());
    REQUIRE(map->getSize() == tson::Vector2i(32, 16));
    REQUIRE(map->getBackgroundColor() == tson::Colori("#3288c1"));
    REQUIRE(map->getLayers()[0].getProperties().getSize() > 0);
    REQUIRE(map->getLayers()[0].getProperties().get()[0]->getType() != tson::Type::Undefined);
    REQUIRE(map->getLayers()[2].getName() == "Object Layer");
    REQUIRE(map->getLayers()[2].getObjects().size() > 1);
    REQUIRE(map->getLayers()[2].getObjects()[0].getName() == "coin");
    REQUIRE(map->getLayers()[2].getObjects()[0].getProperties().getSize() > 0);
    REQUIRE(map->getLayer("Main Layer") != nullptr);
    REQUIRE(map->getLayer("Main Layer")->getType() == tson::LayerType::TileLayer);
    REQUIRE(map->getLayer("Background Image")->getType() == tson::LayerType::ImageLayer);
    REQUIRE(map->getLayer("Background Image")->get<float>("scroll_speed") == 1.f);
    REQUIRE(map->getLayer("Background Image")->getProp("repeat_bg")->getType() == tson::Type::Boolean);
    REQUIRE(map->getLayer("Object Layer")->getType() == tson::LayerType::ObjectGroup);
    REQUIRE(map->getLayer("Object Layer")->firstObj("coin") != nullptr);
    REQUIRE(map->getLayer("Object Layer")->getObjectsByName("goomba").size() == 2);
    REQUIRE(!map->getLayer("Object Layer")->getObjectsByType(tson::ObjectType::Object).empty());
    REQUIRE(map->getLayer("Object Layer")->getObj(2)->getName() == "coin");
    REQUIRE(map->getTileset("demo-tileset") != nullptr);
    REQUIRE(map->getTileset("demo-tileset")->getTile(36) != nullptr);
    REQUIRE(map->getTileset("demo-tileset")->getTile(36)->getAnimation().size() == 2);
    if(isOldMap)
    {
        REQUIRE(map->getTileset("demo-tileset")->getTerrain("test_terrain")->getProperties().getSize() == 2);
        REQUIRE(map->getTileset("demo-tileset")->getTerrain("test_terrain")->getProp("i_like_this")->getType() == tson::Type::Boolean);
        REQUIRE(!map->getTileset("demo-tileset")->getTerrain("test_terrain")->get<std::string>("description").empty());
    }
    REQUIRE(!tileData[{4,4}]->getAnimation().any());
    REQUIRE(map->getTileMap().size() > 10);
    REQUIRE(tileData[{4,4}] != nullptr);
    REQUIRE(tileData[{4,4}]->getId() == 1);
    REQUIRE(tileData[{5,4}] != nullptr);
    REQUIRE(tileData[{5,4}]->getId() == 3);
    REQUIRE(main->getTileData(8,14) != nullptr);
    REQUIRE(main->getTileData(8,14)->getId() == 2);
    REQUIRE(main->getTileData(17,5) != nullptr);
    REQUIRE(main->getTileData(17,5)->getId() == 5);

    //v1.2.0-tests
    REQUIRE(main->getTileData(8,14)->getPositionInTileUnits({8,14}) == tson::Vector2i(8, 14));
    REQUIRE(main->getTileData(8,14)->getPosition({8,14}) == tson::Vector2f(8.f * 16.f, 14.f * 16.f));
    REQUIRE(main->getMap() != nullptr);
    REQUIRE(main->getMap() == map);
    REQUIRE(main->getTileData(8,14)->getMap() != nullptr);
    REQUIRE(main->getTileData(8,14)->getMap() == map);
    REQUIRE(main->getTileData(8,14)->getTileset() != nullptr);
    REQUIRE(main->getTileData(8,14)->getPosition({8,14}) == main->getTileObject(8,14)->getPosition());
    REQUIRE(main->getTileData().size() == main->getTileObjects().size());

    //v1.3.0
    if(isOldMap)
    {
        REQUIRE(!map->getTileset("demo-tileset")->getTransformations().allowHflip());
        REQUIRE(!map->getTileset("demo-tileset")->getTransformations().allowVflip());
        REQUIRE(!map->getTileset("demo-tileset")->getTransformations().allowPreferuntransformed());
        REQUIRE(!map->getTileset("demo-tileset")->getTransformations().allowRotation());
    }
    else
    {
        REQUIRE(map->getTileset("demo-tileset")->getTransformations().allowHflip());
        REQUIRE(map->getTileset("demo-tileset")->getTransformations().allowVflip());
        REQUIRE(map->getTileset("demo-tileset")->getTransformations().allowPreferuntransformed());
        REQUIRE(map->getTileset("demo-tileset")->getTransformations().allowRotation());
    }
}

/*!
 * Asserts related to Tiled v1.5
 * @param map
 */
void performAssertsOnTiled15Changes(tson::Map *map)
{
    auto *bgLayer = map->getLayer("Background Image");
    REQUIRE(bgLayer->getParallax().x > 1.39f);
    REQUIRE(bgLayer->getParallax().y > 1.01f);

    tson::WangSet *nonexisting = map->getTileset("demo-tileset")->getWangset("non-existing");
    tson::WangSet *terrainWang = map->getTileset("demo-tileset")->getWangset("Terrains");

    REQUIRE(nonexisting == nullptr);
    REQUIRE(terrainWang != nullptr);

    tson::WangColor *nonexistingcolor = terrainWang->getColor("nonexisting");
    tson::WangColor *color = terrainWang->getColor("test_terrain");

    REQUIRE(nonexistingcolor == nullptr);
    REQUIRE(color != nullptr);

    REQUIRE(color->getProperties().getSize() == 2);
    REQUIRE(color->getProp("i_like_this")->getType() == tson::Type::Boolean);
    REQUIRE(!color->get<std::string>("description").empty());

    //Data previously retrieved like this:
    //REQUIRE(map->getTileset("demo-tileset")->getTerrain("test_terrain")->getProperties().getSize() == 2);
    //REQUIRE(map->getTileset("demo-tileset")->getTerrain("test_terrain")->getProp("i_like_this")->getType() == tson::Type::Boolean);
    //REQUIRE(!map->getTileset("demo-tiles")->getTerrain("test_terrain")->get<std::string>("description").empty());
}

void checkChangesAfterTiledVersion124(tson::Map *map)
{
    REQUIRE(map->getLayer("Front Layer")->getTintColor() == tson::Colori(255, 0, 0, 255));
}

TEST_CASE( "Nlohmann - Parse a whole map by file", "[complete][parse][file]" )
{
    tson::Tileson t{std::make_unique<tson::NlohmannJson>()};

    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
        checkChangesAfterTiledVersion124(map.get());
        //Just check the colors here
        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
        REQUIRE(color.r == 254);
        REQUIRE(color.g == 254);
        REQUIRE(color.b == 254);
        REQUIRE(color.a == 255);
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

TEST_CASE( "PicoJson - Parse a whole map by file", "[complete][parse][file]" )
{
    tson::Tileson t{std::make_unique<tson::PicoJson>()};

    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
        checkChangesAfterTiledVersion124(map.get());
        //Just check the colors here
        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
        REQUIRE(color.r == 254);
        REQUIRE(color.g == 254);
        REQUIRE(color.b == 254);
        REQUIRE(color.a == 255);
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

//TEST_CASE( "Gason - Parse a whole map by file", "[complete][parse][file]" )
//{
//    tson::Tileson t{std::make_unique<tson::Gason>()};
//
//    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
//    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
//    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;
//
//    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
//    if(map->getStatus() == tson::ParseStatus::OK)
//    {
//        performMainAsserts(map.get());
//        checkChangesAfterTiledVersion124(map.get());
//        //Just check the colors here
//        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
//        REQUIRE(color.r == 254);
//        REQUIRE(color.g == 254);
//        REQUIRE(color.b == 254);
//        REQUIRE(color.a == 255);
//    }
//    else
//    {
//        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
//        REQUIRE(true);
//    }
//}

TEST_CASE( "Parse a whole map by file", "[complete][parse][file]" )
{
    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
        checkChangesAfterTiledVersion124(map.get());
        //Just check the colors here
        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
        REQUIRE(color.r == 254);
        REQUIRE(color.g == 254);
        REQUIRE(color.b == 254);
        REQUIRE(color.a == 255);
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

TEST_CASE( "Parse a Tiled v1.5 map with external tileset by file - Expect no errors and correct data", "[complete][parse][file]" )
{
    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/ultimate_test_v1.5.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test_v1.5.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get(), false);
        checkChangesAfterTiledVersion124(map.get());
        performAssertsOnTiled15Changes(map.get());
        //Just check the colors here
        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
        REQUIRE(color.r == 254);
        REQUIRE(color.g == 254);
        REQUIRE(color.b == 254);
        REQUIRE(color.a == 255);
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

TEST_CASE( "Parse a whole COMPRESSED map by file", "[complete][parse][file][compression]" )
{
    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/ultimate_test.lzma"};
    fs::path pathTravis {"../content/test-maps/ultimate_test.lzma"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse}, std::make_unique<tson::Lzma>());
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
        checkChangesAfterTiledVersion124(map.get());
        //Just check the colors here
        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
        REQUIRE(color.r == 254);
        REQUIRE(color.g == 254);
        REQUIRE(color.b == 254);
        REQUIRE(color.a == 255);
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

TEST_CASE( "Parse a whole COMPRESSED map by memory", "[complete][parse][file][compression]" )
{
    tson::Tileson t;

    std::unique_ptr<tson::Map> map = t.parse(tson_files_mapper::_ULTIMATE_TEST_LZMA, tson_files_mapper::_ULTIMATE_TEST_LZMA_SIZE, std::make_unique<tson::Lzma>());
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
        checkChangesAfterTiledVersion124(map.get());
        //Just check the colors here
        tson::Colori color = map->getLayer("Object Layer")->firstObj("text")->getText().color;
        REQUIRE(color.r == 254);
        REQUIRE(color.g == 254);
        REQUIRE(color.b == 254);
        REQUIRE(color.a == 255);
    }
}

TEST_CASE( "Parse a whole map by file - minimal", "[complete][parse][file]" )
{
    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/ultimate_test_min.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test_min.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

TEST_CASE( "Test set", "[set]" )
{
    std::set<uint32_t> s;
    s.insert(34);
    s.insert(12);
    s.insert(93);
    s.insert(34); //Should be ignored
    s.insert(34); //Should be ignored

    REQUIRE(s.size() == 3);
}

TEST_CASE( "Parse map - expect correct flip flags", "[parse][file][flip]" )
{
    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/ultimate_test.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        tson::Object *obj_ver_flip = map->getLayer("Object Layer")->firstObj("mario_ver_flip");
        tson::Object *obj_hor_flip = map->getLayer("Object Layer")->firstObj("mario_hor_flip");
        tson::Object *obj_all_flip = map->getLayer("Object Layer")->firstObj("mario_all_flipped");
        tson::Object *obj_no_flip = map->getLayer("Object Layer")->firstObj("mario_no_flip");

        tson::Tile *tile_ver_flip = map->getLayer("Main Layer")->getTileData(28, 15);
        tson::Tile *tile_hor_flip = map->getLayer("Main Layer")->getTileData(28, 14);
        tson::Tile *tile_diagvert_flip = map->getLayer("Main Layer")->getTileData(29, 15);
        tson::Tile *tile_diaghori_flip = map->getLayer("Main Layer")->getTileData(29, 14);
        tson::Tile *tile_no_flip = map->getLayer("Main Layer")->getTileData(25, 14);

        tson::TileObject *tileobj_hor_flip = map->getLayer("Main Layer")->getTileObject(28, 14);
        tson::TileObject *tileobj_diagvert_flip = map->getLayer("Main Layer")->getTileObject(29, 15);

        //Objects
        REQUIRE(obj_ver_flip != nullptr);
        REQUIRE(obj_ver_flip->hasFlipFlags(tson::TileFlipFlags::Vertically));
        REQUIRE(obj_hor_flip != nullptr);
        REQUIRE(obj_hor_flip->hasFlipFlags(tson::TileFlipFlags::Horizontally));
        REQUIRE(obj_all_flip != nullptr);
        REQUIRE(obj_all_flip->hasFlipFlags(tson::TileFlipFlags::Vertically | tson::TileFlipFlags::Horizontally));
        REQUIRE(obj_no_flip != nullptr);
        REQUIRE(obj_no_flip->hasFlipFlags(tson::TileFlipFlags::None));

        //Tiles
        REQUIRE(tile_ver_flip != nullptr);
        REQUIRE(tile_ver_flip->get<std::string>("name") == "cloudyboy");
        REQUIRE(tile_ver_flip->hasFlipFlags(tson::TileFlipFlags::Vertically));
        REQUIRE(tile_ver_flip->getId() == 3221225484); //Id including the flags
        REQUIRE(tile_ver_flip->getGid() == 12); //Id of the tile

        REQUIRE(tile_hor_flip != nullptr);
        REQUIRE(tile_hor_flip->get<std::string>("name") == "cloudyboy");
        REQUIRE(tile_hor_flip->hasFlipFlags(tson::TileFlipFlags::Horizontally));
        REQUIRE(tile_diagvert_flip != nullptr);
        REQUIRE(tile_diagvert_flip->get<std::string>("name") == "cloudyboy");
        REQUIRE(tile_diagvert_flip->hasFlipFlags(tson::TileFlipFlags::Vertically | tson::TileFlipFlags::Diagonally));
        REQUIRE(tile_diaghori_flip != nullptr);
        REQUIRE(tile_diaghori_flip->get<std::string>("name") == "cloudyboy");
        REQUIRE(tile_diaghori_flip->hasFlipFlags(tson::TileFlipFlags::Horizontally | tson::TileFlipFlags::Diagonally));
        REQUIRE(tile_no_flip != nullptr);
        REQUIRE(tile_no_flip->get<std::string>("name") == "cloudyboy");
        REQUIRE(tile_no_flip->hasFlipFlags(tson::TileFlipFlags::None));

        //TileObjects
        auto rect = tileobj_hor_flip->getDrawingRect();
        REQUIRE(tileobj_hor_flip != nullptr);
        REQUIRE(tileobj_hor_flip->getTile() != nullptr);
        REQUIRE(tileobj_hor_flip->getPositionInTileUnits() == tson::Vector2i(28, 14));
        REQUIRE(tileobj_hor_flip->getPosition() == tson::Vector2f(448, 224));
        REQUIRE(tileobj_hor_flip->getTile()->get<std::string>("name") == "cloudyboy");
        REQUIRE(tileobj_hor_flip->getTile()->hasFlipFlags(tson::TileFlipFlags::Horizontally));
        REQUIRE(tileobj_diagvert_flip != nullptr);
        REQUIRE(tileobj_diagvert_flip->getTile() != nullptr);
        REQUIRE(tileobj_diagvert_flip->getPositionInTileUnits() == tson::Vector2i(29, 15));
        REQUIRE(tileobj_diagvert_flip->getPosition() == tson::Vector2f(464, 240));
        REQUIRE(tileobj_diagvert_flip->getTile()->get<std::string>("name") == "cloudyboy");
        REQUIRE(tileobj_diagvert_flip->getTile()->hasFlipFlags(tson::TileFlipFlags::Vertically | tson::TileFlipFlags::Diagonally));
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }
}

TEST_CASE( "Parse a whole map with base64 data by file", "[complete][parse][file][base64]" )
{

    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/ultimate_test_base64.json"};
    fs::path pathTravis {"../content/test-maps/ultimate_test_base64.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    if(map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }


}

TEST_CASE( "Parse a whole map by memory", "[complete][parse][memory]" )
{
    tson::Tileson t;
    std::unique_ptr<tson::Map> map = t.parse(tson_files::_ULTIMATE_TEST_JSON, tson_files::_ULTIMATE_TEST_JSON_SIZE);
    if (map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
    }
    else
    {
        std::cout << "Memory parse error - " << map->getStatusMessage() << std::endl;
        //REQUIRE(true);
        FAIL("Unexpected memory read failure!");
    }
}

//TEST_CASE( "Gason - Parse a whole map by memory", "[complete][parse][memory]" )
//{
//    tson::Tileson t {std::make_unique<tson::Gason>()};
//    std::unique_ptr<tson::Map> map = t.parse(tson_files::_ULTIMATE_TEST_JSON, tson_files::_ULTIMATE_TEST_JSON_SIZE);
//    if (map->getStatus() == tson::ParseStatus::OK)
//    {
//        performMainAsserts(map.get());
//    }
//    else
//    {
//        std::cout << "Memory parse error - " << map->getStatusMessage() << std::endl;
//        //REQUIRE(true);
//        FAIL("Unexpected memory read failure!");
//    }
//}

TEST_CASE( "Parse a whole base64 encoded map by memory", "[complete][parse][memory][base64]" )
{
    tson::Tileson t;
    std::unique_ptr<tson::Map> map = t.parse(tson_files::_ULTIMATE_TEST_BASE64_JSON, tson_files::_ULTIMATE_TEST_BASE64_JSON_SIZE);
    if (map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
    }
    else
    {
        std::cout << "Memory parse error - " << map->getStatusMessage() << std::endl;
        //REQUIRE(true);
        FAIL("Unexpected memory read failure!");
    }
}

TEST_CASE( "Parse a simple map by memory - tiles without any properties (issue #4)", "[simple][parse][memory]" )
{
    tson::Tileson t;
    std::unique_ptr<tson::Map> map = t.parse(tson_files_mapper::_SIMPLE_MAP_JSON, tson_files_mapper::_SIMPLE_MAP_JSON_SIZE);
    if (map->getStatus() == tson::ParseStatus::OK)
    {
        auto main = map->getLayer("simple_layer");
        auto tileset = map->getTilesets()[0];
        auto tileData = main->getTileData();
        bool result = (tileData.size() > 16 && main->getTileData(0,0) != nullptr && tileset.getTiles().size() == 48);
        REQUIRE(result);
    }
    else
    {
        std::cout << "Memory parse error - " << map->getStatusMessage() << std::endl;
        //REQUIRE(true);
        FAIL("Unexpected memory read failure!");
    }
}

TEST_CASE( "Parse a minimal version of whole map by memory", "[complete][parse][memory]" )
{
    tson::Tileson t;
    std::unique_ptr<tson::Map> map = t.parse(tson_files::_ULTIMATE_TEST_MIN_JSON, tson_files::_ULTIMATE_TEST_MIN_JSON_SIZE);
    if (map->getStatus() == tson::ParseStatus::OK)
    {
        performMainAsserts(map.get());
    }
    else
    {
        std::cout << "Memory parse error - " << map->getStatusMessage() << std::endl;
        //REQUIRE(true);
        FAIL("Unexpected memory read failure!");
    }
}

TEST_CASE( "Parse map3.json - expect correct tileset data for all TileObjects", "[complete][parse][map3]" )
{
    tson::Tileson t;

    fs::path pathLocal {"content/test-maps/project/maps/map3.json"};
    fs::path pathTravis {"../content/test-maps/project/maps/map3.json"};
    fs::path pathToUse = (fs::exists(pathLocal)) ? pathLocal : pathTravis;

    std::unique_ptr<tson::Map> map = t.parse({pathToUse});
    size_t numberOfEmptyTilesets = 0;
    size_t numberOfOkTilesets = 0;
    if (map->getStatus() == tson::ParseStatus::OK)
    {
        for(auto &layer : map->getLayers())
        {
            if(layer.getType() == tson::LayerType::TileLayer)
            {
                for (auto& [pos, tileObject] : layer.getTileObjects())
                {
                    tson::Tileset *tileset = tileObject.getTile()->getTileset();
                    tson::Rect drawingRect = tileObject.getDrawingRect();
                    tson::Vector2f position = tileObject.getPosition();
                    REQUIRE(tileset != nullptr);
                    //fs::path p = pathToUse.parent_path() / tileset->getImage();
                    //if (!fs::is_regular_file(p))
                    //    ++numberOfEmptyTilesets;
                    if (tileset->getColumns() < 0)
                        ++numberOfEmptyTilesets;
                    else
                        ++numberOfOkTilesets;
                }
            }
        }
        tson::Layer *layer = map->getLayer("Main Layer");
        tson::Tile *tile = layer->getTileData(3, 0);
        REQUIRE(tile->getTileset()->getName() == "demo-tileset");
    }
    else
    {
        std::cout << "Ignored - " << map->getStatusMessage() << std::endl;
        REQUIRE(true);
    }

    REQUIRE(numberOfEmptyTilesets == 0);
}



TEST_CASE( "Go through demo code - get success", "[demo]" )
{
    tson::Tileson t;
    std::unique_ptr<tson::Map> map = t.parse(tson_files::_ULTIMATE_TEST_JSON, tson_files::_ULTIMATE_TEST_JSON_SIZE);

    if(map->getStatus() == tson::ParseStatus::OK)
    {
        //Get color as an rgba color object
        tson::Colori bgColor = map->getBackgroundColor(); //RGBA with 0-255 on each channel

        //This color can be compared with Tiled hex string
        if (bgColor == "#ffaa07")
            printf("Cool!");

        //Or you can compare them with other colors
        tson::Colori otherColor{255, 170, 7, 255};
        if (bgColor == otherColor)
            printf("This works, too!");

        //You can also get the color as float, transforming values if they are already in their int form, from max 255 to 1.f
        tson::Colorf bgColorFloat = bgColor.asFloat();

        //Or the other way around
        tson::Colori newBg = bgColorFloat.asInt();

        //You can loop through every container of objects
        for (auto &layer : map->getLayers())
        {
            if (layer.getType() == tson::LayerType::ObjectGroup)
            {
                for (auto &obj : layer.getObjects())
                {
                    //Just iterate through all the objects
                }
                //Or use these queries:

                //Gets the first object it finds with the name specified
                tson::Object *player = layer.firstObj("player"); //Does not exist in demo map->..

                //Gets all objects with a matching name
                std::vector<tson::Object> enemies = layer.getObjectsByName("goomba"); //But we got two of those

                //Gets all objects of a specific type
                std::vector<tson::Object> objects = layer.getObjectsByType(tson::ObjectType::Object);

                //Gets an unique object by its name.
                tson::Object *uniqueObj = layer.getObj(2);
            }
        }

        //Or get a specific object if you know its name (or id)
        tson::Layer *layer = map->getLayer("Main Layer");
        tson::Tileset *tileset = map->getTileset("demo-tileset");
        tson::Tile *tile = tileset->getTile(1); //Tileson tile-IDs starts with 1, to be consistent
        // with IDs in data lists. This is in other words the
        //first tile.

        //For tile layers, you can get the tiles presented as a 2D map by calling getTileData()
        //Using x and y positions in tile units.
        if(layer->getType() == tson::LayerType::TileLayer)
        {
            //When the map is of a fixed size, you can get the tiles like this
            if(!map->isInfinite())
            {
                std::map<std::tuple<int, int>, tson::Tile *> tileData = layer->getTileData();

                //Unsafe way to get a tile
                tson::Tile *invalidTile = tileData[{0, 4}]; // x:0,  y:4  - There is no tile here, so this will be nullptr.
                                                                   // Be careful with this, as it expands the map with an ID of {0,4} pointing
                                                                   // to a nullptr...

                //Individual tiles should be retrieved by calling the safe version:
                tson::Tile *safeInvalid = layer->getTileData(0, 5); //Another non-existent tile, but with safety check.
                                                                         //Will not expand the map with a nullptr

                tson::Tile *tile1 = layer->getTileData(4, 4);       //x:4,  y:4  - Points to tile with ID 1 (Tiled internal ID: 0)
                tson::Tile *tile2 = layer->getTileData(5, 4);       //x:5,  y:4  - Points to tile with ID 3 (Tiled internal ID: 2)
                tson::Tile *tile3 = layer->getTileData(8, 14);      //x:8,  y:14 - Points to tile with ID 2 (Tiled internal ID: 1)
                tson::Tile *tile4 = layer->getTileData(17, 5);      //x:17, y:5  - Points to tile with ID 5 (Tiled internal ID: 4)

                //New in v1.2.0
                //You can now get tiles with positions and drawing rect via tson::TileObject
                //Drawing rects are also accessible through tson::Tile.
                tson::TileObject *tileobj1 = layer->getTileObject(4, 4);
                tson::Vector2f position = tileobj1->getPosition();
                tson::Rect drawingRect = tileobj1->getDrawingRect();

                //You can of course also loop through every tile!
                for (const auto &[id, tile] : tileData)
                {
                    //Must check for nullptr, due to how we got the first invalid tile (pos: 0, 4)
                    //Would be unnecessary otherwise.
                    if(tile != nullptr)
                        int tileId = tile->getId(); //A bit verbose, as this is the same as id from the key, but you get the idea.
                }
            }
        }

        //If an object supports properties, you can easily get a property value by calling get<T>() or the property itself with getProp()
        int myInt = layer->get<int>("my_int");
        float myFloat = layer->get<float>("my_float");
        bool myBool = layer->get<bool>("my_bool");
        std::string myString = layer->get<std::string>("my_string");
        tson::Colori myColor = layer->get<tson::Colori>("my_color");

        fs::path file = layer->get<fs::path>("my_file");

        tson::Property *prop = layer->getProp("my_property");
    }
    else //Error occurred
    {
        std::cout << map->getStatusMessage();
    }
    REQUIRE( true );
}

TEST_CASE( "A simple example on how to use data of objects and tiles", "[demo]" )
{
    tson::Tileson t;
    std::unique_ptr<tson::Map> map = t.parse(tson_files::_ULTIMATE_TEST_JSON, tson_files::_ULTIMATE_TEST_JSON_SIZE);

    if(map->getStatus() == tson::ParseStatus::OK)
    {
        //Gets the layer called "Object Layer" from the "ultimate_demo.json map
        tson::Layer *objectLayer = map->getLayer("Object Layer"); //This is an Object Layer

        //Example from an Object Layer.
        if(objectLayer->getType() == tson::LayerType::ObjectGroup)
        {
            tson::Object *goomba = objectLayer->firstObj("goomba"); //Gets the first object with this name. This can be any object.

            //If you want to just go through every existing object in the layer:
            for(auto &obj : objectLayer->getObjects())
            {
                tson::Vector2i position = obj.getPosition();
                tson::Vector2i size = obj.getSize();
                tson::ObjectType objType = obj.getObjectType();

                //You may want to check the object type to make sure you use the data right.
            }

            tson::ObjectType objType = goomba->getObjectType();

            /*!
             * tson::ObjectType is defined like this.
             * They are automatically detected based on what kind of object you have created
             * enum class Type : uint8_t
                {
                    Undefined = 0,
                    Object = 1,
                    Ellipse = 2, //<-- Circle
                    Rectangle = 3,
                    Point = 4,
                    Polygon = 5,
                    Polyline = 6,
                    Text = 7,
                    Template = 8
                };
             */

            if (objType == tson::ObjectType::Rectangle)
            {
                tson::Vector2i size = goomba->getSize();
                tson::Vector2i position = goomba->getPosition();

                //If you have set a custom property, you can also get this
                int hp = goomba->get<int>("hp");

                //Using size and position you can can create a Rectangle object by your library of choice.
                //An example if you were about to use SFML for drawing:
                //sf::RectangleShape rect;
                //rect.setSize(sf::Vector2f(size.x, size.y));
                //rect.setPosition(sf::Vector2f(position.x, position.y));
            }
            else if (objType == tson::ObjectType::Polygon)
            {
                for(auto const &poly : goomba->getPolygons())
                {
                    //Set a point on a shape taking polygons
                }
                tson::Vector2i position = goomba->getPosition();
            }
            else if (objType == tson::ObjectType::Polyline)
            {
                std::vector<tson::Vector2i> polys = goomba->getPolylines();
                for(auto const &poly : goomba->getPolylines())
                {

                }
                tson::Vector2i position = goomba->getPosition();
            }
        }

        tson::Layer *tileLayer = map->getLayer("Main Layer"); //This is a Tile Layer.

        //You can get your tileset like this, but in v1.2.0
        //The tiles themselves holds a pointer to their related tileset.
        tson::Tileset *tileset = map->getTileset("demo-tileset");

        //Example from a Tile Layer
        //I know for a fact that this is a Tile Layer, but you can check it this way to be sure.
        if(tileLayer->getType() == tson::LayerType::TileLayer)
        {
            //pos = position in tile units
            for(auto &[pos, tileObject] : tileLayer->getTileObjects()) //Loops through absolutely all existing tileObjects
            {
                tson::Tileset *tileset = tileObject.getTile()->getTileset();
                tson::Rect drawingRect = tileObject.getDrawingRect();
                tson::Vector2f position = tileObject.getPosition();

                //Here you can determine the offset that should be set on a sprite
                //Example on how it would be done using SFML (where sprite presumably is a member of a generated game object):
                //sf::Sprite *sprite = storeAndLoadImage(tileset->getImage().u8string(), {0, 0});
                //if (sprite != nullptr)
                //{
                //    sprite->setTextureRect({drawingRect.x, drawingRect.y, drawingRect.width, drawingRect.height});
                //    sprite->setPosition({position.x, position.y});
                //    m_window.draw(*sprite);
                //}
            }
        }
    }
    REQUIRE( true );
}
