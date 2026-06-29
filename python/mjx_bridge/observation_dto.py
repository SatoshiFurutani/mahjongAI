"""MJX ObservationをC++側のObservationDto相当へ変換する。

このモジュールはPython/MJX依存を閉じ込める境界層です。
C++側のWorldはMJXオブジェクトを直接知らず、このDTOだけを受け取ります。
"""

from __future__ import annotations

import json
from typing import Any, Optional


ACTION_TYPE_MAP = {
    "DISCARD": "Discard",
    "TSUMOGIRI": "Tsumogiri",
    "CHI": "Chi",
    "PON": "Pon",
    "DAIMINKAN": "Daiminkan",
    "ANKAN": "Ankan",
    "KAKAN": "Kakan",
    "REACH": "Reach",
    "RIICHI": "Reach",
    "RON": "Ron",
    "TSUMO": "Tsumo",
    "PASS": "Pass",
    "NO": "Pass",
}

OPEN_TYPE_MAP = {
    "CHI": "Chi",
    "PON": "Pon",
    "DAIMINKAN": "Daiminkan",
    "ANKAN": "Ankan",
    "KAKAN": "Kakan",
}


def observation_to_dto(obs: Any, state: Optional[Any] = None, default_remaining_tile_count: int = 70) -> dict[str, Any]:
    """MJXのObservationからC++へ渡すDTOを作る。

    stateは任意です。env.state()を渡せる場合、山の残り枚数などObservation単体では
    見つけにくい情報を補完します。
    """

    self_player = _as_int(_call_or_default(obs, "who", 0))
    dealer = _as_int(_call_or_default(obs, "dealer", 0))
    scores = [_as_int(score) for score in list(_call_or_default(obs, "tens", [25000, 25000, 25000, 25000]))]
    scores = _pad(scores, 4, 25000)[:4]

    players = [
        {
            "river": [],
            "openMelds": [],
            "score": scores[player_index],
            "reach": False,
            "ippatsu": False,
            "drawOrder": 0,
        }
        for player_index in range(4)
    ]

    events = [_to_json_dict(event) for event in _call_or_default(obs, "events", [])]
    _apply_events_to_players(events, players)

    state_json = _to_json_dict(state) if state is not None else {}

    return {
        "selfPlayer": self_player,
        "roundNumber": _extract_round_number(obs, state_json),
        "honba": _extract_first_int(state_json, ["honba", "numHonba", "roundSticks"], 0),
        "reachSticks": _extract_first_int(state_json, ["reachSticks", "riichiSticks", "deposit", "deposits"], 0),
        "dealer": dealer,
        "remainingTileCount": _extract_remaining_tile_count(state_json, default_remaining_tile_count),
        "scores": scores,
        "doraIndicators": [tile_to_dto(tile) for tile in _call_or_default(obs, "doras", [])],
        "players": players,
        "selfHand": [tile_to_dto(tile) for tile in _closed_tiles(obs)],
        "legalActions": [action_to_dto(action) for action in _call_or_default(obs, "legal_actions", [])],
    }


def tile_to_dto(tile: Any) -> dict[str, Any]:
    """MJX Tileまたはtile JSON断片をC++ TileDtoへ変換する。"""

    if isinstance(tile, dict):
        tile_type = _extract_first_int(tile, ["type", "tileType", "kind"], 0)
        red = bool(tile.get("red", tile.get("isRed", tile.get("is_red", False))))
        return {"type": tile_type, "red": red}

    tile_type = _call_or_default(tile, "type", tile)
    return {
        "type": _as_int(tile_type),
        "red": bool(_call_or_default(tile, "is_red", False)),
    }


def action_to_dto(action: Any) -> dict[str, Any]:
    """MJX ActionをC++ LegalActionDtoへ変換する。"""

    action_type = _normalize_action_type(_call_or_default(action, "type", "Unknown"))
    tile = _safe_call(action, "tile")

    if tile is None:
        return {
            "type": action_type,
            "hasTile": False,
            "tile": {"type": 0, "red": False},
        }

    return {
        "type": action_type,
        "hasTile": True,
        "tile": tile_to_dto(tile),
    }


def dto_to_json(dto: dict[str, Any]) -> str:
    """デバッグやファイル出力向けにDTOをJSON文字列へ変換する。"""

    return json.dumps(dto, ensure_ascii=False, separators=(",", ":"))


def _closed_tiles(obs: Any) -> list[Any]:
    hand = _safe_call(obs, "curr_hand")
    if hand is None:
        return []
    return list(_call_or_default(hand, "closed_tiles", []))


def _apply_events_to_players(events: list[dict[str, Any]], players: list[dict[str, Any]]) -> None:
    reach_flags = [False, False, False, False]

    for event in events:
        event_type = _normalize_event_type(event.get("type", event.get("eventType", "")))
        player_index = _event_player(event)

        if player_index is None or not 0 <= player_index < 4:
            continue

        if event_type in {"REACH", "RIICHI"}:
            players[player_index]["reach"] = True
            reach_flags[player_index] = True
            continue

        if event_type in {"DISCARD", "TSUMOGIRI", "DAHAI"}:
            tile = _event_tile(event)
            if tile is None:
                continue
            players[player_index]["river"].append(
                {
                    "tile": tile_to_dto(tile),
                    "afterReach": reach_flags[player_index],
                    "tsumogiri": event_type == "TSUMOGIRI" or bool(event.get("tsumogiri", event.get("isTsumogiri", False))),
                    "called": bool(event.get("called", event.get("isCalled", False))),
                }
            )
            continue

        if event_type in OPEN_TYPE_MAP:
            players[player_index]["openMelds"].append(_event_open_meld(event, player_index, event_type))
            _mark_called_tile(players, event)


def _event_open_meld(event: dict[str, Any], player_index: int, event_type: str) -> dict[str, Any]:
    tiles = event.get("tiles", event.get("meldTiles", []))
    if not isinstance(tiles, list):
        tiles = []

    return {
        "type": OPEN_TYPE_MAP.get(event_type, "Chi"),
        "tiles": [tile_to_dto(tile) for tile in tiles],
        "fromPlayer": _as_int(event.get("fromPlayer", event.get("target", player_index))),
    }


def _mark_called_tile(players: list[dict[str, Any]], event: dict[str, Any]) -> None:
    from_player = event.get("fromPlayer", event.get("target", None))
    if from_player is None:
        return

    from_player = _as_int(from_player)
    if not 0 <= from_player < 4 or not players[from_player]["river"]:
        return

    players[from_player]["river"][-1]["called"] = True


def _event_player(event: dict[str, Any]) -> Optional[int]:
    for key in ("who", "actor", "player", "playerId", "player_id"):
        if key in event:
            return _as_int(event[key])
    return None


def _event_tile(event: dict[str, Any]) -> Optional[Any]:
    for key in ("tile", "pai", "discard", "discardedTile"):
        if key in event:
            return event[key]
    return None


def _extract_round_number(obs: Any, state_json: dict[str, Any]) -> int:
    state_round = _extract_first_int(state_json, ["roundNumber", "round"], None)
    if state_round is not None:
        return state_round
    return _as_int(_call_or_default(obs, "round", 0))


def _extract_remaining_tile_count(state_json: dict[str, Any], default_value: int) -> int:
    direct = _extract_first_int(state_json, ["remainingTileCount", "wallCount", "numRemainingTiles"], None)
    if direct is not None:
        return direct

    hidden = state_json.get("hiddenState", {})
    if isinstance(hidden, dict) and isinstance(hidden.get("wall"), list):
        return len(hidden["wall"])

    return default_value


def _extract_first_int(data: dict[str, Any], keys: list[str], default: Optional[int]) -> Optional[int]:
    if not isinstance(data, dict):
        return default

    for key in keys:
        if key in data:
            return _as_int(data[key])

    for value in data.values():
        if isinstance(value, dict):
            found = _extract_first_int(value, keys, None)
            if found is not None:
                return found

    return default


def _normalize_action_type(action_type: Any) -> str:
    name = _enum_name(action_type)
    return ACTION_TYPE_MAP.get(name, "Unknown")


def _normalize_event_type(event_type: Any) -> str:
    return _enum_name(event_type)


def _enum_name(value: Any) -> str:
    name = getattr(value, "name", None)
    if name is not None:
        return str(name).upper()

    text = str(value)
    if "." in text:
        text = text.rsplit(".", 1)[-1]
    return text.upper()


def _call_or_default(obj: Any, method_name: str, default: Any) -> Any:
    value = _safe_call(obj, method_name)
    return default if value is None else value


def _safe_call(obj: Any, method_name: str) -> Optional[Any]:
    try:
        attr = getattr(obj, method_name)
    except Exception:
        return None

    try:
        return attr()
    except Exception:
        return None


def _to_json_dict(obj: Any) -> dict[str, Any]:
    if obj is None:
        return {}
    if isinstance(obj, dict):
        return obj
    try:
        return json.loads(obj.to_json())
    except Exception:
        return {}


def _as_int(value: Any) -> int:
    if value is None:
        return 0
    enum_value = getattr(value, "value", None)
    if enum_value is not None:
        return int(enum_value)
    return int(value)


def _pad(values: list[Any], size: int, fill: Any) -> list[Any]:
    return values + [fill] * max(0, size - len(values))
