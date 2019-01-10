#
#	%Z% %M%(%I%) %E%
#

INC	=	../include

#OPT	=	-O2 -Wall
OPT	=	-g -Wall
#DEF	=	-DFLIGHT_TEST 
DEF	=	-DTOPGUN_TEST 
#DEF	=	-DTOPGUN_TEST -D_DEBUG_INPUT_
#DEF	=	-DFLIGHT_TEST -DFLIGHT_TIME_3 -DOLD_IMP2
#DEF	=	-DFLIGHT_TEST -DFLIGHT_TIME_4 -D_DEBUG_PRT_ATPG_
#DEF	=	-DFLIGHT_MEMORY_LOG -DFLIGHT_EDIF_DEBUG -D_DEBUG_PRT_TM_SCOAP_ -D_DEBUG_PRT_LV_PI_ 
CFLAGS	=	$(OPT) -I$(INC) $(DEF)

LIB	=
TAGLIB	=

EXE	=	../bin/topgun

OBJS	=	\
		topgunMain.o \
		topgunError.o \
		topgunExtern.o \
		topgunInit.o \
		topgunLevel.o \
		topgunMemory.o \
		topgunTm.o \
		topgunAtpg.o \
		topgunCheck.o \
		topgunSelect.o \
		topgunGenerate.o \
		topgunInsert.o \
		topgunClass.o \
		topgunPropagate.o \
		topgunImplication.o \
		topgunBtree.o \
		topgunState.o \
		topgunDfrontier.o \
		topgunJustified.o \
		topgunResult.o \
		topgunReflesh.o \
		topgunOutput.o \
		topgunPrint.o \
		topgunRead.o \
		topgunBench.o \
		topgunCell.o \
		topgunFlist.o \
		topgunFsim.o \
		topgunCompact.o \
		topgunStock.o \
		topgunLearn.o \
		topgunLearnDag.o \
		topgunLearnKajihara.o \
		topgunFfrg.o \
		topgunUniq.o \
		HelloWorld.o \
		n_line_out.o \
		line_point.o \
		simulate2.o \
		TopologicalSort.o \
		eval.o \
		test.o \
		dfs2.o \
		shuffle.o \
		obfuscation.o \
		countClauses.o \
		makeBench.o \
		makeCnf.o \
		makeNext.o \
		makePrev.o \
		topgunUty.o \
		topgunTime.o \
		topgunTest.o

$(EXE): $(OBJS) $(LIB) $(INC)/*.h
	gcc $(OPT) -o $(EXE) $(OBJS) $(LIB) $(TAGLIB)
	@/bin/rm -f *.o
.c.o:
	gcc $(CFLAGS) -c $*.c -o $*.o
