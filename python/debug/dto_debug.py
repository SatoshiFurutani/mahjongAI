import json
import sys
from pathlib import Path

import mjx

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mjx_bridge import observation_to_dto


env = mjx.MjxEnv()
obs_dict = env.reset()

player_id = list(obs_dict.keys())[0]
obs = obs_dict[player_id]

dto = observation_to_dto(obs, env.state())

print(json.dumps(dto, ensure_ascii=False, indent=2))
