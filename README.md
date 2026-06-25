# mahjongAI

1. Pythonバージョン確認
python3 --version

MJX 0.1.0 は Python 3.9 で動作確認済み。

2. Python3.9のインストール（未導入の場合）

Ubuntuの場合

sudo apt update

sudo apt install python3.9 python3.9-venv

確認

python3.9 --version
3. GitHubからプロジェクト取得
git clone <GitHubのURL>

cd mahjongAI

例

git clone https://github.com/ユーザー名/mahjongAI.git

cd mahjongAI
4. 仮想環境作成
python3.9 -m venv mjx39

有効化

source mjx39/bin/activate

確認

which python

出力例

/home/ユーザー名/mahjongAI/mjx39/bin/python
5. pip更新
pip install --upgrade pip setuptools wheel
6. 必要ライブラリ導入

requirements.txt がある場合

pip install -r requirements.txt
requirements.txt が無い場合
pip install mjx
7. インストール確認

Python起動

python

確認

import mjx

print(mjx)

正常例

<module 'mjx' from '.../site-packages/mjx/__init__.py'>

終了

exit()
8. 動作テスト

test.py を実行

python test.py

または

python surv.py

エラーが出なければ環境構築完了。

作業開始時

毎回以下を実行

cd ~/mahjongAI

source mjx39/bin/activate

確認

which python

出力例

.../mahjongAI/mjx39/bin/python
GitHubとの同期

研究室PCで取得

git pull

変更を保存

git add .

git commit -m "update"

git push
注意事項
mjx39/ は GitHub にアップロードしない
.gitignore に mjx39/ を追加する
Python 3.12 では MJX 0.1.0 の導入に失敗するため Python 3.9 を使用する
研究室PCと自宅PCでそれぞれ仮想環境を作成する
初回セットアップまとめ
git clone <GitHub URL>

cd mahjongAI

python3.9 -m venv mjx39

source mjx39/bin/activate

pip install --upgrade pip setuptools wheel

pip install -r requirements.txt

python test.py

これだけで自宅で動いているMJX環境を研究室PC上に再現できます。
