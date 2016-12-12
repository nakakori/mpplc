# Cross Reference
## モジュール構成
### cross referenceモジュール
メインモジュール．symbol tableモジュールで作成した記号表からクロスリファレンスを標準出力に出力する．クロスリファレンスは変数名もしくは副プログラム名を辞書順でソートして出力する．表示形式はテキスト参照．記号表からソートするのは面倒なので，記号表から一度名前をすべて取り出し，名前一覧を作成する．この名前一覧をソートした形で作成し，名前一覧を順に取り出しキーとして記号表を参照する．必要そうな関数は，メイン関数，クロスリファレンス表示関数，ソート済名前一覧作成関数．名前一覧の構造は以下の通りにする．
> name list
>> name (char *)

>> procedure name (char *)

>> next pointer (struct *)

### symbol tableモジュール
記号表を作成するモジュール．記号表の構造は以下の通りにする．また，記号表はハッシュ法を用いる．

> symbol table
>> name (char *)

>> procedure name (char *)

>> type pointer (struct TYPE)

>> parameter flag (int, 0:variable, 1:parameter)

>> define line num (int)

>> reference pointer (struct LINE)

>> chain next (struct *, hash chain list)

また，型を示す構造は以下の通りにする．

> TYPE
>> type (int, TPINT, TPCHAR, TPBOOL, TPARRAY, TPPROC)

>> array size (int)

>> element type pointer (struct *, array element type)

>> parameter pointer (struct *, parameter's type list)

また，リファレンスを示す構造を以下の通りとする．
> LINE
>> reference line num (int)

>> next(struct *)

次に，記号表の作り方を示す．まず，本モジュールが初期化された時点で，グローバルな記号表を作成する．副プログラムが宣言されたら，その時点で新しい記号表を作成する．記号表は連結リストでスタックする．次に，変数の定義は，"変数宣言部"もしくは"仮引数部"で行われる．どちらにせよ，"変数名の並び"を通るので，"変数名の並び"の関数内で変数の登録をすれば良い．登録時に，変数が定義されていないかどうかをチェックする(同一のスコープ内だけでチェックすれば良い)．"変数名の並び"で作成されたノードの型は未登録である．したがって，未登録の変数に直後に書かれた型を登録する．未登録の変数が格納されている番号を保持しておいて，その要素にあるノードの型に登録する．チェインリンクで作成するので，一応型が未登録がどうかをチェック！型を登録するのは"型"の関数内で行う(標準型が他でも参照されているため)．副プログラムが宣言された時点で，副プログラム名を保持して，スコープが副プログラム内であるなら，procedure nameを保持した副プログラム名に設定する．
