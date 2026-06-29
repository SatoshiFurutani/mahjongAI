#pragma once

#include "Tile.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace mahjong::world {

// 手牌はvector<Tile>ではなく34種類ごとの枚数で管理する。
// シャンテン計算や受け入れ枚数計算で同じ配列をそのまま使えるようにするため。
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

    // 表示やデバッグ用途向けにTile列へ展開する。
    // 赤ドラ情報は枚数配列に含めないため、ここでは通常牌として返す。
    [[nodiscard]] std::vector<Tile> tiles() const;

    void clear();
    void setCount(TileType type, std::uint8_t count);
    void setCounts(const Counts& counts);

    bool add(TileType type, std::uint8_t amount = 1);
    bool add(const Tile& tile, std::uint8_t amount = 1);
    bool remove(TileType type, std::uint8_t amount = 1);
    bool remove(const Tile& tile, std::uint8_t amount = 1);

private:
    // 添字はTileTypeの定義順と一致する。各要素は0から4までの枚数。
    Counts counts_ {};
};

} // namespace mahjong::world
