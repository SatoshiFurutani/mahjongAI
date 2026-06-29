#pragma once

#include "Tile.h"

#include <cstddef>
#include <vector>

namespace mahjong::world {

// 河に置かれた1枚の捨て牌。
// 順序、リーチ後の捨て牌か、鳴かれて河から消えたかを追跡する。
struct RiverTile {
    Tile tile {};
    bool afterReach {false};
    bool called {false};
};

// 捨て牌の時系列を保持する。
// 防御評価では順序やリーチ後情報が重要になるため、単なる枚数集計にはしない。
class River {
public:
    [[nodiscard]] const std::vector<RiverTile>& discards() const;
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] bool empty() const;

    void clear();
    void addDiscard(const Tile& tile, bool afterReach = false);
    bool markCalled(std::size_t discardIndex);

private:
    // 捨てられた順に保存する。
    std::vector<RiverTile> discards_ {};
};

} // namespace mahjong::world
