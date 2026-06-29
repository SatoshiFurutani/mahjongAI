import json
import sys
from pathlib import Path

import mjx

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mjx_bridge import observation_to_dto


MAX_STEPS = 80


def first_action(actions):
    # 仕様確認用なので、必ず何かを返して先へ進める。
    for action in actions:
        if "PASS" in str(action.type()):
            return action
    return actions[0]


def short_state_json(env):
    try:
        state = json.loads(env.state().to_json())
    except Exception as exc:
        return {"error": str(exc)}

    hidden = state.get("hiddenState", {})
    public = state.get("publicObservation", {})

    return {
        "keys": list(state.keys()),
        "hiddenStateKeys": list(hidden.keys()) if isinstance(hidden, dict) else [],
        "publicObservationKeys": list(public.keys()) if isinstance(public, dict) else [],
        "wallLength": len(hidden.get("wall", [])) if isinstance(hidden, dict) else None,
        "doraIndicators": public.get("doraIndicators") if isinstance(public, dict) else None,
        "eventCount": len(public.get("events", [])) if isinstance(public, dict) else None,
    }


def main():
    env = mjx.MjxEnv()
    obs_dict = env.reset()

    for step in range(MAX_STEPS):
        if env.done() or not obs_dict:
            print("STOP", {"step": step, "done": env.done(), "obsCount": len(obs_dict)})
            break

        print("\n" + "=" * 80)
        print("STEP", step)
        print("STATE", json.dumps(short_state_json(env), ensure_ascii=False))

        actions = {}
        for player_id, obs in obs_dict.items():
            print("\nPLAYER_ID", player_id)
            print("who", obs.who(), "dealer", obs.dealer(), "round", obs.round(), "tens", obs.tens(), "doras", obs.doras())

            print("EVENTS")
            for event in obs.events():
                print(event.to_json())

            legal_actions = obs.legal_actions()
            print("LEGAL_ACTIONS")
            for index, action in enumerate(legal_actions):
                tile_type = None
                try:
                    tile_type = action.tile().type().value
                except Exception:
                    pass
                print(index, str(action.type()), "idx", _safe_idx(action), "tileType", tile_type, "json", action.to_json())

            dto = observation_to_dto(obs, env.state())
            print("DTO_SUMMARY", json.dumps({
                "selfPlayer": dto["selfPlayer"],
                "roundNumber": dto["roundNumber"],
                "honba": dto["honba"],
                "reachSticks": dto["reachSticks"],
                "dealer": dto["dealer"],
                "remainingTileCount": dto["remainingTileCount"],
                "scores": dto["scores"],
                "riverSizes": [len(player["river"]) for player in dto["players"]],
                "selfHandSize": len(dto["selfHand"]),
                "legalActionCount": len(dto["legalActions"]),
            }, ensure_ascii=False))

            actions[player_id] = first_action(legal_actions)

        obs_dict = env.step(actions)
    else:
        print("STOP", {"reason": "max steps reached", "maxSteps": MAX_STEPS})


def _safe_idx(action):
    try:
        return action.to_idx()
    except Exception:
        return None


if __name__ == "__main__":
    main()
