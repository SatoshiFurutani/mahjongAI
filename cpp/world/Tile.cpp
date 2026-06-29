#include "Tile.h"

#include <array>
#include <stdexcept>

namespace mahjong::world {

namespace {

constexpr std::uint8_t kTileTypeCount = static_cast<std::uint8_t>(TileType::Count);

constexpr std::array<const char*, kTileTypeCount> kTileNames {
    "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9",
    "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8", "P9",
    "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9",
    "East", "South", "West", "North", "White", "Green", "Red"
};

} // namespace

Tile::Tile(TileType type, bool redDora)
    : type_(type),
      redDora_(redDora)
{
    if (!isValidTileType(type_)) {
        throw std::invalid_argument("invalid tile type");
    }
}

TileType Tile::type() const
{
    return type_;
}

bool Tile::isRedDora() const
{
    return redDora_;
}

void Tile::setType(TileType type)
{
    if (!isValidTileType(type)) {
        throw std::invalid_argument("invalid tile type");
    }
    type_ = type;
}

void Tile::setRedDora(bool redDora)
{
    redDora_ = redDora;
}

std::uint8_t toIndex(TileType type)
{
    if (!isValidTileType(type)) {
        throw std::invalid_argument("invalid tile type");
    }
    return static_cast<std::uint8_t>(type);
}

TileType tileTypeFromIndex(std::uint8_t index)
{
    if (index >= kTileTypeCount) {
        throw std::out_of_range("tile index must be in [0, 33]");
    }
    return static_cast<TileType>(index);
}

bool isValidTileType(TileType type)
{
    const auto index = static_cast<std::uint8_t>(type);
    return index < kTileTypeCount;
}

bool isNumberTile(TileType type)
{
    return isValidTileType(type) && static_cast<std::uint8_t>(type) <= static_cast<std::uint8_t>(TileType::S9);
}

bool isHonorTile(TileType type)
{
    return isValidTileType(type) && static_cast<std::uint8_t>(type) >= static_cast<std::uint8_t>(TileType::East);
}

std::string toString(TileType type)
{
    return kTileNames.at(toIndex(type));
}

std::string toString(const Tile& tile)
{
    return tile.isRedDora() ? "Red" + toString(tile.type()) : toString(tile.type());
}

bool operator==(const Tile& lhs, const Tile& rhs)
{
    return lhs.type() == rhs.type() && lhs.isRedDora() == rhs.isRedDora();
}

bool operator!=(const Tile& lhs, const Tile& rhs)
{
    return !(lhs == rhs);
}

} // namespace mahjong::world
