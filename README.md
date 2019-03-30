#ECHONET Lite Lite
Raspberry PiとWi-SUNモジュールを使用して、スマートメーターから家中の瞬間電力計測値を取得します。
また、取得した瞬間電力計測値をgpio制御で接続した7セグに表示します。
ECHONET Liteと書いていますが、厳密な規約に則っているわけではなく、簡易版(Lite)であるため、実験用 かつ 自己責任にてお使いください。

##Raspberry Piについて
動作確認しているRaspberry Piは、Raspberry Pi 1 Type Bになります。
OSは、[RASPBIAN Debian Wheezy](https://www.raspberrypi.org/downloads/raspbian/)（2015-05-05-raspbian-wheezy.zip）を使用しました。

##Wi-SUNモジュールについて
使用したWi-SUNモジュールは、ROHMのBP35A1を使用しています。
プログラム内では、USB接続したBP35A1とシリアル通信し、BP35A1のSKコマンドを発行することで、スマート>メーターと接続＆情報取得しています。
- [Wi-SUN対応無線モジュール - BP35A1 | ローム株式会社 - ROHM Semiconductor](http://www.rohm.co.jp/web/japan/products/-/product/BP35A1)

##GPIO制御について
WiringPiを使用しています。
インストールされていない場合は、次のコマンドでインストールしてください。
`$ sudo apt-get install libi2c-dev`

##7セグについて
すべて秋月電子通商で揃えました。
主要部品は次の通りです。
- [ダイナミック接続４桁高輝度黄色７セグメントＬＥＤ表示器　カソードコモン　カソード共通接続](http://akizukidenshi.com/catalog/g/gI-04423/)
- [カーボン抵抗（炭素皮膜抵抗）　１／６Ｗ　７５Ω　（１００本入）](http://akizukidenshi.com/catalog/g/gR-16750/)
- [トランジスター２ＳＣ１８１５Ｙ（６０Ｖ１５０ｍＡ）（１０個入テーピング品）](http://akizukidenshi.com/catalog/g/gI-04268/)
- [２ｘ１３（２６Ｐ）両端コネクタ付ＩＤＣリボンケーブル（フラットケーブル）](http://akizukidenshi.com/catalog/g/gC-06322/)
- [ピンヘッダ　２×１３　（２６Ｐ）](http://akizukidenshi.com/catalog/g/gC-00079/)

7セグの制御にはgpioを12本(7セグ+1ドットの8つのLEDと4桁制御)使用しています。
gpio7seg.hで使用するピンを適宜変更してください。

## 使い方
### コンパイル
make
### 実行
sm_pwd=aaaaaaaaaaaa sm_rbid=bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb sudo -E ./ellite > ~/smartmeter.json 2> ~/smartmeter.log
### 実行例
- 標準出力にはjson形式で時刻と消費電力の瞬間値が出る
``` sh:stdout.txt
{"datetime":"2016/02/20 16:22:54", "Watt":1493, "method": "smartmeter"}
{"datetime":"2016/02/20 16:22:57", "Watt":1490, "method": "smartmeter"}
{"datetime":"2016/02/20 16:23:00", "Watt":1488, "method": "smartmeter"}
{"datetime":"2016/02/20 16:23:03", "Watt":1488, "method": "smartmeter"}
```
- 標準エラーには接続時に取得した情報などが出る
``` sh:stderr.txt
```

### 想定使用例
下記のようにすることで約３秒毎に使用電力量の瞬時値がmqttサーバに送られる
``` sh:sample-usage.txt
sudo sm_pwd=aaaaaaaaaaaa sm_rbid=bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb ./ellite | python sendToMqtt.py
```
