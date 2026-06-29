#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace mahjong::world {

// Python側でMJX Observationから抽出した牌情報。
// C++側ではMJXのtile idを受け取らず、34種の牌種と赤ドラ情報だけを扱う。
struct TileDto {
    std::uint8_t type {0};
    bool red {false};
};

struct RiverTileDto {
    TileDto tile {};
    bool afterReach {false};
    bool tsumogiri {false};
    bool called {false};
};

enum class OpenTypeDto : std::uint8_t {
    Chi,
    Pon,
    Daiminkan,
    Ankan,
    Kakan
};

struct OpenMeldDto {
    OpenTypeDto type {OpenTypeDto::Chi};
    std::vector<TileDto> tiles {};
    std::uint8_t fromPlayer {0};
};

struct PlayerObservationDto {
    std::vector<RiverTileDto> river {};
    std::vector<OpenMeldDto> openMelds {};
    int score {25000};
    bool reach {false};
    bool ippatsu {false};
    std::uint8_t drawOrder {0};
};

enum class LegalActionTypeDto : std::uint8_t {
    Discard,
    Tsumogiri,
    Chi,
    Pon,
    Daiminkan,
    Ankan,
    Kakan,
    Reach,
    Ron,
    Tsumo,
    Pass,
    Unknown
};

struct LegalActionDto {
    LegalActionTypeDto type {LegalActionTypeDto::Unknown};
    bool hasTile {false};
    TileDto tile {};
};

struct ObservationDto {
    std::uint8_t selfPlayer {0};
    std::uint8_t roundNumber {0};
    std::uint8_t honba {0};
    std::uint8_t reachSticks {0};
    std::uint8_t dealer {0};
    std::uint8_t remainingTileCount {70};
    std::array<int, 4> scores {25000, 25000, 25000, 25000};

    // 現時点ではWorld::doraTilesへそのまま格納する。
    // DTO名はPython/MJX側に合わせてドラ表示牌としておく。
    std::vector<TileDto> doraIndicators {};

    std::array<PlayerObservationDto, 4> players {};
    std::vector<TileDto> selfHand {};
    std::vector<LegalActionDto> legalActions {};
};

} // namespace mahjong::world
