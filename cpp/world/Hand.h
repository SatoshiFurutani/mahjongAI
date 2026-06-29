#pragma once

#include "Tile.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace mahjong::world {

class Hand {
public:
    using Counts = std::array<std::uint8_t, 34>;

    Hand();

    [[nodiscard]] std::uint8_t count(TileType type) const;
    [[nodiscard]] std::uint8_t count(const Tile& tile) const;
    [[nodiscard]] std::uint8_t countByIndex(std::size_t index) const;
    [[nodiscard]] std::uint16_t totalCount() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] const Counts& counts() const;
    [[nodiscard]] std::vector<Tile> tiles() const;

    void clear();
    void setCount(TileType type, std::uint8_t count);
    void setCounts(const Counts& counts);

    bool add(TileType type, std::uint8_t amount = 1);
    bool add(const Tile& tile, std::uint8_t amount = 1);
    bool remove(TileType type, std::uint8_t amount = 1);
    bool remove(const Tile& tile, std::uint8_t amount = 1);

private:
    Counts counts_ {};
};

} // namespace mahjong::world
