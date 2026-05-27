# Flask（Webサーバー）関連の機能を読み込む
from flask import Flask, request, abort

# LINE Botを操作するためのクラスを読み込む
from linebot import LineBotApi, WebhookHandler

# LINEのメッセージイベント関連のクラスを読み込む
from linebot.models import MessageEvent, TextMessage, TextSendMessage

# HTTP通信を行うためのライブラリ
import requests

#乱数
import random

# Flaskアプリ（Webサーバー本体）を作成
app = Flask(__name__)


# ===== LINE設定 =====

# LINE Botのアクセストークン（APIを使うための鍵）
CHANNEL_ACCESS_TOKEN = "tVSF1dW04X3CWjsVBafV0oCHCeyW/Ojqy+MGP58o3Qq7Myfk4NYkkjO57ATqH/mnwWGa40uz8nrWGG+c6FGb0iFK4xMeGDus28HaiTM3PwxC0CXEFLl1B6SBfXDMdhd4/CA8PbvXk5bPakWtxECVbgdB04t89/1O/w1cDnyilFU="

# LINE Botのシークレット（認証用）
CHANNEL_SECRET = "76df9e5c25495a0ebd0c21b215eeb394"

# LINEのAPIを操作するためのインスタンスを作成
line_bot_api = LineBotApi(CHANNEL_ACCESS_TOKEN)

# Webhook（LINEからの通知）を処理するためのインスタンス
handler = WebhookHandler(CHANNEL_SECRET)


# ===== Nature Remo設定 =====

# Nature Remo APIを使うためのトークン
NATURE_TOKEN = "ory_at_EHrXRUgr38W7nqNIGE3Nnwyno3fn_tWwKDKLkY2UEO8.kTxGvXkciMN02lEnLRMqLcCyLrIkSDjgkJNAmjpQs8Q"


# ===== Spotify設定 =====

# Spotify APIを使うためのアクセストークン
SPOTIFY_TOKEN = "BQCKDaQWnqFybIZZjNR5umo5C85HKOLO1buqDSMKwe2ks1E86SKX4J1TcViQKLJrZTLOeYxVNrXTdRWkxwt6DrHB35IvfPVQ8yDrY675OTjl9zO9iT2d6bGwnWQ5lgD3vKoCj8X9z2M_iOIuqgcAy6qAuasRMFZJTcyuIa4c9yC5q9NRFICsJZtxGw6g95-zjLrKnx2S0Bs1xxiU2r8Ddg6dvCgsKiQnurG1RRtalWPUzTkVK9s3UXH2zew"


# ===== Webhook =====

# 「/callback」というURLにPOSTリクエストが来たときの処理
@app.route("/callback", methods=['POST'])
def callback():
    # LINEから送られてくる署名（改ざん防止用）
    signature = request.headers['X-Line-Signature']

    # リクエストの中身（メッセージなど）を文字列として取得
    body = request.get_data(as_text=True)

    try:
        # 受信したデータをLINEのhandlerで処理
        handler.handle(body, signature)
    except:
        # エラーが出たら400エラーを返す
        abort(400)

    # 正常終了したらOKを返す
    return 'OK'


# ===== 温度・湿度取得 =====

# Nature Remoから温度・湿度を取得する関数
def get_temperature_humidity():
    # 認証ヘッダー（トークンを付ける）
    headers = {"Authorization": f"Bearer {NATURE_TOKEN}"}

    # Nature Remo APIにリクエスト送信
    res = requests.get("https://api.nature.global/1/devices", headers=headers)

    # JSON形式のデータをPythonで扱える形に変換
    data = res.json()

    # 気温データを取得
    temp = data[0]["newest_events"]["te"]["val"]

    # 湿度データを取得
    humidity = data[0]["newest_events"]["hu"]["val"]

    #照度データを取得
    illumination = data[0]["newest_events"]["il"]["val"]

    # 気温と湿度を返す
    return temp, humidity, illumination


# ===== 曲選択 =====

# 気温と湿度から再生する曲を決める関数
def choose_song(temp, humidity, illumination):
    r=random.randint(1,3)
    if illumination < 30:
        if r == 1:
            
            return "spotify:track:4oE7MyJhqSD3BaHRpNs8Nl"
        elif r == 2:
            return "spotify:track:2cxcqKwQtfKq7giphxjzOu"
        else:
            return "spotify:track:7loLSr7JUn78pLHitHr89Z"
        
    elif temp > 28:
        if r == 1:
            return "spotify:track:6rDJe5gVgAunlkaSY1vnAF"
        elif r == 2:
            return "spotify:track:0a3587n25Xhw098UeEbeJq"
        else:
            return "spotify:track:7CgxrGWsDarS8ZvGAQG0jE"
        
    elif temp < 12 or humidity < 30:
        if r == 1:
            return "spotify:track:1Vimcaq6kgTq6xgHSnr4pS"
        elif r == 2:
            return "spotify:track:6EvrfdvQ254FkKbNOnVXuQ"
        else:
            return "spotify:track:6Zyizxfz4NXdCgVLwhuBNw"

    # それ以外（普通）
    else:
        if r == 1:
            return "spotify:track:7dTW21YAJxnAOiUh7Rtu9S"
        elif r == 2:
            return "spotify:track:5LkWXxDxUTwEVHL0pfKL55"
        else:
            return "spotify:track:60ReNXcPcsVFd7nUuQw5HZ"


# ===== Spotify再生 =====

# 指定した曲をSpotifyで再生する関数
def play_spotify(song_uri):

    headers = {
        "Authorization": f"Bearer {SPOTIFY_TOKEN}",
        "Content-Type": "application/json"
    }

    data = {
        "uris": [song_uri]
    }

    response = requests.put(
        "https://api.spotify.com/v1/me/player/play",
        headers=headers,
        json=data
    )

    print("Spotify status:", response.status_code)
    print("Spotify response:", response.text)

#def play_spotify(song_uri):

    # HTTPヘッダー（認証情報など）
#    headers = {
 #       "Authorization": f"Bearer {SPOTIFY_TOKEN}",  # トークン
  #      "Content-Type": "application/json"           # データ形式
   # }

    # 再生する曲の情報
    #data = {
     #   "uris": [song_uri]  # 再生する曲（URI）
    #}

    # Spotify APIに再生リクエストを送る
    #requests.put(
     #   "https://api.spotify.com/v1/me/player/play",
      #  headers=headers,
       # json=data
    #)


# ===== メッセージ処理 =====

# LINEでメッセージが送られてきたときの処理
@handler.add(MessageEvent, message=TextMessage)
def handle_message(event):

    # ユーザーが送ったメッセージを取得
    user_message = event.message.text

    # 「再生」と送られた場合
    if user_message == "再生":
        try:
            # ① 温度・湿度取得
            temp, humidity, illumination = get_temperature_humidity()

            # ② 曲決定
            song_uri = choose_song(temp, humidity, illumination)

            # ③ Spotify再生
            play_spotify(song_uri)

            # 成功メッセージ作成
            reply = f"再生しました！\n気温:{temp}℃ 湿度:{humidity}%"
        except Exception as e:
            reply = f"エラー: {str(e)}"
            print(e)
        #except Exception as e:
            # エラー時のメッセージ
            #reply = "エラーが発生しました"

    else:
        # 「再生」以外のときの返答
        reply = "「再生」と送ると音楽を流します"

    # LINEに返信を送る
    line_bot_api.reply_message(
        event.reply_token,
        TextSendMessage(text=reply)
    )


# ===== サーバー起動 =====

# このファイルが直接実行されたときだけ動く
if __name__ == "__main__":

    # Webサーバーを起動（外部アクセスOK、ポート5000）
    app.run(host="0.0.0.0", port=5000)
