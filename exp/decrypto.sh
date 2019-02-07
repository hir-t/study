#!/bin/bash

echo "*** Decryption Shell @ Yoshimura Lab ***"

if [ $# -ne 3 ]; then
    echo "指定された引数は$#個です．" 1>&2
    echo "実行するには3個の引数が必要です．" 1>&2
    echo "decrypto.sh circuit.bench normalInput.info keyInput.info" >1&2
    exit 1
fi

cnt=1

echo ""
echo "*** Generate f$cnf.cnf ***"
echo ""

#topgunに難読化するベンチマークを与える

echo "topgun $1 c1 eval" 1>&2
./topgun $1 c1 eval

echo "End c1.cnf.end $(cat c1.cnf.end)"

#cnfをコピーする
# c1.cnf      -> コピーするファイル
# c1.cnf.end  -> 一番大きい信号線ID(最後の信号線)を記録。これとc1.cnf内の数値を使ってできる最大値に内容を更新する
# c2.cnf      -> コピーされるファイル
echo "copyCNF c1.cnf c1.cnf.end c2.cnf" 1>&2
./copyCNF c1.cnf c1.cnf.end c2.cnf
./copyInfo c1.keyInfo c1.cnf.end c2.keyInfo
./copyInfo c1.piCnfInfo c1.cnf.end c2.piCnfInfo
./copyInfo c1.poCnfInfo c1.cnf.end c2.poCnfInfo

cat c1.keyInfo c2.keyInfo > key.keyInfo

### c2.cnf.end  -> 上のcopyCNFでできた最大値をこの名前で出力されている。
echo "End c2.cnf.end $(cat c2.cnf.end)"

# $2                ->  外部入力をまとめたファイル
# c1.cnf.start  ->  1つ目の難読化回路の最小信号線ID
# c2.cnf.start  ->  内容は"c1.cnf.end+1"。35行目のcopyCNFで作成
# c2.cnf.end    ->  +1してブランチの別れる前のIDにするために使用
# br.cnf            ->  出力用ファイル
echo "geneBranch $2 c1.cnf.start c2.cnf.start c2.cnf.end br.cnf" 1>&2
./geneBranch $2 c1.cnf.start c2.cnf.start c2.cnf.end br.cnf

echo "End br.cnf.end $(cat br.cnf.end)"

# c1.poCnfInfo      ->  c1の外部入力リスト
# c1.cnf.start  ->  c1の最小ID
# c2.cnf.start  ->  c2の最小ID
# br.cnf.end        ->  ブランチの最大ID(ここまで作ったIDの最大値)
# xort.cnf          ->  出力用
echo "geneXORT c1.poCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf" 1>&2
./geneXORT c1.poCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf

echo "End xort.cnf.end $(cat xort.cnf.end)"

# f1.cnfにc1.cnf c2.cnf br.cnf xort.cnf fix.cnf key.cnfを書き込む
#echo "cat c1.cnf c2.cnf br.cnf xort.cnf fix.cnf key.cnf > f1.cnf" 1>&2
#cat c1.cnf c2.cnf br.cnf xort.cnf fix.cnf key.cnf > f1.cnf
echo "cat c1.cnf c2.cnf br.cnf xort.cnf > f1.cnf" 1>&2
cat c1.cnf c2.cnf br.cnf xort.cnf > f1.cnf

echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

# f1.cnfを入力としてminisatを実行し、f1.cnf.outで結果を出力
minisat f1.cnf f1.cnf.out

./sat2dat f1.cnf.out > f1.cnf.out.dat

##先頭の150行をキーとして出力
#head -150 f1.cnf.out.dat > f1.key

result=$(cat f$cnt.cnf.out.result)

if test $result = "SAT" ; then
    echo "f1.cnf is SAT"
else
    echo "f1.cnf is UNSAT"
    exit 1;
fi

echo ""
echo "*** Generate e1.cnf ***"
echo ""


#echo "obfuscation $2 eval" 1>&2
#../exp/topgun $2 eval

#eval.$cntへの入力値ビット列をまとめるP = {(IN1,OUT1),(IN2,OUT2)...}の　INn
echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf.tmp" 1>&2
#../exp/geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf
./geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf.tmp

./geneInputFix key.keyInfo f$cnt.cnf.out.dat k$cnt.keyfix.cnf
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

echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
./geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

#Pへ入出力値ペアを追加する
# echo "cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
# cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf
echo "cat eval.$cnt.pifix.cnf..tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp" 1>&2
cat eval.$cnt.pifix.cnf.tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp


echo ""
echo "*** discriminate correct key ***"
echo ""
#ここで正しいキーと誤ったキーの判別をする。k1の場合。
echo "copyCNF c1.cnf xort.cnf.end test1$cnt.cnf" 1>&2
./copyCnf c1.cnf xort.cnf.end test1$cnt.cnf
./copyInfo c1.piCnfInfo xort.cnf.end test1$cnt.piCnfInfo
./copyInfo c1.poCnfInfo xort.cnf.end test1$cnt.poCnfInfo
./copyInfo c1.keyInfo xort.cnf.end test1$cnt.keyInfo

echo "convertID test1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test1$cnt.pifix.cnf" 1>&2
./convertID test1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp test1$cnt.pifix.cnf
echo "convertID test1$cnt.keyInfo c1.key test1$cnt.keyfix.cnf" 1>&2
./convertID test1$cnt.keyInfo c1.key test1$cnt.keyfix.cnf

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

    echo "excludeKey c1.keyInfo c1.key badKey1$cnt.cnf" 1>&2
    ./excludeKey c1.keyInfo c1.key badKey1$cnt.cnf

    echo "copyExclude c2.keyInfo c1.key badKey2$cnt.cnf" 1>&2
    ./copyExclude c2.keyInfo c1.key badKey2$cnt.cnf
    #./excludeKey c2.keyInfo c1.key badKey2$cnt.cnf
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
echo "convertID test2$cnt.keyInfo c2.key test2$cnt.keyfix.cnf" 1>&2
./convertID test2$cnt.keyInfo c2.key test2$cnt.keyfix.cnf

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
    #./excludeKey c1.keyInfo c2.key badKey3$cnt.cnf
    echo "excludeKey c2.keyInfo c2.key badKey4$cnt.cnf" 1>&2
    ./excludeKey c2.keyInfo c2.key badKey4$cnt.cnf

    echo "copyExclude c1.keyInfo c2.key badKey3$cnt.cnf" 1>&2
    ./copyExclude c1.keyInfo c2.key badKey3$cnt.cnf
else
    echo "k2 is correct key"
    #exit 1;
fi

echo ""
echo "*** Generate f2.cnf ***"
echo ""

#f$cntの複製->次の異なる出力出し、複数のキーをもつ入力を探す。
#echo "copyCnf XY$cnt.cnf xort.cnf.end c1$cnt.cnf" 1>&2
echo "copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp" 1>&2
./copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp
#../exp/copyCnf XY$cnt.cnf xort.cnf.end c1$cnt.cnf
echo "End c1$cnt.cnf.tmp.end $(cat c1$cnt.cnf.tmp.end)"

echo "copyInfo c1.piCnfInfo tstxort2$cnt.cnf.end c1$cnt.piCnfInfo" 1>&2
./copyInfo c1.piCnfInfo tstxort2$cnt.cnf.end c1$cnt.piCnfInfo

echo "copyInfo c1.poCnfInfo tstxort2$cnt.cnf.end c1$cnt.poCnfInfo" 1>&2
./copyInfo c1.poCnfInfo tstxort2$cnt.cnf.end c1$cnt.poCnfInfo

./copyInfo c1.keyInfo tstxort2$cnt.cnf.end c1$cnt.keyInfo

echo "cat c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo" 1>&2
cat c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo

echo "convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf" 1>&2
./convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf

# echo "convertID c11.piCnfInfo eval.$cnt.pifix.cnf.tmp eval.1$cnt.pifix.cnf" 1>&2
# ../exp/convertID c11.piCnfInfo eval.$cnt.pifix.cnf.tmp eval.1$cnt.pifix.cnf

# echo "convertID c11.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.1$cnt.pofix.cnf" 1>&2
# ../exp/convertID c11.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.1$cnt.pofix.cnf

#echo "cat eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
#cat eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf

echo "cat c1$cnt.cnf.tmp XY1$cnt.cnf> c1$cnt.cnf" 1>&2
cat c1$cnt.cnf.tmp XY1$cnt.cnf > c1$cnt.cnf

# echo "copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf" 1>&2
# ../exp/copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf
echo "copyCnf c1.cnf c1$cnt.tmp.cnf.end c2$cnt.cnf" 1>&2
./copyCnf c1.cnf c1$cnt.cnf.tmp.end c2$cnt.cnf.tmp

#MAX IDの確認
echo "End c2$cnt.cnf.tmp.end $(cat c2$cnt.cnf.tmp.end)"

# echo "cat c1$cnt.b.cnf XY$cnt.cnf > c1$cnt.cnf" 1>&2
# cat c2$cnt.b.cnf XY$cnt.cnf > c2$cnt.cnf

echo "copyInfo c1.piCnfInfo c1$cnt.cnf.tmp.end c2$cnt.piCnfInfo" 1>&2
./copyInfo c1.piCnfInfo c1$cnt.cnf.tmp.end c2$cnt.piCnfInfo

echo "copyInfo c1.poCnfInfo c1$cnt.cnf.tmp.end c2$cnt.poCnfInfo" 1>&2
./copyInfo c1.poCnfInfo c1$cnt.cnf.tmp.end c2$cnt.poCnfInfo

./copyInfo c1.keyInfo c1$cnt.cnf.tmp.end c2$cnt.keyInfo

# echo "convertID c2$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp eval.2$cnt.pifix.cnf" 1>&2
# ../exp/convertID c2$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp eval.2$cnt.pifix.cnf

# echo "convertID c2$cnt.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.2$cnt.pofix.cnf" 1>&2
# ../exp/convertID c2$cnt.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.2$cnt.pofix.cnf

# echo "cat eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
# cat eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf

echo "cat c2$cnt.piCnfInfo c2$cnt.poCnfInfo > c2$cnt.pipoCnfInfo" 1>&2
cat c2$cnt.piCnfInfo c2$cnt.poCnfInfo > c2$cnt.pipoCnfInfo

echo "convertID c2$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY2$cnt.cnf" 1>&2
./convertID c2$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY2$cnt.cnf

#../exp/convertID key.keyInfo XY$cnt.cnf.tmp key$cnt.keyInfo

echo "cat c2$cnt.cnf.tmp XY2$cnt.cnf> c2$cnt.cnf" 1>&2
cat c2$cnt.cnf.tmp XY2$cnt.cnf > c2$cnt.cnf

#c1のキー入力のブランチを生成
echo "geneBranch $3 c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf" 1>&2
./geneBranch $3 c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf

echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

#c2のキー入力のブランチを生成
echo "geneBranch $3 c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
./geneBranch $3 c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

#MAX ID の確認
echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

echo "geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf" 1>&2
./geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf
echo "End xxort1$cnt.cnf.end $(cat xxort1$cnt.cnf.end)"

echo "geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf" 1>&2
./geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf
echo "End xxort2$cnt.cnf.end $(cat xxort2$cnt.cnf.end)"

# cntを1加算する
# ここでcnt=2,pre=1になる
pre=`expr $cnt`
cnt=`expr $cnt + 1`

# echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf>f$cnt.cnf" 1>&2
# cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf > f$cnt.cnf
echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf" 1>&2
cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf

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

    #eval.$cntへの入力値をまとめるP = {(IN1,OUT1),(IN2,OUT2)...}の　OUT
    #echo "convertID c1.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf" 1>&2
    #../exp/convertID c1.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf

    #Pに入出力のペアを追加(IN, OUT) = (eval.$cnt.pifix.cnf, eval.$cnt.pofix.cnf)
    #echo "cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
    #cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf
    echo "cat eval.$cnt.pifix.cnf.tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp" 1>&2
    cat eval.$cnt.pifix.cnf.tmp eval.$cnt.pofix.cnf.tmp > XY$cnt.cnf.tmp



    echo ""
    echo "*** discriminate correct key ***"
    echo ""
    #ここで正しいキーと誤ったキーの判別をする。k1の場合。
    echo "copyCNF c1.cnf xxort2$pre.cnf.end test1$cnt.cnf" 1>&2
    ./copyCnf c1.cnf xxort2$pre.cnf.end test1$cnt.cnf
    ./copyInfo c1.piCnfInfo xxort2$pre.cnf.end test1$cnt.piCnfInfo
    ./copyInfo c1.poCnfInfo xxort2$pre.cnf.end test1$cnt.poCnfInfo
    ./copyInfo c1.keyInfo xxort2$pre.cnf.end test1$cnt.keyInfo

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

        echo "excludeKey c1.keyInfo c1$cnt.key badKey1$cnt.cnf" 1>&2
        ./excludeKey c1.keyInfo c1$cnt.key badKey1$cnt.cnf

        echo "copyExclude c2.keyInfo c1$cnt.key badKey2$cnt.cnf" 1>&2
        ./copyExclude c2.keyInfo c1$cnt.key badKey2$cnt.cnf
        #./excludeKey c2.keyInfo c1.key badKey2$cnt.cnf
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
        #./excludeKey c1.keyInfo c2.key badKey3$cnt.cnf
        echo "excludeKey c2.keyInfo c2$cnt.key badKey4$cnt.cnf" 1>&2
        ./excludeKey c2.keyInfo c2$cnt.key badKey4$cnt.cnf

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

    #echo "copyCnf XY$cnt.cnf brC2C2$pre.cnf.end c1$cnt.cnf" 1>&2
    #../exp/copyCnf XY$cnt.cnf brC2C2$pre.cnf.end c1$cnt.cnf

    #echo "copyCnf c1.cnf brC2C2$pre.cnf.end c1$cnt.b.cnf" 1>&2
    #../exp/copyCnf c1.cnf brC2C2$pre.cnf.end c1$cnt.b.cnf
    echo "copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp" 1>&2
    ../exp/copyCnf c1.cnf tstxort2$cnt.cnf.end c1$cnt.cnf.tmp

     echo "End c1$cnt.cnf.end $(cat c1$cnt.cnf.tmp.end)"

    # ./copyInfo c1.piCnfInfo brC2C2$pre.cnf.end c1$cnt.piCnfInfo
    # ./copyInfo c1.poCnfInfo brC2C2$pre.cnf.end c1$cnt.poCnfInfo
    ./copyInfo c1.piCnfInfo tstxort2$cnt.cnf.end c1$cnt.piCnfInfo
    ./copyInfo c1.poCnfInfo tstxort2$cnt.cnf.end c1$cnt.poCnfInfo
    ./copyInfo c1.keyInfo tstxort2$cnt.cnf.end c1$cnt.keyInfo

    echo "c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo" 1>&2
    cat c1$cnt.piCnfInfo c1$cnt.poCnfInfo > c1$cnt.pipoCnfInfo

    echo "convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf" 1>&2
    ./convertID c1$cnt.pipoCnfInfo XY$cnt.cnf.tmp XY1$cnt.cnf


    # echo "convertID c1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp eval.$cnt.pifix.cnf" 1>&2
    # ../exp/convertID c1$cnt.piCnfInfo eval.$cnt.pifix.cnf.tmp eval.$cnt.pifix.cnf

    # echo "convertID c1$cnt.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf" 1>&2
    # ../exp/convertID c1$cnt.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf

    # echo "cat eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
    # cat eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf

    echo "cat c1$cnt.cnf.tmp XY1$cnt.cnf > c1$cnt.cnf" 1>&2
    cat c1$cnt.cnf.tmp XY1$cnt.cnf > c1$cnt.cnf


    #exit 1

    #echo "copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf" 1>&2
    #../exp/copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf

    echo "copyCnf c1.cnf c1$cnt.cnf.tmp.end c2$cnt.cnf.tmp" 1>&2
    ./copyCnf c1.cnf c1$cnt.cnf.tmp.end c2$cnt.cnf.tmp

    #echo "cat c2$cnt.b.cnf XY$cnt.cnf > c2$cnt.cnf" 1>&2
    #cat c2$cnt.b.cnf XY$cnt.cnf > c2$cnt.cnf

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

    echo "geneBranch $3 c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf" 1>&2
    ./geneBranch $3 c1.cnf.start c1$cnt.cnf.tmp.start c2$cnt.cnf.tmp.end brC1C1$cnt.cnf

    echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

    echo "geneBranch $3 c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
    ./geneBranch $3 c2.cnf.start c2$cnt.cnf.tmp.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

    echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

    echo "geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf" 1>&2
    ./geneXORT c1.piCnfInfo c1.cnf.start c1$cnt.cnf.tmp.start brC2C2$cnt.cnf.end xxort1$cnt.cnf
    echo "End xxort1$cnt.cnf.end $(cat xxort1$cnt.cnf.end)"

    echo "geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf" 1>&2
    ./geneXORT c1.piCnfInfo c2.cnf.start c2$cnt.cnf.tmp.start xxort1$cnt.cnf.end xxort2$cnt.cnf
    echo "End xxort2$cnt.cnf.end $(cat xxort2$cnt.cnf.end)"

    # cntを1加算する
    # 1回目のここで cnt=3,pre=2になる
    pre=`expr $cnt`
    cnt=`expr $cnt + 1`

    #echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf > f$cnt.cnf" 1>&2
    #cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf > f$cnt.cnf

    echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$cnt.cnf.end badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf" 1>&2
    cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf xxort1$pre.cnf xxort2$pre.cnf badKey1$pre.cnf badKey2$pre.cnf badKey3$pre.cnf badKey4$pre.cnf > f$cnt.cnf

    echo ""
    echo "*** minisat f$cnt.cnf ***"
    echo ""

    minisat f$cnt.cnf f$cnt.cnf.out

    ./sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

    #head -150 f$cnt.cnf.out.dat > f$cnt.key


    result=$(cat f$cnt.cnf.out.result)


    if test $result = "SAT" ; then
    echo "f$cnt.cnf is SAT"
    else
    echo "f$cnt.cnf is UNSAT"
    fi
done
./problem c1$pre.cnf problem.cnf #とりあえずつけるだけで書き込む数字はなんでもいい
# 鍵を特定する

# echo "cat c1.cnf fix.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf" 1>&2
# cat c1.cnf fix.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf

#echo "cat c1.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf" 1>&2
#cat c1.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf
mv c1.cnf original.cnf
echo "cat c1*.cnf brC1C1*.cnf > decrypto.cnf" 1>&2
cat problem.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf
mv original.cnf c1.cnf
echo "f$cnt.cnf is UNSAT"

minisat decrypto.cnf decrypto.cnf.out


