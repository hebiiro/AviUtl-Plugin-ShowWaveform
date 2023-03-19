﻿# AviUtl プラグイン - アイテム内音声波形

拡張編集ウィンドウの音声アイテム内に音声波形を表示します。
[最新バージョンをダウンロード](../../releases/latest/)

![アイテム内音声波形 1 0 0](https://user-images.githubusercontent.com/96464759/164222503-7f0e446a-5a7e-42b1-b052-0a500594eee5.png)

## 導入方法

以下のファイルを AviUtl の Plugins フォルダに入れてください。
* ShowWaveform (フォルダ)
* ShowWaveform.auf
* ShowWaveform.ini
* lwinput.aui (L-SMASH Works)

## 使用方法

更新モードを 1 に設定すると自動的に音声波形が表示されるようになります。

### キャッシュをクリア

1. 「キャッシュをクリア」ボタンを押します。
2. すべての音声アイテムの音声波形が再計算されます。

## 設定方法

ShowWaveform.ini をテキストエディタで編集してから AviUtl を起動します。

```ini
[Config]
penColor=0,255,255
; 音声波形の縁の色を指定します。
; r,g,b、#rgb、#rrggbb の形式、または -1 を指定できます。
; -1 を指定すると描画が抑制されます。

brushColor=0,255,255
; 音声波形の塗りつぶし色を指定します。
; penColor と同じ形式で指定できます。

scale=200
; 音声波形を描画するサイズを指定します。
; 数値が大きいほど音声波形が大きく描画されます。

showType=1
; 音声波形の表示タイプを指定します。
; 0 を指定すると音声波形が鏡面描画されます。※他の表示タイプより重いです。
; 1 を指定すると音声波形が下から上に描画されます。
; 2 を指定すると音声波形が上から下に描画されます。

updateMode=1
; 音声波形の更新モードを指定します。
; 0 - 音声波形を更新しません。
; 1 - 音声波形を自動的に更新します。

showWaveform=1
; 音声波形を表示するか指定します。
; 0 を指定すると音声波形を表示しません。
; 1 を指定すると音声波形を表示します。

showText=1
; テキストを表示するか指定します。
; 0 を指定するとテキストを表示しません。
; 1 を指定するとテキストを表示します。

noScrollText=1
; テキストのスクロールを無効化するか指定します。
; 0 を指定するとテキストをスクロールします。
; 1 を指定するとテキストをスクロールしません。
```

## 更新履歴

* 3.0.0 - 2023/03/19 外部プロセスで音声データの計算を行うように変更
* 2.3.0 - 2022/08/08 テキストのスクロールを無効化する機能を追加
* 2.2.0 - 2022/07/30 プレビュー中は何もしないように変更
* 2.1.0 - 2022/05/22 縮小率の上限を 4000 に変更
* 2.0.0 - 2022/04/26 更新モードを変更
* 1.0.1 - 2022/04/21 保存中は何もしないように修正
* 1.0.0 - 2022/04/20 初版

## 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r42 https://scrapbox.io/ePi5131/patch.aul

## クレジット

* Microsoft Research Detours Package https://github.com/microsoft/Detours
* aviutl_exedit_sdk https://github.com/ePi5131/aviutl_exedit_sdk
* Common Library https://github.com/hebiiro/Common-Library

## 作成者情報
 
* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://twitter.com/io_hebiiro

## 免責事項

この作成物および同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
