◆TOPPERSライセンスについて
当プログラムは一部，TOPPERSライセンス下のソースを流用・改造して作成されています．
機器に組み込んで利用する場合にはTOPPERSライセンスを確認してください．
https://www.toppers.jp/license.html

◆免責
プログラムを用いることで生じるいかなる損害に関して，当プログラムの著作者，
流用元ソースの著作権者・管理者は一切の責任を負わないものとします．


◆ビルド環境構築メモ

ビルド環境の構築に関しては、以下のQiitaのページを参照のこと。

・ラズパイを使ったTOPPERS用Cortex-M開発環境の構築
  https://qiita.com/Yukiya_Ishioka/items/e68a6aabf72f0721948d

・ラズパイPico2用TOPPERS/ASPのビルド手順
  https://qiita.com/Yukiya_Ishioka/items/652f295cd43b159857d8


◆ビルド手順 メモ

tar xzf pico-asp-4Demo-V2-asp.tar.gz

----------------------------------------
-rw-rw-r--  1 ubuntu ubuntu 1690 Oct 11  2022 TOPPERS_License.txt
drwxr-xr-x 15 ubuntu ubuntu 4096 May 10  2021 asp/
drwxrwxr-x  2 ubuntu ubuntu 4096 Sep  9 00:25 build/
drwxrwxr-x  2 ubuntu ubuntu 4096 Sep  9 00:29 elf2uf2/
drwxrwxr-x 11 ubuntu ubuntu 4096 Sep  6  2022 pico-sdk/
-rw-rw-r--  1 ubuntu ubuntu  367 May 16  2021 readme.txt
drwxrwxr-x  4 ubuntu ubuntu 4096 Sep  8 20:16 src/
----------------------------------------

git clone -b 1.4.0 https://github.com/raspberrypi/pico-sdk.git

cd elf2uf2
export PICO_SDK_PATH=../pico-sdk
cmake ../pico-sdk/tools/elf2uf2

----------------------------------------
-rw-rw-r-- 1 ubuntu ubuntu 14195 Sep  9 00:31 CMakeCache.txt
drwxrwxr-x 6 ubuntu ubuntu  4096 Sep  9 00:31 CMakeFiles/
-rw-rw-r-- 1 ubuntu ubuntu  5183 Sep  9 00:31 Makefile
drwxrwxr-x 3 ubuntu ubuntu  4096 Sep  9 00:31 boot_uf2_headers/
-rw-rw-r-- 1 ubuntu ubuntu  1847 Sep  9 00:31 cmake_install.cmake
----------------------------------------

make

----------------------------------------
-rw-rw-r-- 1 ubuntu ubuntu  14195 Sep  9 00:31 CMakeCache.txt
drwxrwxr-x 6 ubuntu ubuntu   4096 Sep  9 00:32 CMakeFiles/
-rw-rw-r-- 1 ubuntu ubuntu   5183 Sep  9 00:31 Makefile
drwxrwxr-x 3 ubuntu ubuntu   4096 Sep  9 00:31 boot_uf2_headers/
-rw-rw-r-- 1 ubuntu ubuntu   1847 Sep  9 00:31 cmake_install.cmake
-rwxrwxr-x 1 ubuntu ubuntu 126376 Sep  9 00:32 elf2uf2*
----------------------------------------

cd ../build
make

