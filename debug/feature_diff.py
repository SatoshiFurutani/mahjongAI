#一巡進めてどのチャネルが変わるか調べる
import mjx
import numpy as np

env = mjx.MjxEnv()

obs_dict = env.reset()

obs = list(obs_dict.values())[0]

before = obs.to_features("mjx-small-v0")

actions = {}

for pid, o in obs_dict.items():
    actions[pid] = o.legal_actions()[0]

obs_dict = env.step(actions)

obs = list(obs_dict.values())[0]

after = obs.to_features("mjx-small-v0")

print("changed channels")

for ch in range(before.shape[0]):

    if np.any(before[ch] != after[ch]):

        print("channel", ch)

        print("before")
        print(before[ch])

        print("after")
        print(after[ch])

        print()