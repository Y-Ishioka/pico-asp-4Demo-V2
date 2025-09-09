◆概要

ラズパイPicoとTOPPERS/ASPを使ったマトリクスLEDへの表示プログラムです。

TOPPERS/ASPは CQ出版の Interface誌 2021年8月号に掲載されたTOPPERS/ASPを用いています。

- https://interface.cqpub.co.jp/magazine/202108/

- https://interface.cqpub.co.jp/wp-content/uploads/if2108_109.pdf

マトリクスLEDへの表示プログラムは以下の４種類が入っています。

- 二字熟語穴埋問題

- 四字熟語連鎖

- 四字駅名連鎖

- 迷路自動探索

表示プログラムの切り換えはロータリーDIPスイッチで選択できます。

![schematic](/pico-asp-4Demo-V2.jpg)


◆ビルド手順 メモ

tar xzf pico-asp-4Demo-V2-asp.tar.gz

git clone -b 1.4.0 https://github.com/raspberrypi/pico-sdk.git

cd elf2uf2

export PICO_SDK_PATH=../pico-sdk

cmake ../pico-sdk/tools/elf2uf2

make

cd ../build

make



◆TOPPERSライセンスについて

当プログラムは一部，TOPPERSライセンス下のソースを流用・改造して作成されています．

機器に組み込んで利用する場合にはTOPPERSライセンスを確認してください．

https://www.toppers.jp/license.html


◆免責

プログラムを用いることで生じるいかなる損害に関して，当プログラムの著作者，

流用元ソースの著作権者・管理者は一切の責任を負わないものとします．

