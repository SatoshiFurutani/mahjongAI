#pragma once

#include "Player.h"
#include "Tile.h"

#include <array>
#include <cstdint>
#include <vector>

namespace mahjong::world {

// 1局面の世界状態。
// MJXのObservationを直接持たず、AIが解釈しやすい独自の状態だけを保持する。
class World {
public:
    static constexpr std::uint8_t kPlayerCount = 4;

    using Players = std::array<Player, kPlayerCount>;

    [[nodiscard]] const Players& players() const;
    [[nodiscard]] Players& players();
    [[nodiscard]] const Player& player(std::uint8_t playerIndex) const;
    [[nodiscard]] Player& player(std::uint8_t playerIndex);
    [[nodiscard]] const std::vector<Tile>& doraTiles() const;
    [[nodiscard]] std::uint8_t roundNumber() const;
    [[nodiscard]] std::uint8_t honba() const;
    [[nodiscard]] std::uint8_t reachSticks() const;
    [[nodiscard]] std::uint8_t dealer() const;
    [[nodiscard]] std::uint8_t turn() const;
    [[nodiscard]] std::uint8_t remainingTileCount() const;

    void setPlayers(const Players& players);
    void setPlayer(std::uint8_t playerIndex, const Player& player);
    void setDoraTiles(const std::vector<Tile>& doraTiles);
    void addDoraTile(const Tile& tile);
    void clearDoraTiles();
    void setRoundNumber(std::uint8_t roundNumber);
    void setHonba(std::uint8_t honba);
    void setReachSticks(std::uint8_t reachSticks);
    void setDealer(std::uint8_t dealer);
    void setTurn(std::uint8_t turn);
    void setRemainingTileCount(std::uint8_t remainingTileCount);

private:
    // 4人分の状態。添字はプレイヤー番号として扱う。
    Players players_ {};

    // 表示済みドラをTileとして保持する。裏ドラなどを入れるかは変換層側で決める。
    std::vector<Tile> doraTiles_ {};

    // roundNumberは東1局を0とするような内部表現を想定する。
    std::uint8_t roundNumber_ {0};
    std::uint8_t honba_ {0};
    std::uint8_t reachSticks_ {0};
    std::uint8_t dealer_ {0};
    std::uint8_t turn_ {0};
    std::uint8_t remainingTileCount_ {70};
};

} // namespace mahjong::world
