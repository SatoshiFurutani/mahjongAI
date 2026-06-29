#pragma once

#include "Tile.h"

#include <cstddef>
#include <vector>

namespace mahjong::world {

struct RiverTile {
    Tile tile {};
    bool afterReach {false};
    bool called {false};
};

class River {
public:
    [[nodiscard]] const std::vector<RiverTile>& discards() const;
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] bool empty() const;

    void clear();
    void addDiscard(const Tile& tile, bool afterReach = false);
    bool markCalled(std::size_t discardIndex);

private:
    std::vector<RiverTile> discards_ {};
};

} // namespace mahjong::world
