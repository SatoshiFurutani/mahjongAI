import mjx
import mjx.agents as agents

# 環境生成
env = mjx.MjxEnv()

# 適当なエージェント
agent = agents.RandomAgent()

# 対局開始
obs_dict = env.reset()

# 対局終了まで進める
while not env.done():

    player_id = list(obs_dict.keys())[0]
    obs = obs_dict[player_id]

    action = agent.act(obs)

    obs_dict = env.step({
        player_id: action
    })

print("=== GAME END ===")
print("Rewards =", env.rewards())

print("\n=== PAST DECISIONS ===")

decisions = env.state().past_decisions()

for i, (obs, action) in enumerate(decisions):

    print(f"\n----- Decision {i} -----")

    # 行動
    print("ACTION:")
    print(action.to_json())

    # 手牌
    hand = obs.curr_hand()

    print("\nHAND:")
    print(hand.to_json())

    # シャンテン数
    print("\nSHANTEN:")
    print(hand.shanten_number())

    # 有効牌
    print("\nEFFECTIVE DRAWS:")
    print(hand.effective_draw_types())

    # 有効打牌
    print("\nEFFECTIVE DISCARDS:")
    print(hand.effective_discard_types())