#pragma once

#include "Situation.h"
#include "../world/World.h"

#include <cstdint>

namespace mahjong::situation {

// Worldを読み取り専用で解析し、GoalSelectorやFeatureGeneratorが使うSituationへ要約する。
class SituationAnalyzer {
public:
    [[nodiscard]] Situation analyze(const world::World& world, std::uint8_t selfPlayerIndex) const;

private:
    [[nodiscard]] std::int8_t calculateShanten(const world::Player& player) const;
    [[nodiscard]] std::int8_t calculateStandardShanten(
        const world::Hand::Counts& counts,
        std::uint8_t openMeldCount) const;
    [[nodiscard]] std::int8_t calculateSevenPairsShanten(const world::Hand::Counts& counts) const;
    [[nodiscard]] std::uint8_t countDora(const world::Player& player, const world::World& world) const;
    [[nodiscard]] Situation::Scores buildScores(const world::World& world) const;
    [[nodiscard]] std::uint8_t calculateRank(const Situation::Scores& scores, std::uint8_t selfPlayerIndex) const;
    [[nodiscard]] int calculatePointGapToNextRank(const Situation::Scores& scores, std::uint8_t selfPlayerIndex) const;
    [[nodiscard]] int calculatePointGapFromLowerRank(const Situation::Scores& scores, std::uint8_t selfPlayerIndex) const;
    [[nodiscard]] Situation::VisibleCounts buildVisibleTileCounts(
        const world::World& world,
        std::uint8_t selfPlayerIndex) const;
    [[nodiscard]] HandSpeed classifyHandSpeed(std::int8_t shanten) const;
    [[nodiscard]] GamePhase classifyGamePhase(std::uint8_t remainingTileCount) const;
};

} // namespace mahjong::situation
