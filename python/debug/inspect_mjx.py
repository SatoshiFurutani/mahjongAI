#仕様一括表示
import json
import mjx

env = mjx.MjxEnv()

# 初期局面
obs_dict = env.reset()

player_id = list(obs_dict.keys())[0]
obs = obs_dict[player_id]

print("=" * 60)
print("PLAYER")
print("=" * 60)
print(player_id)

print("\n" + "=" * 60)
print("OBSERVATION")
print("=" * 60)

print("who =", obs.who())
print("dealer =", obs.dealer())
print("round =", obs.round())
print("tens =", obs.tens())
print("doras =", obs.doras())

print("\n" + "=" * 60)
print("HAND")
print("=" * 60)

hand = obs.curr_hand()

print("shanten =", hand.shanten_number())

print("\nhand json")
print(hand.to_json())

print("\nclosed tiles")
for t in hand.closed_tiles():
    print(
        f"id={t.id():3d}",
        f"type={t.type()}",
        f"num={t.num()}",
        f"red={t.is_red()}"
    )

print("\neffective draws")
print(hand.effective_draw_types())

print("\neffective discards")
print(hand.effective_discard_types())

print("\n" + "=" * 60)
print("LEGAL ACTIONS")
print("=" * 60)

actions = obs.legal_actions()

for i, a in enumerate(actions):
    print(f"[{i}]")
    print("json =", a.to_json())
    print("type =", a.type())

    try:
        print("tile =", a.tile().type())
    except:
        pass

    try:
        print("idx =", a.to_idx())
    except:
        pass

    print()

print("\n" + "=" * 60)
print("FEATURES")
print("=" * 60)

try:
    x = obs.to_features("mjx-small-v0")
    print("mjx-small-v0")
    print("shape =", x.shape)
except Exception as e:
    print(e)

print()

try:
    x = obs.to_features("han22-v0")
    print("han22-v0")
    print("shape =", x.shape)
except Exception as e:
    print(e)

print("\n" + "=" * 60)
print("EVENTS")
print("=" * 60)

for e in obs.events():
    print(e.to_json())

print("\n" + "=" * 60)
print("STATE")
print("=" * 60)

state = env.state()

print(type(state))

state_json = json.loads(state.to_json())

print("keys =", state_json.keys())

print("\nhiddenState keys")
print(state_json["hiddenState"].keys())

print("\nwall length")
print(len(state_json["hiddenState"]["wall"]))

print("\nura dora")
print(state_json["hiddenState"]["uraDoraIndicators"])

print("\npublicObservation keys")
print(state_json["publicObservation"].keys())

print("\n" + "=" * 60)
print("ENUMS")
print("=" * 60)

print("\nTileType")
for t in mjx.TileType:
    print(t)

print("\nActionType")
for a in mjx.ActionType:
    print(a)

print("\n" + "=" * 60)
print("ENV")
print("=" * 60)

print(dir(env))

print("\n" + "=" * 60)
print("DONE")
print("=" * 60)