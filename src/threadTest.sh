DES_EXEC_CUDA=./des_cu
KEY_FILE=key
DATAFILE=data_fiep
OUTPUT=out
RUNS=1
THREADS_PER_BLOCK_MAX=640
THREADS_PER_BLOCK_MIN=20
THREADS_PER_BLOCK_STEP=20


FILESIZE=$(stat -c%s "$DATAFILE")
NUM_BLOCKS=$[$FILESIZE/8]
for (( THREADS = $THREADS_PER_BLOCK_MIN; THREADS < $THREADS_PER_BLOCK_MAX; THREADS+=$THREADS_PER_BLOCK_STEP )); do

    THREAD_BLOCKS=$[1 + $NUM_BLOCKS/$THREADS]

      for (( i = 0; i < $RUNS; i++ )); do

      echo -e "\t" `$DES_EXEC_CUDA encrypt \
              -i $DATAFILE \
                    -o $OUTPUT \
                          -k $KEY_FILE \
                                -b $THREAD_BLOCKS \
                                      -t $THREADS`
        done
      done



