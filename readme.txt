・概要
木構造を記したテキストファイル(treedata.txt)を読み込むと、木構造を表示するツール(VisTree.cpp)を作りました。
あくまでも個人でてきとーに作ったものなので、利用する場合は自己責任でお願いします。


・開発環境
Visual Studio 2015
Dxライブラリ
Windows7/10


・実行方法
VisTree.exeとtreedata.txtを同じ場所において、VisTree.exeを実行するだけ。
もしくは、VisTree.cppとtreedata.txtを同じフォルダにおいてビルドする。（Dxライブラリが使える必要があります。ある程度古いバージョンでも大丈夫です。）


・操作方法
縮小／拡大 … Z,X
平行移動   … 十字キー
終了       … ESCAPE


・toTreeについて
treedata.txtを作るためのコンソールアプリケーションです。
VisTree.cppとは別物です。
