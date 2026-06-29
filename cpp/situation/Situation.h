#pragma once

#include "../world/Tile.h"

#include <array>
#include <cstdint>
#include <vector>

namespace mahjong::situation {

// 山の残り枚数から見た局面の進行段階。
// Worldでは巡目を直接持たず、ここで説明や評価に使いやすい粒度へ変換する。
enum class GamePhase : std::uint8_t {
    Early,
    Middle,
    Late,
    Endgame
};

// 手牌速度の大まかな分類。GoalSelectorが攻守判断を行う入口になる。
enum class HandSpeed : std::uint8_t {
    Slow,
    Medium,
    Fast,
    Tenpai
};

// Worldから抽出した局面特徴。
// ここには判断ロジックを置かず、GoalSelectorやFeatureGeneratorが読む派生情報だけを保持する。
class Situation {
public:
    using VisibleCounts = std::array<std::uint8_t, 34>;

    [[nodiscard]] std::uint8_t selfPlayerIndex() const;
    [[nodiscard]] std::int8_t shanten() const;
    [[nodiscard]] HandSpeed handSpeed() const;
    [[nodiscard]] GamePhase gamePhase() const;
    [[nodiscard]] bool isDealer() const;
    [[nodiscard]] bool selfReach() const;
    [[nodiscard]] bool anyOpponentReach() const;
    [[nodiscard]] std::uint8_t opponentReachCount() const;
    [[nodiscard]] const std::vector<std::uint8_t>& opponentReachPlayers() const;
    [[nodiscard]] std::uint8_t remainingTileCount() const;
    [[nodiscard]] std::uint8_t doraCount() const;
    [[nodiscard]] std::uint8_t pairCount() const;
    [[nodiscard]] std::uint8_t openMeldCount() const;
    [[nodiscard]] const VisibleCounts& visibleTileCounts() const;

    void setSelfPlayerIndex(std::uint8_t selfPlayerIndex);
    void setShanten(std::int8_t shanten);
    void setHandSpeed(HandSpeed handSpeed);
    void setGamePhase(GamePhase gamePhase);
    void setDealer(bool isDealer);
    void setSelfReach(bool selfReach);
    void setOpponentReachPlayers(const std::vector<std::uint8_t>& opponentReachPlayers);
    void setRemainingTileCount(std::uint8_t remainingTileCount);
    void setDoraCount(std::uint8_t doraCount);
    void setPairCount(std::uint8_t pairCount);
    void setOpenMeldCount(std::uint8_t openMeldCount);
    void setVisibleTileCounts(const VisibleCounts& visibleTileCounts);

private:
    std::uint8_t selfPlayerIndex_ {0};
    std::int8_t shanten_ {8};
    HandSpeed handSpeed_ {HandSpeed::Slow};
    GamePhase gamePhase_ {GamePhase::Early};
    bool isDealer_ {false};
    bool selfReach_ {false};
    std::vector<std::uint8_t> opponentReachPlayers_ {};
    std::uint8_t remainingTileCount_ {70};
    std::uint8_t doraCount_ {0};
    std::uint8_t pairCount_ {0};
    std::uint8_t openMeldCount_ {0};
    VisibleCounts visibleTileCounts_ {};
};

} // namespace mahjong::situation
