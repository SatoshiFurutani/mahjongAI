#pragma once

#include "Hand.h"
#include "River.h"

#include <cstdint>
#include <vector>

namespace mahjong::world {

// 副露の種類。暗槓も手牌外の公開/固定情報として同じ枠で扱う。
enum class OpenType : std::uint8_t {
    Chi,
    Pon,
    Daiminkan,
    Ankan,
    Kakan
};

// 副露1つ分の情報。
// fromPlayerは誰の捨て牌から鳴いたかを0から3のプレイヤー番号で表す。
struct OpenMeld {
    OpenType type {OpenType::Chi};
    std::vector<Tile> tiles {};
    std::uint8_t fromPlayer {0};
};

// 1人分の公開状態と手牌状態を保持する。
// 判断ロジックはここに置かず、WorldからFeatureGeneratorへ渡す材料に徹する。
class Player {
public:
    [[nodiscard]] const Hand& hand() const;
    [[nodiscard]] Hand& hand();
    [[nodiscard]] const River& river() const;
    [[nodiscard]] River& river();
    [[nodiscard]] const std::vector<OpenMeld>& openMelds() const;
    [[nodiscard]] int score() const;
    [[nodiscard]] bool isReach() const;
    [[nodiscard]] bool isIppatsu() const;
    [[nodiscard]] std::uint8_t drawOrder() const;

    void setHand(const Hand& hand);
    void setRiver(const River& river);
    void setScore(int score);
    void setReach(bool reach);
    void setIppatsu(bool ippatsu);
    void setDrawOrder(std::uint8_t drawOrder);

    void addOpenMeld(const OpenMeld& openMeld);
    void clearOpenMelds();
    void resetForNewRound(int initialScore);

private:
    // AI本人以外の手牌は、MJX変換層で見える範囲だけを入れる想定。
    Hand hand_ {};
    River river_ {};
    std::vector<OpenMeld> openMelds_ {};
    int score_ {25000};
    bool reach_ {false};
    bool ippatsu_ {false};
    std::uint8_t drawOrder_ {0};
};

} // namespace mahjong::world
