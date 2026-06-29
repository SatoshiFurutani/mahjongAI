from mjx.agents import Agent
from mjx import ActionType
import random

class HonorDiscardAgent(Agent):

    def act(self, observation):

        actions = observation.legal_actions()

        if len(actions) == 1:
            return actions[0]

        # 和了
        for a in actions:
            if a.type() in [ActionType.TSUMO, ActionType.RON]:
                return a

        # 打牌候補
        discards = [
            a for a in actions
            if a.type() in [ActionType.DISCARD, ActionType.TSUMOGIRI]
        ]

        # 字牌優先
        for a in discards:

            tile_type = a.tile().type().value

            if tile_type >= 27:
                return a

        return random.choice(discards)