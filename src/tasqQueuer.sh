#!/bin/bash
DES_EXEC_STD=./des_c
DES_EXEC_CUDA=./des_cu
KEY_FILE=key
DATA_FILES=../../experiments/data/*
OUTPUT_FOLDER=../../experiments/output
TASQ_OUTPUT_FOLDER=../../experiments/tasq_output
RUNS=1
THREADS_PER_BLOCK=500
MAX_FILE_SIZE=$((10**7))

for DATAFILE in $DATA_FILES; do
	FILESIZE=$(stat -c%s "$DATAFILE")
	if [[ "$FILESIZE" -gt "$MAX_FILE_SIZE" ]]; then
		echo Ignoring $DATAFILE with $FILESIZE bytes
		continue	
	fi
	NUM_BLOCKS=$[$FILESIZE/8]
	THREAD_BLOCKS=$[1 + $NUM_BLOCKS/$THREADS_PER_BLOCK]
	echo Processing $DATAFILE with $FILESIZE bytes = $NUM_BLOCKS blocks...
	for (( i = 0; i < $RUNS; i++ )); do
		STD_OUTPUT_FILENAME=$OUTPUT_FOLDER/std/$(basename $DATAFILE)_RUN_$i
		CUDA_OUTPUT_FILENAME=$OUTPUT_FOLDER/cuda/$(basename $DATAFILE)_RUN_$i
		STD_TASQ_OUTPUT_FILENAME=$TASQ_OUTPUT_FOLDER/std/$(basename $DATAFILE)_RUN_$i
		CUDA_TASQ_OUTPUT_FILENAME=$TASQ_OUTPUT_FOLDER/cuda/$(basename $DATAFILE)_RUN_$i
		
		echo -e "\t" `tasq enq $STD_TASQ_OUTPUT_FILENAME \
			$DES_EXEC_STD encrypt \
			-i $DATAFILE \
			-o $STD_OUTPUT_FILENAME \
			-k $KEY_FILE`

		echo -e "\t" `tasq enq $CUDA_TASQ_OUTPUT_FILENAME \
			$DES_EXEC_CUDA encrypt \
			-i $DATAFILE \
			-o $CUDA_OUTPUT_FILENAME \
			-k $KEY_FILE \
			-b $THREAD_BLOCKS \
			-t $THREADS_PER_BLOCK`
	done
done
