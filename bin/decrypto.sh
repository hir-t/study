#!/bin/bash

echo "*** Decryption Shell @ Yoshimura Lab ***"

if [ $# -ne 5 ]; then
    echo "指定された引数は$#個です．" 1>&2
    echo "実行するには5個の引数が必要です．" 1>&2
    echo "decrypto.sh crypto.bench eval.bench normalInput.info keyInput.info compareInput.info" >1&2
    exit 1
fi

cnt=1

echo ""
echo "*** Generate f$cnf.cnf ***"
echo ""

echo "obfuscation $1 c1" 1>&2
../bin/topgun $1 c1

echo "End c1.cnf.end $(cat c1.cnf.end)"

echo "copyCNF c1 c1.cnf.end c2.cnf" 1>&2
../bin/copyCNF c1.cnf c1.cnf.end c2.cnf

echo "End c2.cnf.end $(cat c2.cnf.end)"

echo "geneBranch $3 c1.cnf.start c2.cnf.start c2.cnf.end br.cnf" 1>&2
../bin/geneBranch $3 c1.cnf.start c2.cnf.start c2.cnf.end br.cnf

echo "End br.cnf.end $(cat br.cnf.end)"

echo "geneXORT c1.poCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf" 1>&2
../bin/geneXORT c1.PoCnfInfo c1.cnf.start c2.cnf.start br.cnf.end xort.cnf

echo "End xort.cnf.end $(cat xort.cnf.end)"

echo "geneXORT $4 c1.cnf.start c2.cnf.start xort.cnf.end key.cnf" 1>&2
../bin/geneXORT $4 c1.cnf.start c2.cnf.start xort.cnf.end key.cnf

echo "End key.cnf.end $(cat key.cnf.end)"

echo "geneFIX1 $5 c1.cnf.start c2.cnf.start fix.cnf" 1>&2
../bin/geneFIX1 $5 c1.cnf.start c2.cnf.start fix.cnf

echo "cat c1.cnf c2.cnf br.cnf xort.cnf fix.cnf key.cnf > f1.cnf" 1>&2
cat c1.cnf c2.cnf br.cnf xort.cnf fix.cnf key.cnf > f1.cnf

echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

minisat f1.cnf f1.cnf.out

../bin/sat2dat f1.cnf.out > f1.cnf.out.dat

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
../bin/topgun $2 eval

echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf" 1>&2
../bin/geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf

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
../bin/sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat

head -150 e$cnt.cnf.out.dat > e$cnt.key

echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
../bin/geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

echo "convertID c1.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf" 1>&2
../bin/convertID c1.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf

echo "cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf



echo ""
echo "*** Generate f$cnt.cnf ***"
echo ""

echo "copyCnf XY$cnt.cnf key.cnf.end c1$cnt.cnf" 1>&2
../bin/copyCnf XY$cnt.cnf key.cnf.end c1$cnt.cnf

echo "End c1$cnt.cnf.end $(cat c1$cnt.cnf.end)"

echo "copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf" 1>&2
../bin/copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf

echo "End c2$cnt.cnf.end $(cat c2$cnt.cnf.end)"

echo "geneBranch $4 c1.cnf.start c1$cnt.cnf.start c2$cnt.cnf.end brC1C1$cnt.cnf" 1>&2
../bin/geneBranch $4 c1.cnf.start c1$cnt.cnf.start c2$cnt.cnf.end brC1C1$cnt.cnf

echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

echo "geneBranch $4 c2.cnf.start c2$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
../bin/geneBranch $4 c2.cnf.start c2$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

echo "geneFIX1 $5 c1$cnt.cnf.start c2$cnt.cnf.start fix$cnt.cnf" 1>&2
../bin/geneFIX1 $5 c1$cnt.cnf.start c2$cnt.cnf.start fix$cnt.cnf

# cntを1加算する
pre=`expr $cnt`
cnt=`expr $cnt + 1`

echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf>f$cnt.cnf" 1>&2
cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf fix$pre.cnf > f$cnt.cnf


echo ""
echo "*** minisat f$cnt.cnf ***"
echo ""

minisat f$cnt.cnf f$cnt.cnf.out

../bin/sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

# head -150 f$cnt.cnf.out.dat > f$cnt.key

result=$(cat f$cnt.cnf.out.result)

while test $result = "SAT" 
do
    echo "f$cnt.cnf is SAT"

    echo ""
    echo "*** Generate e$cnt.cnf ***"
    echo ""


    echo "geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf" 1>&2
    ../bin/geneInputFix eval.piCnfInfo f$cnt.cnf.out.dat eval.$cnt.pifix.cnf

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
    ../bin/sat2dat e$cnt.cnf.out > e$cnt.cnf.out.dat

    head -150 e$cnt.cnf.out.dat > e$cnt.key

    echo "geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp" 1>&2
    ../bin/geneInputFix eval.poCnfInfo e$cnt.cnf.out.dat eval.$cnt.pofix.cnf.tmp

    echo "convertID c1.PoCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf" 1>&2
    ../bin/convertID c1.PoCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf

    echo "cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf" 1>&2
    cat c1.cnf eval.$cnt.pifix.cnf eval.$cnt.pofix.cnf > XY$cnt.cnf


    echo ""
    echo "*** Generate f$cnt.cnf ***"
    echo ""

    echo "copyCnf XY$cnt.cnf brC2C2$pre.cnf.end c1$cnt.cnf" 1>&2
    ../bin/copyCnf XY$cnt.cnf brC2C2$pre.cnf.end c1$cnt.cnf

    echo "End c1$cnt.cnf.end $(cat c1$cnt.cnf.end)"

    #exit 1

    echo "copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf" 1>&2
    ../bin/copyCnf XY$cnt.cnf c1$cnt.cnf.end c2$cnt.cnf

    echo "End c2$cnt.cnf.end $(cat c2$cnt.cnf.end)"

    echo "geneBranch $4 c1.cnf.start c1$cnt.cnf.start c2$cnt.cnf.end brC1C1$cnt.cnf" 1>&2
    ../bin/geneBranch $4 c1.cnf.start c1$cnt.cnf.start c2$cnt.cnf.end brC1C1$cnt.cnf

    echo "End brC1C1$cnt.cnf.end $(cat brC1C1$cnt.cnf.end)"

    echo "geneBranch $4 c2.cnf.start c2$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf" 1>&2
    ../bin/geneBranch $4 c2.cnf.start c2$cnt.cnf.start brC1C1$cnt.cnf.end brC2C2$cnt.cnf

    echo "End brC2C2$cnt.cnf.end $(cat brC2C2$cnt.cnf.end)"

    echo "geneFIX1 $5 c1$cnt.cnf.start c2$cnt.cnf.start fix$cnt.cnf" 1>&2
    ../bin/geneFIX1 $5 c1$cnt.cnf.start c2$cnt.cnf.start fix$cnt.cnf

    # cntを1加算する
    pre=`expr $cnt`
    cnt=`expr $cnt + 1`

    echo "cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf > f$cnt.cnf" 1>&2
    cat f$pre.cnf c1$pre.cnf c2$pre.cnf brC1C1$pre.cnf brC2C2$pre.cnf fix$pre.cnf > f$cnt.cnf

    echo ""
    echo "*** minisat f$cnt.cnf ***"
    echo ""

    minisat f$cnt.cnf f$cnt.cnf.out

    ../bin/sat2dat f$cnt.cnf.out > f$cnt.cnf.out.dat

    head -150 f$cnt.cnf.out.dat > f$cnt.key


    result=$(cat f$cnt.cnf.out.result)


    if test $result = "SAT" ; then
    echo "f$cnt.cnf is SAT"
    else
    echo "f$cnt.cnf is UNSAT"
    fi
done

# 鍵を特定する

echo "cat c1.cnf fix.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf" 1>&2
cat c1.cnf fix.cnf c1*.cnf brC1C1*.cnf > decrypto.cnf

echo "f$cnt.cnf is UNSAT"

