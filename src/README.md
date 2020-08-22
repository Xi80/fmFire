# fmSynth16

STM32とYMF825Boardを使用したスタンドアロンハードウェアMIDI音源。



## ライセンス

fmSynth16はmbedフレームワーク上で動作します、それらの部分はもともとのライセンスに準じます。

fmSynth16のソースコード自体は[CC BY-NC-ND](https://creativecommons.org/licenses/by-nc-nd/4.0/deed.ja)です。



## 概要

STM32F303K8T6または同等以上の性能を持ったSTM32 Nucleoボード上で動作するハードウェアMIDI音源です。

音源部にはFM音源であるYMF825を使用しています。

本体にリズム音源以外のGM音色を内蔵しており、PCを介さずスタンドアロンでの使用が可能です。

## サンプル(演奏風景)

https://soundcloud.com/user-273146202/ethereal-house-mix-with-ymf825-midi-synthesiser

## 諸元

> - GM Lite相当(ただし、リズム音源は除く)
> - fmFire Ver2.2d時点での情報です
>   - MCU:STM32F303K8T6(72MHz,RAM12KB,Flash64KB)
>   - チャネル数:16
>   - 最大同時発音数:16
>   - 音源IC:YMF825
>   - インプリメンテーションチャートは別途記載
>   - I/F:3.5mmジャック、MIDI IN
>   - 電源:12v 1.0A以上



## 開発環境

> OS : Windows10 x64
>
> PC : HP Z400
>
> IDE: VSCode + PlatformIO




## MIDIインプリメンテーションチャート

| ファンクション                                               | 送信                                         | 受信                                         | 備考                                                         |
| ------------------------------------------------------------ | -------------------------------------------- | -------------------------------------------- | ------------------------------------------------------------ |
| **ベーシックチャネル**<br>電源ON時<br>設定可能               | <br>X<br>X                                   | <br>1-16<br>1-16                             | 記憶されない                                                 |
| **モード**<br>電源ON時<br>メッセージ<br>代用                 | <br>X<br>\*\*\*\*\*\*                        | <br>3<br>                                    |                                                              |
| **ノートナンバー**<br><br>音域                               | <br>X<br>X                                   | <br>0-127<br>音色依存                        |                                                              |
| **ベロシティ**<br>ノート・オン<br>ノート・オフ<br>           | <br>X<br>X                                   | <br>0-127<br>X                               |                                                              |
| **アフタータッチ**<br>キー別<br>チャネル別                   | <br>X<br>X                                   | <br>X<br>X                                   |                                                              |
| **ピッチ・ベンダー**                                         | X                                            | O                                            | 256段階                                                      |
| **コントロールチェンジ**<br>1<br/>6/38<br/>7<br/>11<br/>64<br/>100<br/>101 | <br><br/>X<br>X<br>X<br>X<br>X<br>X<br>X<br> | <br/><br>O<br>O<br>O<br>O<br>O<br>O<br>O<br> | <br>モジュレーション<br/>データエントリー<br/>メインボリューム<br/>エクスプレッション<br/>ホールド１<br/>RPN LSB<br/>RPN MSB |
| **プログラムチェンジ**<br/><br/>設定可能範囲                 | <br/>X<br/>X                                 | <br/>0-127<br/>0-127                         |                                                              |
| **エクスクルーシブ**                                         | X                                            | O                                            |                                                              |
| **コモン**<br/>ソング・ポジション<br/>ソング・セレクト<br/>チューン | <br/>X<br/>X<br/>X                           | <br/>X<br/>X<br/>X                           |                                                              |
| **リアルタイム**<br/>クロック<br/>コマンド                   | <br/>X<br/>X                                 | <br/>X<br/>X                                 |                                                              |
| **その他**<br/>ローカル ON/OFF<br/>オールノートオフ<br/>アクティブセンシング<br/>リセット | <br/>X<br/>X<br/>X<br/>X                     | <br/>X<br/>O<br/>O<br/>X                     |                                                              |

> 対応SysEx
>
> - GMシステム・オン
> - XGシステム・オン
> - GSリセット



> 対応RPN
>
> - ピッチベンドセンシティビティ(0-24)

## 備考

質問・バグなどは[Twitter](twitter.com/status/LunaTsukinashi)もしくはこの[RepoのIssue](https://github.com/Xi80/fmFire/issues)で知らせていただけると幸いです。

コードについては徐々にリファクタリングを進めていきます。

## 参考

- https://qiita.com/Shigosen/items/a087546ba2322b82288f
- https://github.com/yamaha-webmusic/ymf825board
- http://madscient.hatenablog.jp/entry/2017/08/06/173827
- http://madscient.hatenablog.jp/entry/2017/08/06/183052
- http://madscient.hatenablog.jp/entry/2017/08/13/013913

## 改変履歴

> [2020-08-04] - 初版(Ver1.7c)

> [2020-08-22] - 第2版(Ver2.2d)


