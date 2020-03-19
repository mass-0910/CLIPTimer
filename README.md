# CLIP Timer

CLIP STUDIO PAINTでお絵かきのタイムを計測するためのソフトウェアです.  

## # 使い方

1. releaseからCLIPTimer.zipをダウンロードして解凍  
2. setting.txtの1行目にCLIPStudioPaint.exeへのフルパスを記述  
3. setting.txtの2行目に.clipファイルを格納しているフォルダへのフルパスを記述  
4. CLIPTimer.exeを実行すると,タイマーウインドウとCLIP STUDIO PAINTが開きます  

## # 使用コンパイラとライブラリ

本プログラムはコンパイラはMinGW gcc 8.2.0, UIなどにはDXライブラリを使用しています.

## # コンパイルコマンド

コンパイルには、MinGW内のincludeにDxLib/DxLib.hが格納されている必要があります。
```
> windres -o src/resource.o src/resource.rc
> g++ -mwindows -static-libstdc++ -static-libgcc -o release/CLIPTimer.exe src/*.cpp src/resource.o --exec-charset=cp932 --input-charset=utf-8 "-LC:\MinGW\lib\DxLib" -DDX_GCC_COMPILE -DDX_NON_INLINE_ASM -lDxLib -lDxUseCLib -lDxDrawFunc -ljpeg -lpng -lzlib -ltiff -ltheora_static -lvorbis_static -lvorbisfile_static -logg_static -lbulletdynamics -lbulletcollision -lbulletmath -lopusfile -lopus -lsilk_common -lcelt
```
cpl.ps1に上記のコマンドを書きました。powershellにてプロジェクト直下で以下のコマンドを打つことでコンパイル出来ます。
```
> ./cpl.ps1
```