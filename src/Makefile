CC     = gcc
NVCC   = nvcc
C_SRC  = des.c
C_OBJ  = des_c
CU_SRC = des.cu
CU_OBJ = des_cu

main: des_c

des_c:
	$(CC) -o $(C_OBJ) $(C_SRC)

des_c_cuda:
	$(NVCC) -o $(CU_OBJ) $(CU_SRC)

test:
	$(CC) -DTEST -o $(C_OBJ) $(C_SRC)
	./$(C_OBJ) test

clean:
	rm -f $(C_OBJ) $(CU_OBJ)

eval: main
	@echo "Creating random file of 1 MB..."
	@dd if=/dev/urandom of=rand.txt bs=1024 count=1000

	@echo "Encrypting random file..."
	./$(C_OBJ) encrypt -i rand.txt -o out.txt -k rand.txt

	@echo "Decrypting previously encrypted file..."
	./$(C_OBJ) decrypt -i out.txt -o rand_decrypted.txt -k rand.txt

	@echo "calculating checksums..."
	@md5sum rand.txt
	@md5sum rand_decrypted.txt

	@echo "Cleaning up..."
	@rm rand.txt out.txt rand_decrypted.txt

eval_cuda: des_c_cuda
	#to be done
