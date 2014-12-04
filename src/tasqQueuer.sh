#!/bin/bash
DES_EXEC_STD=./des_c
DES_EXEC_CUDA=./des_cu
KEY_FILE=key
DATA_FOLDER=data
OUTPUT_FOLDER=output
TASQ_OUTPUT_FOLDER=tasq_output
RUNS=1
MAX_FILE_SIZE=$((10**3))

threads_per_block=-1
cuda_only=0


# -c cuda only, -t number or threads
while getopts ":t:c" opt; do
  case $opt in
    a)
      echo "-t was triggered, Parameter: $OPTARG" >&2
      threads_per_block=$OPTARG 
      ;;
    c)
      echo "Will only execute CUDA part." >&2
      cuda_only=1
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

if [ $threads_per_block -eq -1 ]; then
  echo "Must supply threds per block -t" >&2
  exit 1
fi

#exit 1
if [ $cuda_only -eq 0 ] 
then
  echo "No arguments supplied -> Running all"
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
make des_c_cuda

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
		
		echo -e "\t" `tasq enq $STD_TASQ_OUTPUT_FILENAME \
			$DES_EXEC_STD encrypt \
			-i $DATAFILE \
			-o $STD_OUTPUT_FILENAME \
			-k $KEY_FILE`

    sleep 2 
    
    if [ $# -eq 0 ] 
    then
  		echo -e "\t" `tasq enq $CUDA_TASQ_OUTPUT_FILENAME \
	  		$DES_EXEC_CUDA encrypt \
		  	-i $DATAFILE \
			  -o $CUDA_OUTPUT_FILENAME \
	  		-k $KEY_FILE \
		  	-t $threads_per_block`

      sleep 2
    fi
	done
done
