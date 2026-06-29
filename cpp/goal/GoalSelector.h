#pragma once

#include "../situation/Situation.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace mahjong::goal {

// 上位目標。半荘全体の最大リターンを目指し、終盤ほど順位条件を強く扱う。
enum class GameGoal : std::uint8_t {
    MaximizeReturn,
    SatisfyWinningCondition,
    PreserveRank,
    AvoidRankDrop
};

// 中位目標。現在局で取る押し引き方針を表す。
enum class RoundGoal : std::uint8_t {
    PushAll,
    Fold,
    Mawashi,
    FastWin,
    HighScore
};

struct GoalDecision {
    GameGoal gameGoal {GameGoal::MaximizeReturn};
    RoundGoal roundGoal {RoundGoal::Mawashi};
    std::vector<std::string> reasons {};
};

class GoalRule {
public:
    virtual ~GoalRule() = default;
    [[nodiscard]] virtual bool matches(const situation::Situation& situation) const = 0;
    [[nodiscard]] virtual GoalDecision decide(const situation::Situation& situation) const = 0;
};

// Situationから上位目標と中位目標を決める。
// ルールを追加登録できるため、Goal追加時も既存ルールを壊さず拡張しやすい。
class GoalSelector {
public:
    GoalSelector();

    [[nodiscard]] GoalDecision select(const situation::Situation& situation) const;
    void addRule(std::unique_ptr<GoalRule> rule);

private:
    std::vector<std::unique_ptr<GoalRule>> rules_ {};
};

[[nodiscard]] const char* toString(GameGoal goal);
[[nodiscard]] const char* toString(RoundGoal goal);

} // namespace mahjong::goal
