rm -rf results
mkdir results
./noTasqQueuer.sh 0 512
./resultReader.sh > results/GO_512_threads
cp -R tasq_output/cuda results/GO_512_tasq_output
./noTasqQueuer.sh 1 256
./resultReader.sh > results/GO_256_threads
cp -R tasq_output/cuda results/GO_256_tasq_output
./noTasqQueuer.sh 1 128
./resultReader.sh > results/GO_128_threads
cp -R tasq_output/cuda results/GO_128_tasq_output
./noTasqQueuer.sh 1 64
./resultReader.sh > results/GO_64_threads
cp -R tasq_output/cuda results/GO_64_tasq_output
./noTasqQueuer.sh 1 32
./resultReader.sh > results/GO_32_threads
cp -R tasq_output/cuda results/GO_32_tasq_output
./noTasqQueuer.sh 1 16
./resultReader.sh > results/GO_16_threads
cp -R tasq_output/cuda results/GO_16_tasq_output

