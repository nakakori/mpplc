# Pritty Printer

## pritty_printer
main()関数あり．プログラム実行時にファイル名を与える．LL構文解析器の初期化を行い，LL構文解析を開始する．構文解析を終了すると，LL構文解析器の終了処理をする．解析結果をプリティプリンタして，標準出力する．

## ll_parse
LL構文解析を行う．構文毎に関数を用意する．LL構文解析器の初期化関数，終了関数を用意する．解析結果は解析木に登録していく．解析木に登録するのはトークンの番号と，名前の場合はIDのポインタ，文字列の場合は文字列へのポインタ，符号なし整数の場合は数字へのポインタを登録する．

## parse_tree
解析木のデータを以下の形式にする．ツリーではなく，リスト構造にしても良いかもしれない．ツリーとリストをかけ合わせたデータ構造を使う．
> parse tree
>> Token Data
>>> Token number
>>> Data pointer

>> tree pointer
>> list pointer

データの中を以下の形式にする．
> Data
>> ID pointer
>> STRING pointer
>> NUMBER pointer

ツリー初期化関数で解析木全体の初期化を行う．リスト初期化関数でツリーの1ノード(リスト構造)の生成をおこなう．