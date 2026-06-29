import mjx
import json
from collections import defaultdict

env = mjx.MjxEnv()

event_types = set()
action_table = defaultdict(set)

round_no = 0
max_steps = 200
step_count = 0

while not env.done() and step_count < max_steps:

    obs_dict = env.reset() if round_no == 0 else obs_dict

    while len(obs_dict) > 0 and step_count < max_steps:
        step_count += 1

        actions = {}

        for pid, obs in obs_dict.items():

            # Event収集
            for e in obs.events():
                try:
                    event_types.add(json.loads(e.to_json())["type"])
                except:
                    pass

            # Action対応表収集
            for a in obs.legal_actions():

                try:
                    idx = a.to_idx()
                except:
                    idx = -1

                try:
                    tile = a.tile().type()
                except:
                    tile = None

                action_table[idx].add(
                    (str(a.type()), str(tile))
                )

            # ランダム選択
            actions[pid] = obs.legal_actions()[0]

        obs_dict = env.step(actions)

    round_no += 1

if step_count >= max_steps:
    print(f"\n=== STOPPED BY MAX_STEPS ({max_steps}) ===")

print("\n=== EVENT TYPES ===")

for e in sorted(event_types):
    print(e)

print("\n=== ACTION IDX TABLE ===")

for idx in sorted(action_table.keys()):

    print("\nIDX =", idx)

    for item in sorted(action_table[idx]):
        print(item)

print("\n=== REWARDS ===")
print(env.rewards())
