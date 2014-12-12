#!/bin/bash
DES_EXEC_STD=./des_c
DES_EXEC_CUDA=./des_cu
KEY_FILE=key
DATA_FOLDER=data
OUTPUT_FOLDER=output
TASQ_OUTPUT_FOLDER=tasq_output
RUNS=1
MAX_FILE_SIZE=$((2*10**10))

threads_per_block=$2
cuda_only=$1


echo "$threads_per_block"

if [ $cuda_only -eq 0 ] 
then
  echo "running all"
  rm -rf ${OUTPUT_FOLDER}/{std,cuda}
  rm -rf ${TASQ_OUTPUT_FOLDER}/{std,cuda}
  rm -rf ${DATA_FOLDER}
  mkdir -p ${OUTPUT_FOLDER}/{std,cuda}
  mkdir -p ${TASQ_OUTPUT_FOLDER}/{std,cuda}
  mkdir ${DATA_FOLDER}

  # Create keyfile
  dd if=/dev/urandom of=key bs=8 count=1
  echo "Generated new key file"
else
  echo "Ignoring CPU, not creating data new. Using old key file"
  rm -rf ${OUTPUT_FOLDER}/cuda
  rm -rf ${TASQ_OUTPUT_FOLDER}/cuda
  mkdir -p ${OUTPUT_FOLDER}/cuda
  mkdir -p ${TASQ_OUTPUT_FOLDER}/cuda
fi
# make binaries
make des_c
make cuda

if [ $cuda_only -eq 0 ] 
then
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
  echo Data files created
  sleep 1
fi

for DATAFILE in ${DATA_FOLDER}/*; do
	FILESIZE=$(stat -c%s "$DATAFILE")
	if [[ "$FILESIZE" -gt "$MAX_FILE_SIZE" ]]; then
		echo Ignoring $DATAFILE with $FILESIZE bytes
		continue	
	fi
	NUM_BLOCKS=$[$FILESIZE/8]
	THREAD_BLOCKS=$[1 + $NUM_BLOCKS/$threads_per_block]
	echo Processing $DATAFILE with $FILESIZE bytes = $NUM_BLOCKS blocks...
	for (( i = 0; i < $RUNS; i++ )); do
		STD_OUTPUT_FILENAME=$OUTPUT_FOLDER/std/$(basename $DATAFILE)_RUN_$i
		CUDA_OUTPUT_FILENAME=$OUTPUT_FOLDER/cuda/$(basename $DATAFILE)_RUN_$i
		STD_TASQ_OUTPUT_FILENAME=$TASQ_OUTPUT_FOLDER/std/$(basename $DATAFILE)_RUN_$i
		CUDA_TASQ_OUTPUT_FILENAME=$TASQ_OUTPUT_FOLDER/cuda/$(basename $DATAFILE)_RUN_$i
		
    if [ $cuda_only -eq 0 ]; then
      $DES_EXEC_STD encrypt -i $DATAFILE -o $STD_OUTPUT_FILENAME \
        -k $KEY_FILE > $STD_TASQ_OUTPUT_FILENAME
      sleep 2  
    fi

    $DES_EXEC_CUDA encrypt -i $DATAFILE -o $CUDA_OUTPUT_FILENAME \
      -k $KEY_FILE -t $threads_per_block > $CUDA_TASQ_OUTPUT_FILENAME

    sleep 2
	done
done
