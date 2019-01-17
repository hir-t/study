#!/bin/bash

echo "*** Decryption Shell @ Yoshimura Lab ***"

if [ $# -ne 5 ]; then
    echo "指定された引数は$#個です．" 1>&2
    echo "実行するには5個の引数が必要です．" 1>&2
    echo "decrypto.sh crypto.bench eval.bench normalInput.info keyInput.info compareInput.info" >1&2
    exit 1
fi
    # 1:crypto.bench      -> 難読化するベンチマーク
    # 2:eval.bench        -> 元のベンチマーク
    # 3:nomalInput.info   -> 外部入力をまとめたファイル
    # 4:keyInput.info     -> キー入力
    # 5:compareInput.info -> 外部出力をまとめたファイル

cnt=1
echo ""
echo "*** Generate f$cnf.cnf ***"
echo ""

#topgunに難読化するベンチマークを与える
# シェル実行前に先にやってc432_obf.cnfをc1_obf.cnfに変更するべきかも

echo "obfuscation $1 c1" 1>&2
../exp/topgun $1 c1

echo "End c1_obf.cnf.end $(cat c1_obf.cnf.end)"

#cnfをコピーする
# c1_obf.cnf      -> コピーするファイル
# c1_obf.cnf.end  -> 一番大きい信号線ID(最後の信号線)を記録。これとc1_obf.cnf内の数値を使ってできる最大値に内容を更新する
# c2_obf.cnf      -> コピーされるファイル
echo "copyCNF c1 c1.cnf.end c2.cnf" 1>&2
../exp/copyCNF c1_obf.cnf c1_obf.cnf.end c2_obf.cnf

# c2_obf.cnf.end  -> 上のcopyCNFでできた最大値をこの名前で出力されている。
echo "End c2_obf.cnf.end $(cat c2_obf.cnf.end)"

# $3                ->  外部入力をまとめたファイル
# c1_obf.cnf.start  ->  1つ目の難読化回路の最小信号線ID
# c2_obf.cnf.start  ->  内容は"c1_obf.cnf.end+1"。35行目のcopyCNFで作成
# c2_obf.cnf.end    ->  +1してブランチの別れる前のIDにするために使用
# br.cnf            ->  出力用ファイル
echo "geneBranch $3 c1_obf.cnf.start c2_obf.cnf.start c2_obf.cnf.end br.cnf" 1>&2
../exp/geneBranch $3 c1_obf.cnf.start c2_obf.cnf.start c2_obf.cnf.end br.cnf

echo "End br.cnf.end $(cat br.cnf.end)"

# c1.poCnfInfo      ->  c1の外部入力リスト
# c1_obf.cnf.start  ->  c1の最小ID
# c2_obf.cnf.start  ->  c2の最小ID
# br.cnf.end        ->  ブランチの最大ID(ここまで作ったIDの最大値)
# xort.cnf          ->  出力用
echo "geneXORT c1.poCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf" 1>&2
../exp/geneXORT c1_obf.poCnfInfo c1_obf.cnf.start c2_obf.cnf.start br.cnf.end xort.cnf

echo "End xort.cnf.end $(cat xort.cnf.end)"

# $4                ->  keyInput.info
# c1_obf.cnf.start  ->  c1の最小ID
# c2_obf.cnf.start  ->  c2の最小ID
# xort.cnf.end      ->  ここまで作ったIDの最大値
# key.cnf           ->  出力用
echo "geneXORT $4 c1_obf.cnf.start c2_obf.cnf.start xort.cnf.end key.cnf" 1>&2
../exp/geneXORT $4 c1_obf.cnf.start c2_obf.cnf.start xort.cnf.end key.cnf

#echo "End key.cnf.end $(cat key.cnf.end)"

# $5                ->  compareInput.info
# c1_obf.cnf.start  ->  c1_obfの最小ID
# c2_obf.cnf.start  ->  c2_obfの最小ID
# fix.cnf           ->  出力用
echo "geneFIX1 $5 c1_obf.cnf.start c2_obf.cnf.start fix.cnf" 1>&2
../exp/geneFIX1 $5 c1_obf.cnf.start c2_obf.cnf.start fix.cnf

# f1.cnfにc1_obf.cnf c2_obf.cnf br.cnf xort.cnf fix.cnf key.cnfを書き込む
echo "cat c1_obf.cnf c2_obf.cnf br.cnf xort.cnf fix.cnf key.cnf > f1.cnf" 1>&2
#cat c1_obf.cnf c2_obf.cnf br.cnf xort.cnf fix.cnf key.cnf > f1.cnf
cat c1_obf.cnf c2_obf.cnf br.cnf xort.cnf > f1.cnf

echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

# f1.cnfを入力としてminisatを実行し、f1.cnf.outで結果を出力
minisat f1.cnf f1.cnf.out

../exp/sat2dat f1.cnf.out > f1.cnf.out.dat

head -150 f1.cnf.out.dat > f1.key


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


echo "obfuscation $2 eval" 1>&2
../exp/topgun $2 eval

echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf" 1>&2
../exp/geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf

echo "cat eval.cnf eval.$cnt.pifix.cnf > e$cnt.cnf"
cat eval.cnf eval.$cnt.pifix.cnf > e$cnt.cnf

echo ""
echo "*** minisat e$cnt.cnf ***"
echo ""

minisat e$cnt.cnf e$cnt.cnf.out


echo ""
echo "*** Generate XY$cnt.cnf ***"
echo ""

echo "sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat" 1>&2
../exp/sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat

head -150 e$cnt.cnf.out.dat > e$cnt.key

echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
../exp/geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

echo "convertID c1_obf.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf" 1>&2
../exp/convertID c1_obf.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf

echo "cat c1_obf.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
cat c1_obf.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf



echo ""
echo "*** Generate f$cnt.cnf ***"
echo ""

echo "copyCnf XY$cnt.cnf key.cnf.end c1$cnt.cnf" 1>&2
../exp/copyCnf XY$cnt.cnf key.cnf.end c1_obf$cnt.cnf

echo "End c1_obf$cnt.cnf.end $(cat c1_obf$cnt.cnf.end)"

echo "copyCnf XY$cnt.cnf c1_obf$cnt.cnf.end c2_obf$cnt.cnf" 1>&2
../exp/copyCnf XY$cnt.cnf c1_obf$cnt.cnf.end c2_obf$cnt.cnf

echo "End c2$cnt.cnf.end $(cat c2$cnt.cnf.end)"

echo "geneBranch $4 c1_obf.cnf.start c1_obf$cnt.cnf.start c2_obf$cnt.cnf.end brC1C1$cnt.cnf" 1>&2
../exp/geneBranch $4 c1_obf.cnf.start c1_obf$cnt.cnf.start c2_obf$cnt.cnf.end brC1C1$cnt.cnf

echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

echo "geneBranch $4 c2_obf.cnf.start c2_obf$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
../exp/geneBranch $4 c2_obf.cnf.start c2_obf$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

echo "geneFIX1 $5 c1_obf$cnt.cnf.start c2_obf$cnt.cnf.start fix$cnt.cnf" 1>&2
../exp/geneFIX1 $5 c1_obf$cnt.cnf.start c2_obf$cnt.cnf.start fix$cnt.cnf

# cntを1加算する
pre=`expr $cnt`
cnt=`expr $cnt + 1`

echo "cat f$pre.cnf c1_obf$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf>f$cnt.cnf" 1>&2
cat f$pre.cnf c1_obf$pre.cnf c2_obf$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf fix$pre.cnf > f$cnt.cnf


echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

minisat f$cnt.cnf f$cnt.cnf.out

../exp/sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

# head -150 f$cnt.cnf.out.dat > f$cnt.key

result=$(cat f$cnt.cnf.out.result)

while test $result = "SAT" 
do
    echo "f$cnt.cnf is SAT"

    echo ""
    echo "*** Generate e$cnt.cnf ***"
    echo ""


    echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf" 1>&2
    ../exp/geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf

    echo "cat eval.cnf eval.$cnt.pifix.cnf > e$cnt.cnf"
    cat eval.cnf eval.$cnt.pifix.cnf > e$cnt.cnf

    echo ""
    echo "*** minisat e$cnt.cnf ***"
    echo ""

    minisat e$cnt.cnf e$cnt.cnf.out



    echo ""
    echo "*** Generate XY$cnt.cnf ***"
    echo ""

    echo "sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat" 1>&2
    ../exp/sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat

    head -150 e$cnt.cnf.out.dat > e$cnt.key

    echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
    ../exp/geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

    echo "convertID c1_obf.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf" 1>&2
    ../exp/convertID c1_obf.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf

    echo "cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
    cat c1_obf.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf


    echo ""
    echo "*** Generate f$cnt.cnf ***"
    echo ""

    echo "copyCnf XY$cnt.cnf brC2C2$pre.cnf.end c1$cnt.cnf" 1>&2
    ../exp/copyCnf XY$cnt.cnf brC2C2$pre.cnf.end c1_obf$cnt.cnf

    echo "End c1_obf$cnt.cnf.end $(cat c1_obf$cnt.cnf.end)"

    #exit 1

    echo "copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf" 1>&2
    ../exp/copyCnf XY$cnt.cnf c1_obf$cnt.cnf.end c2_obf$cnt.cnf

    echo "End c2_obf$cnt.cnf.end $(cat c2_obf$cnt.cnf.end)"

    echo "geneBranch $4 c1_obf.cnf.start c1_obf$cnt.cnf.start c2_obf$cnt.cnf.end brC1C1$cnt.cnf" 1>&2
    ../exp/geneBranch $4 c1_obf.cnf.start c1_obf$cnt.cnf.start c2_obf$cnt.cnf.end brC1C1$cnt.cnf

    echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

    echo "geneBranch $4 c2_obf.cnf.start c2_obf$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
    ../exp/geneBranch $4 c2_obf.cnf.start c2_obf$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

    echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

    echo "geneFIX1 $5 c1_obf$cnt.cnf.start c2_obf$cnt.cnf.start fix$cnt.cnf" 1>&2
    ../exp/geneFIX1 $5 c1_obf$cnt.cnf.start c2_obf$cnt.cnf.start fix$cnt.cnf

    # cntを1加算する
    pre=`expr $cnt`
    cnt=`expr $cnt + 1`

    echo "cat f$pre.cnf c1_obf$pre.cnf c2_obf$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf > f$cnt.cnf" 1>&2
    cat f$pre.cnf c1_obf$pre.cnf c2_obf$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf fix$pre.cnf > f$cnt.cnf

    echo ""
    echo "*** minisat f$cnt.cnf ***"
    echo ""

    minisat f$cnt.cnf f$cnt.cnf.out

    ../exp/sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

    head -150 f$cnt.cnf.out.dat > f$cnt.key


    result=$(cat f$cnt.cnf.out.result)


    if test $result = "SAT" ; then
    echo "f$cnt.cnf is SAT"
    else
    echo "f$cnt.cnf is UNSAT"
    fi
done

# 鍵を特定する

echo "cat c1_obf.cnf fix.cnf c1_obf*.cnf brC1C1*.cnf > decrypto.cnf" 1>&2
cat c1_obf.cnf fix.cnf c1_obf*.cnf brC1C1*.cnf > decrypto.cnf

echo "f$cnt.cnf is UNSAT"

minisat decrypto.cnf decrypto.cnf.out


