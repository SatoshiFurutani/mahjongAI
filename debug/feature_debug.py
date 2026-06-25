#特徴量の意味
import mjx
import numpy as np

env = mjx.MjxEnv()

obs_dict = env.reset()

obs = list(obs_dict.values())[0]

small = obs.to_features("mjx-small-v0")
han22 = obs.to_features("han22-v0")

print("small shape =", small.shape)
print("han22 shape =", han22.shape)

print("\n===== MJX SMALL =====")

for i in range(small.shape[0]):

    nz = np.nonzero(small[i])[0]

    print(
        f"channel {i:2d}",
        "count=",
        len(nz),
        "indices=",
        nz
    )

print("\n===== HAN22 =====")

for i in range(han22.shape[0]):

    nz = np.nonzero(han22[i])[0]

    if len(nz) > 0:

        print(
            f"channel {i:2d}",
            "count=",
            len(nz),
            "indices=",
            nz
        )