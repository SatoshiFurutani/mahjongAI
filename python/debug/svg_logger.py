#局終了時のSVG保存
import mjx
import os

env = mjx.MjxEnv()

obs_dict = env.reset()

kyoku = 0

os.makedirs("svg", exist_ok=True)

while not env.done():

    actions = {}

    for pid, obs in obs_dict.items():
        actions[pid] = obs.legal_actions()[0]

    obs_dict = env.step(actions)

    if len(obs_dict) == 0:

        state = env.state()

        filename = f"svg/kyoku_{kyoku}.svg"

        state.save_svg(filename)

        print("saved", filename)

        kyoku += 1

print(env.rewards())