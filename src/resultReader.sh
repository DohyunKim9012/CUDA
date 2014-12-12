DATA_FOLDER=data
TASQ_OUTPUT_FOLDER=tasq_output
OUTPUT_FOLDER=output


echo "-----------------------"
echo "CUDA Results"
echo "-----------------------"

echo -e "file\texec_time\tcuda_threads\tcuda_blocks\thint"
for DATAFILE in $TASQ_OUTPUT_FOLDER/cuda/*; do
  
  echo -e $(basename $DATAFILE) "\t" \
        `cat $DATAFILE | grep Execution | cut -d' ' -f 3` \
        `cat $DATAFILE | grep Threads | cut -d' ' -f 6` "\t" \
        `cat $DATAFILE | tail -n 2 | grep Blocks | cut -d' ' -f 2` "\t" \
        `cat $DATAFILE | grep devNumBlocks` "\t" 
done

echo "-----------------------"
echo "CPU Results"
echo "-----------------------"
for DATAFILE in $TASQ_OUTPUT_FOLDER/std/*; do
    echo -e $(basename $DATAFILE) "\t" `tail --lines=1 $DATAFILE | cut -d' ' -f 3`
done

echo "-----------------------"
echo "CUDA checksums"
echo "-----------------------"
for DATAFILE in $OUTPUT_FOLDER/cuda/*; do
    echo `md5sum $DATAFILE`
done

echo "-----------------------"
echo "CPU checksums"
echo "-----------------------"
for DATAFILE in $OUTPUT_FOLDER/std/*; do
    echo `md5sum $DATAFILE`
done

