CC     = gcc
NVCC   = nvcc
C_SRC  = des.c
C_OBJ  = des_c
CU_SRC = des.cu
CU_OBJ = des_cu

main: des_c

des_c:
	$(CC) -o $(C_OBJ) $(C_SRC)

test:
	$(CC) -DTEST -o $(C_OBJ) $(C_SRC)
	./$(C_OBJ) test

clean:
	rm -f $(C_OBJ) $(CU_OBJ)
