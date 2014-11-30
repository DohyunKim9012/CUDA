#!/bin/bash
DES_EXEC_STD=./des_c
DES_EXEC_CUDA=./des_cu
KEY_FILE=key
DATA_FOLDER=data
OUTPUT_FOLDER=output
TASQ_OUTPUT_FOLDER=tasq_output
RUNS=1
THREADS_PER_BLOCK=500
MAX_FILE_SIZE=$((10**4))

# Create required folder structures
rm -r ${OUTPUT_FOLDER}
rm -r ${TASQ_OUTPUT_FOLDER}
rm -r ${DATA_FOLDER}
mkdir -p ${OUTPUT_FOLDER}/{std,cuda}
mkdir -p ${TASQ_OUTPUT_FOLDER}/{std,cuda}
mkdir ${DATA_FOLDER}

# Create keyfile
dd if=/dev/urandom of=key bs=8 count=1

# Create data
file_size=$((2**10))
while [ $file_size -lt $MAX_FILE_SIZE ]; do
  echo $file_size
  if [ $file_size -lt $((2**20)) ]; then
    file_name="$(($file_size/1024))K"
  elif [ $file_size -lt $((2**30)) ]; then
    file_name="$(($file_size/2**20))M"
  else
    file_name="$(($file_size/2**30))G"
  fi
  dd if=/dev/urandom of=$DATA_FOLDER/$file_name bs=$file_size count=1
  file_size=$(($file_size*2))
done


for DATAFILE in ${DATA_FOLDER}/*; do
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
