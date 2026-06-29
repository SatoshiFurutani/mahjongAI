#include "River.h"

namespace mahjong::world {

const std::vector<RiverTile>& River::discards() const
{
    return discards_;
}

std::size_t River::size() const
{
    return discards_.size();
}

bool River::empty() const
{
    return discards_.empty();
}

void River::clear()
{
    discards_.clear();
}

void River::addDiscard(const Tile& tile, bool afterReach, bool tsumogiri)
{
    discards_.push_back(RiverTile {tile, afterReach, tsumogiri, false});
}

bool River::markCalled(std::size_t discardIndex)
{
    if (discardIndex >= discards_.size()) {
        return false;
    }
    discards_[discardIndex].called = true;
    return true;
}

} // namespace mahjong::world
