# NICT IoT デバイスエミュレータ とは
i386上で動作する以下のCPUエミュレータです。
* Atmel SAMD21 (ARM Cortex M0＋)
* NXP KinetisK64 120MHz (ARM Cortex M4)
この上で動作するソフトウェアの検証を実施する目的で開発されています。

## フォルダ構成
```
/
├─ cortex_m0p			…　CortexM0+ エミュレータ
│  ├─emulator			…　エミュレータ本体
│  │  ├─e4arduino.c
│  │  ├─e4arduino_impl.c
│  │  ├─e4arduino_log.c
│  │  └─Makefile
│  ├─include			…　ヘッダファイル
│  │  ├─e4arduino _defs.h
│  │  ├─e4arduino.h
│  │  ├─e4arduino _impl.h
│  │  ├─e4arduino _log.h
│  │  ├─e4arduino _port.h
│  │  └─xft.h			…　libexternal.c用ヘッダファイル
│  ├─external			…　XFTトラップ対象シンボルの実装部
│  │  ├─libexternal.c		…　XFTトラップ対象シンボル実装ソースコード
│  │  │			　　（現在はサンプルとしてsensor_nodeのI2C通信部、UART通信部を実装）
│  │  ├─libglue.c		…　自動生成されるディスパッチャ・グルー関数
│  │  └─Makefile
│  ├─target			…　XFTトラップ対象アプリケーション
│  │  └─i2cm_polling.c.diff
│  ├─tools			…　各種自動化ツール
│  │  ├─lookup_symbol.py	…　エミュレータ実行結果のアドレスとシンボルを対応させるツール
│  │  ├─stage1.py		…　glue関数・dispatcher自動生成ツール（段階1）
│  │  └─stage2.py		…　glue関数・dispatcher自動生成ツール（段階2）
│  └─Makefile
├─ cortex_m4			…　CortexM4エミュレータ
│  ├─emulator			…　XFTエミュレータ本体、そのソースコード
│  │  ├─e4k64f
│  │  ├─e4k64f.c
│  │  ├─e4k64f_impl.c
│  │  ├─e4k64f_log.c
│  │  └─Makefile
│  ├─include			…　ヘッダファイル
│  │  ├─e4k64f_defs.h
│  │  ├─e4k64f.h
│  │  ├─e4k64f_impl.h
│  │  ├─e4k64f_log.h
│  │  ├─e4k64f_port.h
│  │  └─xft.h			…　libexternal.c用ヘッダファイル
│  ├─external			…　XFTトラップ対象シンボルの実装部
│  │  ├─libexternal.c		…　XFTトラップ対象シンボル実装ソースコード（現在はサンプルとしてsensor_nodeのI2C通信部、UART通信部を実装）
│  │  ├─libglue.c		…　自動生成されるディスパッチャ・グルー関数
│  │  └─Makefile
│  ├─target			…　XFTトラップ対象アプリケーション
│  │  └─i2cm_polling.c.diff
│  ├─tools			…　各種自動化ツール
│  │  ├─lookup_symbol.py	…　エミュレータ実行結果のアドレスとシンボルを対応させるツール
│  │  ├─stage1.py		…　glue関数・dispatcher自動生成ツール（段階1）
│  │  └─stage2.py		…　glue関数・dispatcher自動生成ツール（段階2）
│  └─Makefile
└─ emulation_system		…　エミュレーションシステム
├─ sensor_node		…　センサノードエミュレータ
│  ├─emulator		…　XFTエミュレータ本体、そのソースコード
│  │  ├─e4arduino.c
│  │  ├─e4arduino_impl.c
│  │  ├─e4arduino_log.c
│  │  └─Makefile
│  ├─include			…　ヘッダファイル
│  │  ├─e4arduino _defs.h
│  │  ├─e4arduino.h
│  │  ├─e4arduino _impl.h
│  │  ├─e4arduino _log.h
│  │  ├─e4arduino _port.h
│  │  └─xft.h			…　libexternal.c用ヘッダファイル
│  ├─external			…　XFTトラップ対象シンボルの実装部
│  │  ├─libexternal_sensor.c	…　センサノード実装ソースコード
│  │  ├─uuid.dat			…　センサノードにIDを振るための外部ファイル
│  │  ├─libglue_sensor.c
│  │  └─Makefile
│  ├─target				…　サンプルアプリケーション
│  │  └─e4arduino_sensor_node.c
│  ├─tools			…　各種自動化ツール
│  │  ├─lookup_symbol.py	…　エミュレータ実行結果のアドレスとシンボルと対応させるツール
│  │  ├─stage1.py		…　glue関数・dispatcher自動生成ツール（段階1）
│  │  └─stage2.py		…　glue関数・dispatcher自動生成ツール（段階2）
│  └─Makefile
├─ gateway_node		…　ゲートウェイノードエミュレータ
│  ├─emulator		…　XFTエミュレータ本体、そのソースコード
│  │  ├─e4k64f.c
│  │  ├─e4k64f_impl.c
│  │  ├─e4k64f_log.c
│  │  └─Makefile
│  ├─include			…　ヘッダファイル
│  │  ├─e4k64f _defs.h
│  │  ├─e4k64f.h
│  │  ├─e4k64f_impl.h
│  │  ├─e4k64f_log.h
│  │  ├─e4k64f_port.h
│  │  └─xft.h			…　libexternal.c用ヘッダファイル
│  ├─external			…　XFTトラップ対象シンボルの実装部
│  │  ├─libexternal_gateway.c	…　ゲートウェイノード実装ソースコード
│  │  ├─libglue_gateway.c
│  │  └─Makefile
│  ├─target				…　XFTトラップ対象アプリケーション
│  │  └─e4k64f_gateway_node.c
│  ├─tools			…　各種自動化ツール
│  │  ├─lookup_symbol.py	…　エミュレータ実行結果のアドレスとシンボルを対応させるツール
│  │  ├─stage1.py		…　glue関数・dispatcher自動生成ツール（段階1）
│  │  └─stage2.py		…　glue関数・dispatcher自動生成ツール（段階2）
│  └─Makefile
├─dummy_sensor
│  ├─dummy_sensor.c	…　Boche BME280、Adafruit TSL2561のダミー
│  └─dummy_sensor1-n.c	…　センサをごとに固有にするためにソケットをセンサノード別に取得するようにしたもの
└─http_server
   ├─http_server.c		…　ゲートウェイエミュレータからデータを受取るサーバプログラム。IPは192.168.0.100固定。
   └─local_server.c		…　エミュレータの通信デモのため上記をローカルホストにしたもの
```

## 開発・実行に必要なパッケージ
* make
* gcc
* python2.7
* python-pip
* binutils
* binutils-arm-none-eabi
* pyyaml

## エミュレータ ターゲットサンプルバイナリビルド手順
NXP社が提供しているソフトウェアをベースにしたサンプルの実行が可能です。

### 1. ソフトウェアのダウンロード
ビルドには以下のソフトウェアが必要ですのでそれぞれダウンロードを行って下さい。
なおNXPのユーザ登録が必要です。

- LPCXpress IDE v8.2.2
  - https://www.nxp.com/design/microcontrollers-developer-resources/lpcxpresso-ide-v8-2-2:LPCXPRESSO
  - Windows, macOS, Linux版がありますのでそれぞれの環境に合わせたものをご利用下さい。
  - インストール時にワークスペースの位置をきかれますがその場所を控えておいて下さい。
- LPCOpen SDK for LPCXpresso824-MAX
  - https://www.nxp.com/design/microcontrollers-developer-resources/lpcopen-libraries-and-examples/lpcopen-software-development-platform-lpc8xx:LPCOPEN-SOFTWARE-FOR-LPC8XX

### 2. SDKのインポート（本ドキュメントではmacOS版での動作を確認しています）
1. IDEの起動後、「File」メニューバーから「Import」を選択します。
2. 選択ウィンドウにて、「General」 -> 「Existing Projects into Workspace」の順に選択し「Next」を押します。
3. Import Projects画面で「Select archive file」を選択、Browseボタンを押し、NXP社からダウンロードしたSDKファイル（zip形式）を指定してください。
4. 動画面上Projects欄の一覧の中から、以下の選択肢にチェックを入れてください（それ以外の選択肢のチェックを外すことになります）。
- lpc_board_nxp_lpcxpresso_824
- lpc_chip_82x
- periph_i2cm_polling
その後 Finish ボタンを押してください。

これでインポートが終了です。

### 3. SDKのビルド
1. IDE左メニューのProject Explororの「lpc_board_nxp_lpcxpresso_824」と「lpc_chip_82x」が表示されていることを確認し、それぞれのプロジェクトを選択し、ウィンドウ上部のハンマーマーク（ビルドアイコン）の右の下向きの三角形マークを押し、「2. Release (Release Build)」を選択し、ビルドします（選択するだけでビルドが開始されます）。
2. Project Explororから「Periph_i2cm_polling」プロジェクトを右クリックし、最下部の「Properties」を選択します。
3. 左メニューから「C/C++ Build」右の「>」ボタンをクリックし下位項目開いた後「Settings」を選択、Configuration欄からReleaseを選びます。
4. 「Build steps」タブ内の「Post-build steps」のEditボタンを押します。
5. 「Post-build」コマンドを以下のように変更します。
```
arm-none-eabi-size "${BuildArtifactFileName}"
arm-none-eabi-objcopy -O ihex "${BuildArtifactFileName}" "${BuildArtifactFileBaseName}.hex"
```
6. OKボタンを押すと、確認画面が出るので、OKを押し、さらにプロジェクト設定画面に戻ったら、OKボタンを押し、設定画面を閉じます。

### 4. パッチファイルの適用とビルド
1. 「Periph_i2cm_polling」プロジェクトを右クリックし、「Rename」を選択し、New name欄からプロジェクト名を”target”に変更しOKボタンを押します。
2. 本配布物内のtargetフォルダに配置されている``i2cm_polling.c.diff``を以下のコマンドで適用します。
Windows環境には標準的にはpatchコマンドはありませんが、サードパーティ製のツールを利用するか、その時だけmacOSやLinux等の環境を使って下さい。
最後の行に特にエラーが出なければ成功です。
```
$ cd {IDEのworkspace}/target/src/
$ patch i2cm_polling.c {本配布物の展開場所}/cortex_m0p/target/i2cm_polling.c.diff
patching file i2cm_polling.c
```

3. Target プロジェクトを選択した状態でビルドアイコン右の下三角を押し、Releaseを選びビルドしてください。
4. ターゲットプロジェクトにReleaseフォルダが生成され、その中にtarget.axfとtarget.hexが配置されます。
5. この二つのファイルをコピーします。
```
$ cd {IDEのworkspace}/target/Release/
$ cp target.axf target.hex {本配布物の展開場所}/cortex_m0p/target/
$ cp target.axf target.hex {本配布物の展開場所}/cortex_m4/target/
```

## エミュレータサンプル実行
ターミナルを4つ立ち上げ、各ターミナルにおいてダミーセンサ、センサノード、ゲートウェイノード、HTTPサーバを実行します。

- ターミナル1
    $ ./emulation_system/sensor_node/dummy_sensor1-n

- ターミナル２：
    $ cd ~/emulation_system/gateway_node/
    $ make run

- ターミナル３：
    $ ./emulation_system/http_server/local_server

- ターミナル4：
    $ cd ~/emulation_system/sensor_node/
    $ make run

詳細は各Makefile内を参照してください。なおエミュレータビルドのオプションに-DENABLE_XFTを指定してください。

## ライセンス
Copyright (c) 2022 ICT Testbed Research, Development and Operations Laboratory, ICT Testbed Research and Development Promotion Center, NICT
Released under the MIT license
https://opensource.org/licenses/mit-license.php
