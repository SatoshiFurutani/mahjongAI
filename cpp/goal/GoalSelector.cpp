#include "GoalSelector.h"

#include <utility>

namespace mahjong::goal {

namespace {

bool isFastEnoughToPush(const situation::Situation& situation)
{
    return situation.handSpeed() == situation::HandSpeed::Tenpai
        || situation.handSpeed() == situation::HandSpeed::Fast;
}

bool hasMeaningfulValue(const situation::Situation& situation)
{
    return situation.doraCount() >= 2 || situation.isDealer();
}

class AllLastRule final : public GoalRule {
public:
    [[nodiscard]] bool matches(const situation::Situation& situation) const override
    {
        return situation.isAllLast();
    }

    [[nodiscard]] GoalDecision decide(const situation::Situation& situation) const override
    {
        GoalDecision decision;
        decision.gameGoal = situation.selfRank() == 1 ? GameGoal::PreserveRank : GameGoal::SatisfyWinningCondition;

        if (situation.selfRank() == 1 && situation.anyOpponentReach() && !isFastEnoughToPush(situation)) {
            decision.roundGoal = RoundGoal::Fold;
            decision.reasons.push_back("オーラス首位のため、順位維持を最優先する");
            decision.reasons.push_back("他家リーチに対して手牌速度が足りないため、放銃回避を選ぶ");
            return decision;
        }

        if (situation.selfRank() >= 3 || situation.pointGapToNextRank() > 0) {
            decision.roundGoal = isFastEnoughToPush(situation) ? RoundGoal::PushAll : RoundGoal::FastWin;
            decision.reasons.push_back("オーラスのため、最終順位を上げる和了条件を優先する");
            return decision;
        }

        decision.roundGoal = situation.anyOpponentReach() ? RoundGoal::Mawashi : RoundGoal::FastWin;
        decision.reasons.push_back("オーラスの順位条件を満たすため、局収支よりゲーム収支を優先する");
        return decision;
    }
};

class LateDefenseRule final : public GoalRule {
public:
    [[nodiscard]] bool matches(const situation::Situation& situation) const override
    {
        return situation.anyOpponentReach()
            && situation.gamePhase() >= situation::GamePhase::Late
            && !isFastEnoughToPush(situation);
    }

    [[nodiscard]] GoalDecision decide(const situation::Situation&) const override
    {
        GoalDecision decision;
        decision.gameGoal = GameGoal::AvoidRankDrop;
        decision.roundGoal = RoundGoal::Fold;
        decision.reasons.push_back("終盤に他家リーチがあり、放銃による順位低下リスクが高い");
        decision.reasons.push_back("手牌が遅いため、局の最大リターンより失点回避を優先する");
        return decision;
    }
};

class TenpaiPushRule final : public GoalRule {
public:
    [[nodiscard]] bool matches(const situation::Situation& situation) const override
    {
        return situation.handSpeed() == situation::HandSpeed::Tenpai;
    }

    [[nodiscard]] GoalDecision decide(const situation::Situation& situation) const override
    {
        GoalDecision decision;
        decision.gameGoal = GameGoal::MaximizeReturn;
        decision.roundGoal = RoundGoal::PushAll;
        decision.reasons.push_back("テンパイしているため、局の最大リターンを取りに行く");
        if (situation.anyOpponentReach()) {
            decision.reasons.push_back("他家リーチはあるが、和了に最も近い状態を優先する");
        }
        return decision;
    }
};

class FastValuableHandRule final : public GoalRule {
public:
    [[nodiscard]] bool matches(const situation::Situation& situation) const override
    {
        return situation.handSpeed() == situation::HandSpeed::Fast && hasMeaningfulValue(situation);
    }

    [[nodiscard]] GoalDecision decide(const situation::Situation& situation) const override
    {
        GoalDecision decision;
        decision.gameGoal = GameGoal::MaximizeReturn;
        decision.roundGoal = situation.anyOpponentReach() ? RoundGoal::Mawashi : RoundGoal::PushAll;
        decision.reasons.push_back("手牌速度と打点材料があり、局収支を取りに行ける");
        if (situation.anyOpponentReach()) {
            decision.reasons.push_back("他家リーチがあるため、全ツッパではなく回し打ちを選ぶ");
        }
        return decision;
    }
};

class DefaultRule final : public GoalRule {
public:
    [[nodiscard]] bool matches(const situation::Situation&) const override
    {
        return true;
    }

    [[nodiscard]] GoalDecision decide(const situation::Situation& situation) const override
    {
        GoalDecision decision;
        decision.gameGoal = GameGoal::MaximizeReturn;

        if (situation.anyOpponentReach()) {
            decision.roundGoal = RoundGoal::Mawashi;
            decision.reasons.push_back("他家リーチがあるため、安全度を見ながら回し打ちする");
            return decision;
        }

        decision.roundGoal = situation.doraCount() >= 2 ? RoundGoal::HighScore : RoundGoal::FastWin;
        decision.reasons.push_back("明確な守備制約がないため、局の最大リターンを目指す");
        return decision;
    }
};

} // namespace

GoalSelector::GoalSelector()
{
    addRule(std::make_unique<AllLastRule>());
    addRule(std::make_unique<LateDefenseRule>());
    addRule(std::make_unique<TenpaiPushRule>());
    addRule(std::make_unique<FastValuableHandRule>());
    addRule(std::make_unique<DefaultRule>());
}

GoalDecision GoalSelector::select(const situation::Situation& situation) const
{
    for (const auto& rule : rules_) {
        if (rule->matches(situation)) {
            return rule->decide(situation);
        }
    }

    return {};
}

void GoalSelector::addRule(std::unique_ptr<GoalRule> rule)
{
    rules_.push_back(std::move(rule));
}

const char* toString(GameGoal goal)
{
    switch (goal) {
    case GameGoal::MaximizeReturn:
        return "MaximizeReturn";
    case GameGoal::SatisfyWinningCondition:
        return "SatisfyWinningCondition";
    case GameGoal::PreserveRank:
        return "PreserveRank";
    case GameGoal::AvoidRankDrop:
        return "AvoidRankDrop";
    }
    return "Unknown";
}

const char* toString(RoundGoal goal)
{
    switch (goal) {
    case RoundGoal::PushAll:
        return "PushAll";
    case RoundGoal::Fold:
        return "Fold";
    case RoundGoal::Mawashi:
        return "Mawashi";
    case RoundGoal::FastWin:
        return "FastWin";
    case RoundGoal::HighScore:
        return "HighScore";
    }
    return "Unknown";
}

} // namespace mahjong::goal
