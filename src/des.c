#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const int IP[64] = {
  58, 50, 42, 34, 26, 18, 10,  2,
  60, 52, 44, 36, 28, 20, 12,  4,
  62, 54, 46, 38, 30, 22, 14,  6,
  64, 56, 48, 40, 32, 24, 16,  8,
  57, 49, 41, 33, 25, 17,  9,  1,
  59, 51, 43, 35, 27, 19, 11,  3,
  61, 53, 45, 37, 29, 21, 13,  5,
  63, 55, 47, 39, 31, 23, 15,  7
};

static const int IP_1[64] = {
  40,  8, 48, 16, 56, 24, 64, 32,
  39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30,
  37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28,
  35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26,
  33,  1, 41,  9, 49, 17, 57, 25
};

static const int E[48] = {
  32,  1,  2,  3,  4,  5,
   4,  5,  6,  7,  8,  9,
   8,  9, 10, 11, 12, 13,
  12, 13, 14, 15, 16, 17,
  16, 17, 18, 19, 20, 21,
  20, 21, 22, 23, 24, 25,
  24, 25, 26, 27, 28, 29,
  28, 29, 30, 31, 32,  1
};

static const int S[8][64] = {
  {
    14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7, 
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 },
  {
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 },
  {
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8, 
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 },
  {
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 },
  {
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 },
  {
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 },
  {
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 },
  {
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
};

static const int P[32] =
{
  16,  7, 20, 21,
  29, 12, 28, 17,
   1, 15, 23, 26,
   5, 18, 31, 10,
   2,  8, 24, 14,
  32, 27,  3,  9,
  19, 13, 30,  6,
  22, 11,  4, 25
}; 

static const int PC_1[56] =
{
  57, 49, 41, 33, 25, 17,  9,
   1, 58, 50, 42, 34, 26, 18,
  10,  2, 59, 51, 43, 35, 27,
  19, 11,  3, 60, 52, 44, 36,
  63, 55, 47, 39, 31, 23, 15,
   7, 62, 54, 46, 38, 30, 22,
  14,  6, 61, 53, 45, 37, 29,
  21, 13,  5, 28, 20, 12,  4
};

static int PC_2[48] =
{
  14, 17, 11, 24,  1,  5,
   3, 28, 15,  6, 21, 10,
  23, 19, 12,  4, 26,  8,
  16,  7, 27, 20, 13,  2,
  41, 52, 31, 37, 47, 55,
  30, 40, 51, 45, 33, 48,
  44, 49, 39, 56, 34, 53,
  46, 42, 50, 36, 29, 32
};

static int Shifts[16] = {
  1, 1, 2, 2, 2, 2, 2, 2,
  1, 2, 2, 2, 2, 2, 2, 1
};

/* this function conducts DES xor operation */
long long int
DES (int index, long long int *MD, long long int *keys);

/* the DES function uses this F function during encryption */
unsigned int
F (unsigned int c, long long int key);

/* compute key schedule k1 .. k16 */
void
keySchedule (long long int* keys, long long int key);

/* this encryption function has the in file as an input,
 * performs encryption with key, and writes to the out file
 * as the output. */
void
encryption (char *in, char *out, char *key);

/* this decryption function has the in file as an input,
 * performs decryption with key, and writes to the out file
 * as the output. */
void
decryption (char *in, char *out, char *key);

enum operation {
  ENCRYPT = 0,
  DECRYPT = 1
};

int
main (int argc, char *argv[])
{
  char *inFile = NULL, *outFile = NULL, *keyFile = NULL;
  char opt = -1;
  int index, mode = -1;

  while ((opt = getopt (argc, argv, "i:o:k:")) > 0)
  {
    switch (opt)
    {
      case 'i':
        inFile = optarg;
        break;
      case 'o':
        outFile = optarg;
        break;
      case 'k':
        keyFile = optarg;
        break;
      default:
        break;
    }
  }

  for (index = optind; index < argc; index++)
  {
    if (mode < 0 && strcmp (argv[index], "encrypt") == 0)
    {
      mode = ENCRYPT;
    }
    else if (mode < 0 && strcmp (argv[index], "decrypt") == 0)
    {
      mode = DECRYPT;
    }
    else if (mode >= 0)
    {
      fprintf(stderr, "des: option already specified\n");
      return -1;
    }
    else
    {
      fprintf(stderr, "des: unrecognized operation: %s\n", argv[index]);
      return -1;
    }
  }

  if (inFile == NULL || outFile == NULL)
  {
    fprintf(stderr, "des: must specify input file and output file\n");
    fprintf(stderr, "Usage: (encrypt | decrypt) -i <input_file> -o <output file> [-k <key file>]\n");
    return -1;
  }

  switch (mode)
  {
    case ENCRYPT:
      encryption (inFile, outFile, keyFile);
      break;
    case DECRYPT:
      decryption (inFile, outFile, keyFile);
      break;
    default:
      fprintf(stderr, "des: must specify operation (decrypt, encrypt)\n");
      return -1;
  }

  return 0;
}

long long int
DES (int index, long long int *MD, long long int *keys)
{
  return 0;
}

unsigned int
F (unsigned int c, long long int key)
{
  return 0;
}

void
keySchedule (long long int* keys, long long int key)
{
}

void
encryption (char *in, char *out, char *key)
{
  printf("des: encryption: in(%s) out(%s), key(%s)\n", in, out, key);
  return;
}

void
decryption (char *in, char *out, char *key)
{
  printf("des: decryption: in(%s) out(%s), key(%s)\n", in, out, key);
  return;
}
