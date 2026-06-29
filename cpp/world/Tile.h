#pragma once

#include <cstdint>
#include <string>

namespace mahjong::world {

// 麻雀牌を34種類の論理種別として表す。
// MJXのtile IDや物理牌のコピー番号は保持せず、AI内部の判断に必要な情報だけに絞る。
enum class TileType : std::uint8_t {
    M1 = 0,
    M2,
    M3,
    M4,
    M5,
    M6,
    M7,
    M8,
    M9,
    P1,
    P2,
    P3,
    P4,
    P5,
    P6,
    P7,
    P8,
    P9,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    East,
    South,
    West,
    North,
    White,
    Green,
    Red,
    Count
};

// 1枚の牌を表す値オブジェクト。
// 種別と赤ドラかどうかだけを持ち、ゲームシミュレータ由来の情報は混ぜない。
class Tile {
public:
    Tile() = default;
    explicit Tile(TileType type, bool redDora = false);

    [[nodiscard]] TileType type() const;
    [[nodiscard]] bool isRedDora() const;

    void setType(TileType type);
    void setRedDora(bool redDora);

private:
    TileType type_ {TileType::M1};
    bool redDora_ {false};
};

// Hand内部の34要素配列とTileTypeを相互変換するための補助関数。
[[nodiscard]] std::uint8_t toIndex(TileType type);
[[nodiscard]] TileType tileTypeFromIndex(std::uint8_t index);
[[nodiscard]] bool isValidTileType(TileType type);
[[nodiscard]] bool isNumberTile(TileType type);
[[nodiscard]] bool isHonorTile(TileType type);
[[nodiscard]] std::string toString(TileType type);
[[nodiscard]] std::string toString(const Tile& tile);

[[nodiscard]] bool operator==(const Tile& lhs, const Tile& rhs);
[[nodiscard]] bool operator!=(const Tile& lhs, const Tile& rhs);

} // namespace mahjong::world
