# 引数はベンチマーク回路のみ
# topugunでEvalとCのCNFを引数で与えた回路から生成する。
# 先行研究Cyclic obfuscation の方
echo "*** Decryption Shell @ Yoshimura Lab ***"

if [ $# -ne 3 ]; then
    echo "指定された引数は$#個です．" 1>&2
    echo "実行するには3個の引数が必要です．" 1>&2
    echo "decrypto.sh circuit.bench Number_of_Loops Number_of_Length" >1&2
    exit 1
fi

cnt=1

echo ""
echo "*** Generate f$cnf.cnf ***"
echo ""

#topgunに難読化するベンチマークを与える

echo "obfuscation $1 c1 eval $2 $3" 1>&2
./obfuscation $1 c1 eval $2 $3

echo "End c1.cnf.end $(cat c1.cnf.end)"

# c1を　コピーしてc2を生成
# c1とc2の出力を比較する
echo "copyCNF c1.cnf c1.cnf.end c2.cnf" 1>&2
./copyCNF c1.cnf c1.cnf.end c2.cnf

# Infoもコピーして信号線の確認やfixで使用
./copyInfo c1.keyInfo c1.cnf.end c2.keyInfo
./copyInfo c1.piCnfInfo c1.cnf.end c2.piCnfInfo
./copyInfo c1.poCnfInfo c1.cnf.end c2.poCnfInfo

# c1とc2のキー入力を1つにまとめる
# キーの除外時に使用
cat c1.keyInfo c2.keyInfo > key.keyInfo

echo "End c2.cnf.end $(cat c2.cnf.end)"

# c1とc2の入力.piCnfInfoをブランチで繋いで同じ値が入るようにする
echo "geneBranch c1.piCnfInfo c1.cnf.start c2.cnf.start c2.cnf.end br.cnf" 1>&2
./geneBranch c1.piCnfInfo c1.cnf.start c2.cnf.start c2.cnf.end br.cnf

echo "End br.cnf.end $(cat br.cnf.end)"

# c1とc2の出力をXORで比較して出力が1(c1の出力!=c2の出力)となるようにする
echo "geneXORT c1.poCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf" 1>&2
./geneXORT c1.poCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf

echo "End xort.cnf.end $(cat xort.cnf.end)"

# f1.cnfにc1.cnf c2.cnf br.cnf xort.cnf を書き込む
# これが異なる出力を持つ入力Xを探す回路になる
echo "cat c1.cnf c2.cnf br.cnf xort.cnf > f1.cnf" 1>&2
cat c1.cnf c2.cnf br.cnf xort.cnf > f1.cnf

echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

# f1.cnfを入力としてminisatを実行し、f1.cnf.outで結果を出力
minisat f1.cnf f1.cnf.out

./sat2dat f1.cnf.out > f1.cnf.out.dat

result=$(cat f$cnt.cnf.out.result)

if test $result = "SAT" ; then
    echo "f1.cnf is SAT"
else
    echo "f1.cnf is UNSAT"
    exit 1;
fi

# Xに対する正しい出力を取得する
echo ""
echo "*** Generate e1.cnf ***"
echo ""

#eval.$cntへの入力値ビット列をまとめるP = {(IN1,OUT1),(IN2,OUT2)...}の　INn
echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf.tmp" 1>&2
./geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf.tmp

# c1とc2のキー入力部分の値をまとめたファイル(計算に直接は使用しない。キーの値の確認用)
# c1とc2のキー入力部分の値をまとめたファイル(計算に直接は使用しない。キーの値の確認用)
./geneInputFix key.keyInfo f$cnt.cnf.out.dat k$cnt.keyfix.cnf

# c1とc2のキー入力部分を別々に出力したファイル
./geneInputFix c1.keyInfo f$cnt.cnf.out.dat c1.key
./geneInputFix c2.keyInfo f$cnt.cnf.out.dat c2.key

#evalへ入力を与えたcnfファイルを作る
echo "cat eval.cnf eval.$cnt.pifix.cnf > e$cnt.cnf"
cat eval.cnf eval.$cnt.pifix.cnf.tmp > e$cnt.cnf

echo ""
echo "*** minisat e$cnt.cnf ***"
echo ""

minisat e$cnt.cnf e$cnt.cnf.out

#Pに追加
echo ""
echo "*** Generate XY$cnt.cnf.tmp ***"
echo ""

echo "sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat" 1>&2
./sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat

#正しい出力を得る
echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
./geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

#Pへ入出力値ペアを追加する.IDはc1c2のまま
echo "cat eval.$cnt.pifix.cnf..tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp" 1>&2
cat eval.$cnt.pifix.cnf.tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp


#ここで正しいキーと誤ったキーの判別をする。
echo ""
echo "*** discriminate correct key ***"
echo ""
# x,k1を与えたCの出力とxを与えたEvalの出力を比較
echo "copyCNF c1.cnf xort.cnf.end test1$cnt.cnf" 1>&2
./copyCnf c1.cnf xort.cnf.end test1$cnt.cnf
./copyInfo c1.piCnfInfo xort.cnf.end test1$cnt.piCnfInfo
./copyInfo c1.poCnfInfo xort.cnf.end test1$cnt.poCnfInfo
./copyInfo c1.keyInfo xort.cnf.end test1$cnt.keyInfo

# eval.$cnt.pifix.cnf.tmpの値をtest1$cnt.piCnfInfoの信号線に変換
echo "convertID test1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test1$cnt.pifix.cnf" 1>&2
./convertID test1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test1$cnt.pifix.cnf
# キー入力も同様に
echo "convertID test1$cnt.keyInfo c1.key test1$cnt.keyfix.cnf" 1>&2
./convertID test1$cnt.keyInfo c1.key test1$cnt.keyfix.cnf

# EvalとCの出力比較用のXOR  Evalの出力　!= C となるようにする。これが SATとなれば誤ったキーと判定
echo "geneXORT eval.poCnfInfo eval.cnf.start test1$cnt.cnf.start test1$cnt.cnf.end tstxort1$cnt.cnf" 1>&2
./geneXORT eval.poCnfInfo eval.cnf.start test1$cnt.cnf.start test1$cnt.cnf.end tstxort1$cnt.cnf

# EvalとCの出力を比較する回路の生成(c1とc2の出力比較と同じ構成)
echo "e$cnt.cnf test1$cnt.cnf test1$cnt.pifix.cnf test1$cnt.keyfix.cnf  tstxort1$cnt.cnf > testF1.cnf" 1>&2
cat e$cnt.cnf test1$cnt.cnf test1$cnt.pifix.cnf test1$cnt.keyfix.cnf  tstxort1$cnt.cnf > testF1.cnf

echo "testF1.cnf testF1.cnf.out" 1>&2
minisat testF1.cnf testF1.cnf.out

./sat2dat testF1.cnf.out > testF1.cnf.out.dat

result=$(cat testF1.cnf.out.result)

if test $result = "SAT" ; then
    echo "k1 is incorrect key"

    echo "exclude c1.keyInfo c1.key badKey1$cnt.cnf" 1>&2
    ./exclude c1.keyInfo c1.key badKey1$cnt.cnf

    echo "copyExclude c2.keyInfo c1.key badKey2$cnt.cnf" 1>&2
    ./copyExclude c2.keyInfo c1.key badKey2$cnt.cnf
else
    echo "k1 is correct key"
    #exit 1;
fi

# x,k2を与えた出力とxを与えたEvalの出力を比較
echo "copyCNF c1.cnf tstxort1$cnt.cnf.end test2$cnt.cnf" 1>&2
./copyCnf c1.cnf tstxort1$cnt.cnf.end test2$cnt.cnf
./copyInfo c1.piCnfInfo tstxort1$cnt.cnf.end test2$cnt.piCnfInfo
./copyInfo c1.poCnfInfo tstxort1$cnt.cnf.end test2$cnt.poCnfInfo
./copyInfo c1.keyInfo tstxort1$cnt.cnf.end test2$cnt.keyInfo

echo "convertID test2$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test2$cnt.pifix.cnf" 1>&2
./convertID test2$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test2$cnt.pifix.cnf
echo "convertID test2$cnt.keyInfo c2.key test2$cnt.keyfix.cnf" 1>&2
./convertID test2$cnt.keyInfo c2.key test2$cnt.keyfix.cnf

#Evalと比較
echo "geneXORT eval.poCnfInfo eval.cnf.start test2$cnt.cnf.start test2$cnt.cnf.end tstxort2$cnt.cnf" 1>&2
./geneXORT eval.poCnfInfo eval.cnf.start test2$cnt.cnf.start test2$cnt.cnf.end tstxort2$cnt.cnf

echo "e$cnt.cnf test1$cnt.cnf test2$cnt.pifix.cnf test2$cnt.keyfix.cnf  tstxort2$cnt.cnf > testF2.cnf" 1>&2
cat e$cnt.cnf test2$cnt.cnf test2$cnt.pifix.cnf test2$cnt.keyfix.cnf  tstxort2$cnt.cnf > testF2.cnf

echo "testF2.cnf testF2.cnf.out" 1>&2
minisat testF2.cnf testF2.cnf.out

./sat2dat testF2.cnf.out > testF2.cnf.out.dat

result=$(cat testF2.cnf.out.result)

if test $result = "SAT" ; then
    echo "k2 is incorrect key"
    #./exclude c1.keyInfo c2.key badKey3$cnt.cnf
    echo "exclude c2.keyInfo c2.key badKey4$cnt.cnf" 1>&2
    ./exclude c2.keyInfo c2.key badKey4$cnt.cnf

    echo "copyExclude c1.keyInfo c2.key badKey3$cnt.cnf" 1>&2
    ./copyExclude c1.keyInfo c2.key badKey3$cnt.cnf
else
    echo "k2 is correct key"
    #exit 1;
fi

echo ""
echo "*** Generate f2.cnf ***"
echo ""

#f$cntの複製->次の異なる出力をする入力を探す。
echo "copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp" 1>&2
./copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp
echo "End c1$cnt.cnf.tmp.end $(cat c1$cnt.cnf.tmp.end)"

echo "copyInfo c1.piCnfInfo tstxort2$cnt.cnf.end c1$cnt.piCnfInfo" 1>&2
./copyInfo c1.piCnfInfo tstxort2$cnt.cnf.end c1$cnt.piCnfInfo

echo "copyInfo c1.poCnfInfo tstxort2$cnt.cnf.end c1$cnt.poCnfInfo" 1>&2
./copyInfo c1.poCnfInfo tstxort2$cnt.cnf.end c1$cnt.poCnfInfo

./copyInfo c1.keyInfo tstxort2$cnt.cnf.end c1$cnt.keyInfo

#c1$cntの外部入出力IDをまとめる
echo "cat c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo" 1>&2
cat c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo

#119行目で取得した入出力ペアのIDをc1$cntの入出力IDへ変換->入出力値の固定
echo "convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf" 1>&2
./convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf

#入出力ペアを追加して次の入力を探すためのファイル生成
echo "cat c1$cnt.cnf.tmp XY1$cnt.cnf> c1$cnt.cnf" 1>&2
cat c1$cnt.cnf.tmp XY1$cnt.cnf > c1$cnt.cnf

echo "copyCnf c1.cnf c1$cnt.tmp.cnf.end c2$cnt.cnf" 1>&2
./copyCnf c1.cnf c1$cnt.cnf.tmp.end c2$cnt.cnf.tmp

#MAX IDの確認
echo "End c2$cnt.cnf.tmp.end $(cat c2$cnt.cnf.tmp.end)"

echo "copyInfo c1.piCnfInfo c1$cnt.cnf.tmp.end c2$cnt.piCnfInfo" 1>&2
./copyInfo c1.piCnfInfo c1$cnt.cnf.tmp.end c2$cnt.piCnfInfo

echo "copyInfo c1.poCnfInfo c1$cnt.cnf.tmp.end c2$cnt.poCnfInfo" 1>&2
./copyInfo c1.poCnfInfo c1$cnt.cnf.tmp.end c2$cnt.poCnfInfo

./copyInfo c1.keyInfo c1$cnt.cnf.tmp.end c2$cnt.keyInfo

echo "cat c2$cnt.piCnfInfo c2$cnt.poCnfInfo > c2$cnt.pipoCnfInfo" 1>&2
cat c2$cnt.piCnfInfo c2$cnt.poCnfInfo > c2$cnt.pipoCnfInfo

# XY1$cntと同じ
echo "convertID c2$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY2$cnt.cnf" 1>&2
./convertID c2$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY2$cnt.cnf

echo "cat c2$cnt.cnf.tmp XY2$cnt.cnf> c2$cnt.cnf" 1>&2
cat c2$cnt.cnf.tmp XY2$cnt.cnf > c2$cnt.cnf

#c1のキー入力のブランチを生成
echo "geneBranch c1.keyInfo c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf" 1>&2
./geneBranch c1.keyInfo c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf

echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

#c2のキー入力のブランチを生成
echo "geneBranch c1.keyInfo c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
./geneBranch c1.keyInfo c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

#MAX ID の確認
echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

# 求めた入力Xが今後のSATの計算で出ないようにする
echo "exclude c1.piCnfInfo eval.$cnt.pifix.cnf.tmp exIn$cnt.cnf" 1>&2
    ./exclude c1.piCnfInfo eval.$cnt.pifix.cnf.tmp exIn$cnt.cnf

# echo "geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf" 1>&2
# ./geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf
# echo "End xxort1$cnt.cnf.end $(cat xxort1$cnt.cnf.end)"

# echo "geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf" 1>&2
# ./geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf
# echo "End xxort2$cnt.cnf.end $(cat xxort2$cnt.cnf.end)"

# cntを1加算する
# ここでcnt=2,pre=1になる
pre=`expr $cnt`
cnt=`expr $cnt + 1`

# echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf" 1>&2
# cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf

echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf exIn$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf" 1>&2
cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf exIn$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf

echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

minisat f$cnt.cnf f$cnt.cnf.out

./sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

result=$(cat f$cnt.cnf.out.result)

while test $result = "SAT"
do
    echo "f$cnt.cnf is SAT"

    echo ""
    echo "*** Generate e$cnt.cnf ***"
    echo ""

    ##eval.$cntへの入力値をまとめるP = {(IN1,OUT1),(IN2,OUT2)...}の　IN
    echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf.tmp" 1>&2
    ./geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf.tmp

    #c1とc2のキー入力部分の値をまとめたファイル(計算に直接は使用しない。キーの値の確認用)
    ./geneInputFix key.keyInfo f$cnt.cnf.out.dat k$cnt.keyfix.cnf
    ./geneInputFix c1.keyInfo f$cnt.cnf.out.dat c1$cnt.key
    ./geneInputFix c2.keyInfo f$cnt.cnf.out.dat c2$cnt.key

    echo "cat eval.cnf eval.$cnt.pifix.cnf.tmp > e$cnt.cnf"
    cat eval.cnf eval.$cnt.pifix.cnf.tmp > e$cnt.cnf
    #上で得た入力に対する出力を求める
    echo ""
    echo "*** minisat e$cnt.cnf ***"
    echo ""

    minisat e$cnt.cnf e$cnt.cnf.out


    #Pに追加する
    echo ""
    echo "*** Generate XY$cnt.cnf.tmp ***"
    echo ""

    #e$cnt.cnf.out.result が生成される
    echo "sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat" 1>&2
    ./sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat

    echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
    ./geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

    #Pに入出力のペアを追加(IN, OUT) = (eval.$cnt.pifix.cnf, eval.$cnt.pofix.cnf)
    echo "cat eval.$cnt.pifix.cnf.tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp" 1>&2
    cat eval.$cnt.pifix.cnf.tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp

    echo ""
    echo "*** discriminate correct key ***"
    echo ""
    #ここで正しいキーと誤ったキーの判別をする。k1の場合。
    echo "copyCNF c1.cnf brC2C2$pre.cnf.end test1$cnt.cnf" 1>&2
    ./copyCnf c1.cnf brC2C2$pre.cnf.end test1$cnt.cnf
    ./copyInfo c1.piCnfInfo brC2C2$pre.cnf.end test1$cnt.piCnfInfo
    ./copyInfo c1.poCnfInfo brC2C2$pre.cnf.end test1$cnt.poCnfInfo
    ./copyInfo c1.keyInfo brC2C2$pre.cnf.end test1$cnt.keyInfo

    echo "convertID test1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test1$cnt.pifix.cnf" 1>&2
    ./convertID test1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test1$cnt.pifix.cnf
    echo "convertID test1$cnt.keyInfo c1$cnt.key test1$cnt.keyfix.cnf" 1>&2
    ./convertID test1$cnt.keyInfo c1$cnt.key test1$cnt.keyfix.cnf

    echo "geneXORT eval.poCnfInfo eval.cnf.start test1$cnt.cnf.start test1$cnt.cnf.end tstxort1$cnt.cnf" 1>&2
    ./geneXORT eval.poCnfInfo eval.cnf.start test1$cnt.cnf.start test1$cnt.cnf.end tstxort1$cnt.cnf

    echo "e$cnt.cnf test1$cnt.cnf test1$cnt.pifix.cnf test1$cnt.keyfix.cnf  tstxort1$cnt.cnf > testF1.cnf" 1>&2
    cat e$cnt.cnf test1$cnt.cnf test1$cnt.pifix.cnf test1$cnt.keyfix.cnf  tstxort1$cnt.cnf > testF1.cnf

    echo "testF1.cnf testF1.cnf.out" 1>&2
    minisat testF1.cnf testF1.cnf.out

    ./sat2dat testF1.cnf.out > testF1.cnf.out.dat

    result=$(cat testF1.cnf.out.result)

    if test $result = "SAT" ; then
        echo "k1 is incorrect key"

        echo "exclude c1.keyInfo c1$cnt.key badKey1$cnt.cnf" 1>&2
        ./exclude c1.keyInfo c1$cnt.key badKey1$cnt.cnf

        echo "copyExclude c2.keyInfo c1$cnt.key badKey2$cnt.cnf" 1>&2
        ./copyExclude c2.keyInfo c1$cnt.key badKey2$cnt.cnf
        #./exclude c2.keyInfo c1.key badKey2$cnt.cnf
    else
        echo "k1 is correct key"
        #exit 1;
    fi

    # k2の場合
    echo "copyCNF c1.cnf tstxort1$cnt.cnf.end test2$cnt.cnf" 1>&2
    ./copyCnf c1.cnf tstxort1$cnt.cnf.end test2$cnt.cnf
    ./copyInfo c1.piCnfInfo tstxort1$cnt.cnf.end test2$cnt.piCnfInfo
    ./copyInfo c1.poCnfInfo tstxort1$cnt.cnf.end test2$cnt.poCnfInfo
    ./copyInfo c1.keyInfo tstxort1$cnt.cnf.end test2$cnt.keyInfo

    echo "convertID test2$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test2$cnt.pifix.cnf" 1>&2
    ./convertID test2$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test2$cnt.pifix.cnf
    echo "convertID test2$cnt.keyInfo c2$cnt.key test2$cnt.keyfix.cnf" 1>&2
    ./convertID test2$cnt.keyInfo c2$cnt.key test2$cnt.keyfix.cnf

    echo "geneXORT eval.poCnfInfo eval.cnf.start test2$cnt.cnf.start test2$cnt.cnf.end tstxort2$cnt.cnf" 1>&2
    ./geneXORT eval.poCnfInfo eval.cnf.start test2$cnt.cnf.start test2$cnt.cnf.end tstxort2$cnt.cnf

    echo "e$cnt.cnf test1$cnt.cnf test2$cnt.pifix.cnf test2$cnt.keyfix.cnf  tstxort2$cnt.cnf > testF2.cnf" 1>&2
    cat e$cnt.cnf test2$cnt.cnf test2$cnt.pifix.cnf test2$cnt.keyfix.cnf  tstxort2$cnt.cnf > testF2.cnf

    echo "testF2.cnf testF2.cnf.out" 1>&2
    minisat testF2.cnf testF2.cnf.out

    ./sat2dat testF2.cnf.out > testF2.cnf.out.dat

    result=$(cat testF2.cnf.out.result)

    if test $result = "SAT" ; then
        echo "k2 is incorrect key"
        #./exclude c1.keyInfo c2.key badKey3$cnt.cnf
        echo "exclude c2.keyInfo c2$cnt.key badKey4$cnt.cnf" 1>&2
        ./exclude c2.keyInfo c2$cnt.key badKey4$cnt.cnf

        echo "copyExclude c1.keyInfo c2$cnt.key badKey3$cnt.cnf" 1>&2
        ./copyExclude c1.keyInfo c2$cnt.key badKey3$cnt.cnf
    else
        echo "k2 is correct key"
        #exit 1;
    fi


    num=`expr $cnt + 1`
    echo ""
    echo "*** Generate f$num.cnf ***"
    echo ""

    echo "copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp" 1>&2
    ./copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp

     echo "End c1$cnt.cnf.end $(cat c1$cnt.cnf.tmp.end)"

    ./copyInfo c1.piCnfInfo tstxort2$cnt.cnf.end c1$cnt.piCnfInfo
    ./copyInfo c1.poCnfInfo tstxort2$cnt.cnf.end c1$cnt.poCnfInfo
    ./copyInfo c1.keyInfo tstxort2$cnt.cnf.end c1$cnt.keyInfo

    echo "c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo" 1>&2
    cat c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo

    echo "convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf" 1>&2
    ./convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf

    echo "cat c1$cnt.cnf.tmp XY1$cnt.cnf > c1$cnt.cnf" 1>&2
    cat c1$cnt.cnf.tmp XY1$cnt.cnf > c1$cnt.cnf

    echo "copyCnf c1.cnf c1$cnt.cnf.tmp.end c2$cnt.cnf.tmp" 1>&2
    ./copyCnf c1.cnf c1$cnt.cnf.tmp.end c2$cnt.cnf.tmp

    echo "End c2$cnt.cnf.tmp.end $(cat c2$cnt.cnf.tmp.end)"

    ./copyInfo c1.piCnfInfo c1$cnt.cnf.tmp.end c2$cnt.piCnfInfo
    ./copyInfo c1.poCnfInfo c1$cnt.cnf.tmp.end c2$cnt.poCnfInfo
    ./copyInfo c1.keyInfo c1$cnt.cnf.tmp.end c2$cnt.keyInfo

    echo "cat c2$cnt.piCnfInfo c2$cnt.poCnfInfo > c2$cnt.pipoCnfInfo" 1>&2
    cat c2$cnt.piCnfInfo c2$cnt.poCnfInfo > c2$cnt.pipoCnfInfo

    echo "convertID c2$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY2$cnt.cnf" 1>&2
    ./convertID c2$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY2$cnt.cnf

    echo "cat c2$cnt.cnf.tmp XY2$cnt.cnf> c2$cnt.cnf" 1>&2
    cat c2$cnt.cnf.tmp XY2$cnt.cnf > c2$cnt.cnf

    echo "geneBranch c1.keyInfo c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf" 1>&2
    ./geneBranch c1.keyInfo c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf

    echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

    echo "geneBranch c1.keyInfo c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
    ./geneBranch c1.keyInfo c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

    echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

    echo "exclude c1.piCnfInfo eval.$cnt.pifix.cnf.tmp exIn$cnt.cnf" 1>&2
    ./exclude c1.piCnfInfo eval.$cnt.pifix.cnf.tmp exIn$cnt.cnf

    # echo "geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf" 1>&2
    # ./geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf
    # echo "End xxort1$cnt.cnf.end $(cat xxort1$cnt.cnf.end)"

    # echo "geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf" 1>&2
    # ./geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf
    # echo "End xxort2$cnt.cnf.end $(cat xxort2$cnt.cnf.end)"

    # cntを1加算する
    # 1回目のここで cnt=3,pre=2になる
    pre=`expr $cnt`
    cnt=`expr $cnt + 1`

    # echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$cnt.cnf.end badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf" 1>&2
    # cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf
    echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf exIn$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf" 1>&2
    cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf exIn$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf

    echo ""
    echo "*** minisat f$cnt.cnf ***"
    echo ""

    minisat f$cnt.cnf f$cnt.cnf.out

    ./sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

    result=$(cat f$cnt.cnf.out.result)


    if test $result = "SAT" ; then
    echo "f$cnt.cnf is SAT"
    else
    echo "f$cnt.cnf is UNSAT"
    fi
done

./problem c1$pre.cnf problem.cnf #とりあえずつけるだけで書き込む数字はなんでもいい
# 鍵を特定する
mv c1.cnf original.cnf
echo "cat c1*.cnf brC1C1*.cnf > decrypto.cnf" 1>&2
cat problem.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf
mv original.cnf c1.cnf
echo "f$cnt.cnf is UNSAT"

minisat decrypto.cnf decrypto.cnf.out
./sat2dat decrypto.cnf.out > decrypto.cnf.out.dat

result=$(cat decrypto.cnf.out.result)
 if test $result = "SAT" ; then
    echo "decrypto.cnf is SAT"
    #特定したキーを出力
    ./geneInputFix c1.keyInfo decrypto.cnf.out.dat correct.keyInfo
    else
    echo "decrypto.cnf is UNSAT"
    fi


