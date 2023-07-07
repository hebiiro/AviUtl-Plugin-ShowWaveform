# AviUtl プラグイン - アイテム内音声波形

拡張編集ウィンドウの音声アイテム内に音声波形 (厳密には音声波形ではなく各フレーム毎の音量のグラフ) を表示します。<br>
[最新バージョンをダウンロード](../../releases/latest/)

![アイテム内音声波形 1 0 0](https://user-images.githubusercontent.com/96464759/164222503-7f0e446a-5a7e-42b1-b052-0a500594eee5.png)

## 導入方法

以下のファイルを AviUtl の Plugins フォルダに入れてください。
* ShowWaveform.auf
* ShowWaveform (フォルダ)
* lwinput.aui (L-SMASH Works)

## 使用方法

更新モードを「更新する」に設定すると自動的に音声波形が表示されるようになります。

### キャッシュをクリアする

1. 「キャッシュをクリア」ボタンを押します。
2. すべての音声アイテムの音声波形が再計算されます。

* 更新モードを「更新しない」に設定してからキャッシュをクリアすると音声波形がない状態と同じになります。

### 全体の音声波形を表示する

1. 「全体の音声波形を表示」ボタンを押します。
2. 全体の音声波形ウィンドウが表示されます。同時に全体の音声波形が再計算されます。

### 全体の音声波形ウィンドウ

* グラフの部分をクリックすると現在フレームを変更できます。
* 時間目盛り上で左右にドラッグすると ```ズーム``` を変更できます。
* 音量メモリ上で上下にドラッグすると ```最大音量 (RMSモード時は下半分が最小RMS、上半分が最大RMS)``` を変更できます。
* マウスホイールすると ```ズーム``` を変更できます。

### マルチプロセスを有効化する

Config.ini の ```[Config]maxReaderCount``` で同時に実行するプロセスの最大数を指定します。

### 全体の音声波形ウィンドウのコンテキストメニュー

* 全体の音声波形ウィンドウで右クリックしてコンテキストメニューを表示します。
  * ```RMS``` 描画モードを「RMS」に設定します。RMSが描画されるようになります。
  * ```中央``` 描画モードを「中央」に設定します。音量が中央から線対称に描画されます。
  * ```ボトムアップ``` 描画モードを「ボトムアップ」に設定します。音量が下から上に描画されます。
  * ```トップダウン``` 描画モードを「トップダウン」に設定します。音量が上から下に描画されます。
  * ```水平目盛りを上下に表示``` 時間目盛りを上下両方に表示します。
  * ```水平目盛りを上のみに表示``` 時間目盛りを上のみに表示します。
  * ```水平目盛りを下のみに表示``` 時間目盛りを下のみに表示します。
  * ```水平目盛りをサイドだけに表示``` 時間目盛りをグラフのサイドにだけに表示します。
  * ```水平目盛りを一直線に表示``` 時間目盛りをグラフを横切るように一直線に表示します。
  * ```垂直目盛りを左右に表示``` 音量目盛りを左右両方に表示します。
  * ```垂直目盛りを左のみに表示``` 音量目盛りを左のみに表示します。
  * ```垂直目盛りを右のみに表示``` 音量目盛りを右のみに表示します。
  * ```垂直目盛りをサイドだけに表示``` 音量目盛りをグラフのサイドにだけに表示します。
  * ```垂直目盛りを一直線に表示``` 音量目盛りをグラフを横切るように一直線に表示します。
  * ```その他の設定``` 「全体の音声波形の設定」ウィンドウを表示します。

### 全体の音声波形の設定

* ```BPMを表示``` チェックを入れるとBPMを描画するようになります。
* ```基準フレーム``` BPMを描画するとき基準になるフレーム番号を指定します。
* ```BPM``` BPMを指定します。
* ```拍子``` 拍子を指定します。(楽譜に書いてある上の数字)
* ```分``` 分を指定します。(楽譜に書いてある下の数字)
* ↑ここまでの設定はプロジェクト毎に保存されます。
* ```最大音量``` 描画可能な最大音量を指定します。(グラフの最大値)
* ```基準音量``` グラフで強調表示したい音量を指定します。
* ```最小RMS``` 描画可能な最小RMSを指定します。(グラフの最小値)
* ```最大RMS``` 描画可能な最大RMSを指定します。(グラフの最大値)
* ```基準RMS``` グラフで強調表示したいRMSを指定します。
* ```ズーム``` 時間方向のズーム値を指定します。

## 詳細な設定方法

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
; 0 を指定すると音声波形が中央に描画されます。※他の表示タイプより重いです。
; 1 を指定すると音声波形が下に描画されます。
; 2 を指定すると音声波形が上に描画されます。

updateMode=1
; 音声波形の更新モードを指定します。
; 0 - 音声波形を更新しません。
; 1 - 音声波形を自動的に更新します。
; 2 - 音声波形を自動的に更新しますが、再生中は更新しません。

xorMode=1
; 音声波形の描画モードを指定します。
; 0 - 通常の上書き描画です。
; 1 - XOR で描画します。
; 2 - NOT XOR で描画します。
; 3 - NOT で描画します。

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

behind=1
; 音声波形をテキストの後ろに描画するか指定します。
; 0 を指定するとテキストの前に描画します。
; 1 を指定するとテキストの後ろに描画します。
```

## 全体の音声波形の設定方法

Config.ini をテキストエディタで編集します。AviUtl が起動中の場合は変更が自動的に反映されます。
```ini
[Config]
maxReaderCount=2 ; 同時実行するプロセスの数。大きいほどシステムリソース(瞬間的なメモリ増、及び複数ファイルへの同時アクセス)を消費するが、その分高速化が期待できる。
limitVolume=100 ; グラフで描画する最大音量を指定する。(0~100)
baseVolume=50 ; グラフで描画する基準音量を指定する。(0~100)
minRMS=-33 ; グラフで描画するRMSの最小値を指定する。
maxRMS=14 ; グラフで描画するRMSの最大値を指定する。
baseRMS=0 ; グラフで描画するRMSの基準値を指定する。
zoom=0 ; 追加する描画幅をピクセル単位で指定する。
mode=rms ; 描画モードを指定する。rms, center, bottomUp, topDown のいずれか。

[Design]
fontDefault=C:\Windows\Fonts\segoeui.ttf
fontDefault2=C:\Windows\Fonts\meiryo.ttc
image.fileName= ; 背景に使用する画像ファイルのパス。
image.offset.x=0
image.offset.y=0
image.align.x=center ; left, right, center のいずれか。
image.align.y=center ; top, bottom, center のいずれか。
image.scaleMode=normal ; normal, fit, crop のいずれか。
image.scale=1.000000
image.angle=0.000000
image.alpha=1.000000
background.fill.color1=51, 51, 51, 255 ; 縦方向のグラデーションの上側。
background.fill.color2=26, 26, 26, 255 ; 縦方向のグラデーションの下側。
body.margin.x=40 ; 左右のマージン。垂直目盛りが描画される範囲。
body.margin.y=30 ; 上下のマージン。水平目盛りが描画される範囲。
body.stroke.style=solid ; ストロークのラインスタイルを指定する。solid, dashed, dotted, glow のいずれか。
body.stroke.width=1 ; ストロークの幅を指定する。
body.stroke.color=255, 255, 255, 128 ; ストロークの色を指定する。
scale.horz.minUnitWidth=100 ; 時間目盛りの主罫線の最小間隔をピクセル単位で指定する。
scale.horz.primary.height=10 ; 時間目盛りの主罫線の高さをピクセル単位で指定する。
scale.horz.primary.stroke.style=solid
scale.horz.primary.stroke.width=1
scale.horz.primary.stroke.color=255, 255, 255, 204
scale.horz.primary.text.height=16 ; フォントの大きさを指定する。
scale.horz.primary.text.color=255, 255, 255, 204
scale.horz.primary.text.shadow.dilate=1.000000 ; 縁取り量をピクセル単位の整数で指定する。offset の影響を受ける。
scale.horz.primary.text.shadow.blur=1.000000 ; ぼかし量をピクセル単位で指定する。
scale.horz.primary.text.shadow.offset.x=0 ; 影を描画する位置を指定する。
scale.horz.primary.text.shadow.offset.y=0
scale.horz.primary.text.shadow.color=0, 0, 0, 128
scale.horz.secondary.height=5 ; 時間目盛りの副罫線の高さをピクセル単位で指定する。
scale.horz.secondary.stroke.style=solid
scale.horz.secondary.stroke.width=1
scale.horz.secondary.stroke.color=255, 255, 255, 102
scale.vert.width=10 ; 音量目盛りの幅をピクセル単位で指定する。
scale.vert.text.height=16
scale.vert.text.color=255, 255, 255, 204
scale.vert.text.shadow.dilate=1.000000
scale.vert.text.shadow.blur=1.000000
scale.vert.text.shadow.offset.x=0
scale.vert.text.shadow.offset.y=0
scale.vert.text.shadow.color=0, 0, 0, 128
scale.vert.stroke.style=solid
scale.vert.stroke.width=1
scale.vert.stroke.color=255, 255, 255, 102
scale.vert.base.stroke.style=solid
scale.vert.base.stroke.width=1
scale.vert.base.stroke.color=255, 0, 0, 128
graph.fill.color1=255, 255, 0, 102 ; 縦方向のグラデーションの上側。
graph.fill.color2=255, 255, 0, 102 ; 縦方向のグラデーションの下側。
graph.stroke.style=solid
graph.stroke.width=1
graph.stroke.color=255, 255, 0, 51
graph.current.stroke.style=solid
graph.current.stroke.width=1
graph.current.stroke.color=0, 255, 0, 102
graph.hot.stroke.style=solid
graph.hot.stroke.width=1
graph.hot.stroke.color=255, 0, 255, 102
graph.last.stroke.style=solid
graph.last.stroke.width=1
graph.last.stroke.color=255, 255, 255, 128
bpm.primary.stroke.style=solid ; ↓ BPM の主罫線の設定。
bpm.primary.stroke.width=1
bpm.primary.stroke.color=255, 255, 255, 128
bpm.primary.text.height=12
bpm.primary.text.color=255, 255, 255, 204
bpm.primary.text.shadow.dilate=1.000000
bpm.primary.text.shadow.blur=1.000000
bpm.primary.text.shadow.offset.x=0
bpm.primary.text.shadow.offset.y=0
bpm.primary.text.shadow.color=0, 0, 0, 128
bpm.secondary.stroke.style=dashed ; ↓ BPM の主罫線の設定。
bpm.secondary.stroke.width=1
bpm.secondary.stroke.color=255, 255, 255, 128
```

## 更新履歴

* 7.0.0 - 2023/07/07 BPM 表示機能を追加
* 6.4.1 - 2023/04/14 音声アイテムにフィルタを付けたときの問題を修正
* 6.4.0 - 2023/03/30 音声波形をテキストより後ろに描画する機能を追加
* 6.3.0 - 2023/03/29 全体の音声波形ウィンドウの位置を記憶するように修正
* 6.2.0 - 2023/03/27 描画モードを変更できる機能を追加
* 6.1.0 - 2023/03/27 描画幅を変更できる機能を追加
* 6.0.0 - 2023/03/26 音声データの計算をマルチプロセスで行える機能を追加
* 5.0.0 - 2023/03/25 全体の音声波形を表示する機能を追加、24bit PCM データに対応
* 4.0.1 - 2023/03/22 32bit PCM データに対応
* 4.0.0 - 2023/03/21 UI を変更
* 3.2.0 - 2023/03/21 XOR モードを追加
* 3.1.0 - 2023/03/20 ズームアウト時に描画が重くなる問題に対応
* 3.0.0 - 2023/03/19 外部プロセスで音声データの計算を行うように変更
* 2.3.0 - 2022/08/08 テキストのスクロールを無効化する機能を追加
* 2.2.0 - 2022/07/30 プレビュー中は何もしないように変更
* 2.1.0 - 2022/05/22 縮小率の上限を 4000 に変更
* 2.0.0 - 2022/04/26 更新モードを変更
* 1.0.1 - 2022/04/21 保存中は何もしないように修正
* 1.0.0 - 2022/04/20 初版

## 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r43 謎さうなフォーク版29 https://scrapbox.io/nazosauna/patch.aul

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
