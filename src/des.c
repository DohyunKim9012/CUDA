/*
 * Data Encryption Standard (DES) Implementation in C
 *
 * Reference: "FIPS Publication 46-3"
 *            (http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf)
 * Test case: J. Orlin Grabbe, "The DES Algorithm Illustrated"
 *            (https://www.uop.edu.jo/issa/isec/Des-Example.doc) *
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define CEIL(a, b) (((a) / (b)) + (((a) % (b)) > 0 ? 1 : 0))

#define IP_SIZE 64
static const int IP[IP_SIZE] = {
  58, 50, 42, 34, 26, 18, 10,  2,
  60, 52, 44, 36, 28, 20, 12,  4,
  62, 54, 46, 38, 30, 22, 14,  6,
  64, 56, 48, 40, 32, 24, 16,  8,
  57, 49, 41, 33, 25, 17,  9,  1,
  59, 51, 43, 35, 27, 19, 11,  3,
  61, 53, 45, 37, 29, 21, 13,  5,
  63, 55, 47, 39, 31, 23, 15,  7
};

#define IP_1_SIZE 64
static const int IP_1[IP_1_SIZE] = {
  40,  8, 48, 16, 56, 24, 64, 32,
  39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30,
  37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28,
  35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26,
  33,  1, 41,  9, 49, 17, 57, 25
};

#define E_SIZE 48
static const int E[E_SIZE] = {
  32,  1,  2,  3,  4,  5,
   4,  5,  6,  7,  8,  9,
   8,  9, 10, 11, 12, 13,
  12, 13, 14, 15, 16, 17,
  16, 17, 18, 19, 20, 21,
  20, 21, 22, 23, 24, 25,
  24, 25, 26, 27, 28, 29,
  28, 29, 30, 31, 32,  1
};

#define S_SIZE 8
static const int S[S_SIZE][4][16] = {
  {
    { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7 },
    {  0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8 },
    {  4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0 },
    { 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 }
  },
  {
    { 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10 },
    {  3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5 },
    {  0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15 },
    { 13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 }
  },
  {
    { 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8 },
    { 13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1 },
    { 13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7 },
    {  1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 }
  },
  {
    {  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15 },
    { 13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9 },
    { 10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4 },
    {  3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 }
  },
  {
    {  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9 },
    { 14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6 },
    {  4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14 },
    { 11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 }
  },
  {
    { 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11 },
    { 10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8 },
    {  9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6 },
    {  4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 }
  },
  {
    {  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1 },
    { 13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6 },
    {  1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2 },
    {  6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 }
  },
  {
    { 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7 },
    {  1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2 },
    {  7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8 },
    {  2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
  }
};

#define P_SIZE 32
static const int P[P_SIZE] =
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

#define PC_1_SIZE 56
static const int PC_1[PC_1_SIZE] =
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

#define PC_2_SIZE 48
static int PC_2[PC_2_SIZE] =
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

#ifdef TEST
static const unsigned int       check_R    = 0xF0AAF0AA;
static const unsigned int       check_F    = 0x234AA9BB;
static const long long unsigned check_M    = 0x0123456789ABCDEF;
static const long long unsigned check_C    = 0x85E813540F0AB405;
static const long long unsigned check_key  = 0x133457799BBCDFF1;
static const long long unsigned check_keys[16] =
{
  0x1B02EFFC70720000,
  0x79AED9DBC9E50000,
  0x55FC8A42CF990000,
  0x72ADD6DB351D0000,
  0x7CEC07EB53A80000,
  0x63A53E507B2F0000,
  0xEC84B7F618BC0000,
  0xF78A3AC13BFB0000,
  0xE0DBEBEDE7810000,
  0xB1F347BA464F0000,
  0x215FD3DED3860000,
  0x7571F59467E90000,
  0x97C5D1FABA410000,
  0x5F43B7F2E73A0000,
  0xBF918D3D3F0A0000,
  0xCB3D8B0E17F50000
};

bool
test(void);
#endif // TEST

enum operation {
  UNSPECIFIED = -1,
#ifdef TEST
  TESTING = 0,
#endif // TEST
  ENCRYPT = 1,
  DECRYPT = 2
};

int
main (int argc, char *argv[])
{
  char *inFile = NULL, *outFile = NULL, *keyFile = NULL;
  char opt = -1;
  int index, mode = UNSPECIFIED;

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
    if (mode != UNSPECIFIED)
    {
      fprintf(stderr, "des: operation already specified\n");
      return -1;
    }

    if (mode == UNSPECIFIED &&
        strcmp (argv[index], "encrypt") == 0)
    {
      mode = ENCRYPT;
    }
    else if (mode == UNSPECIFIED &&
             strcmp (argv[index], "decrypt") == 0)
    {
      mode = DECRYPT;
    }
#ifdef TEST
    else if (mode == UNSPECIFIED &&
             strcmp (argv[index], "test") == 0)
    {
      mode = TESTING;
    }
#endif // TEST
    else
    {
      fprintf(stderr, "des: unrecognized operation: %s\n", argv[index]);
      return -1;
    }
  }

  if (inFile == NULL || outFile == NULL)
  {
#ifdef TEST
    if (mode != TESTING)
    {
      fprintf(stderr, "des: must specify input file and output file\n");
      fprintf(stderr, "Usage: (encrypt | decrypt | test) -i <input_file> -o <output file> [-k <key file>]\n");
      return -1;
    }
#else
    fprintf(stderr, "des: must specify input file and output file\n");
    fprintf(stderr, "Usage: (encrypt | decrypt) -i <input_file> -o <output file> [-k <key file>]\n");

    return -1;
#endif // TEST
  }

  switch (mode)
  {
    case ENCRYPT:
      encryption (inFile, outFile, keyFile);
      break;
    case DECRYPT:
      decryption (inFile, outFile, keyFile);
      break;
#ifdef TEST
    case TESTING:
      if (test ())
      {
        printf("OVERALL TEST PASSED\n");
      }
      else
      {
        printf("OVERALL TEST FAILED\n");
      }
      break;
    default:
      fprintf(stderr, "des: must specify operation (decrypt, encrypt, test)\n");
      return -1;
#else
    default:
      fprintf(stderr, "des: must specify operation (decrypt, encrypt)\n");
      return -1;
#endif
  }
  return 0;
}

long long int
DES (int index, long long int *MD, long long int *keys)
{
  const long long unsigned lMask     = 0x8000000000000000;

  long long int M   = MD[index];
  long long int ip  = 0x0;
  long long int iip = 0x0;
  unsigned int  L   = 0x0;
  unsigned int  R   = 0x0;
  unsigned int  t;

  int k;

  for (k = 0; k < IP_SIZE; k++)
  {
     ip |= ((lMask & (M << (IP[k] - 1))) >> k);
  }

  R = *((unsigned int*)(&ip));
  L = *(((unsigned int*)(&ip)) + 1);

  for (k = 0; k < 16; k++)
  {
    t = L;

    // L(k+1) = R(k)
    L = R;
    // R(k+1) = L(k) ^ f(R(k), K(k+1))
    R = t ^ F(R, keys[k]);
  }

  t = L;
  L = R;
  R = t;

  *((unsigned int*)(&ip)) = R;
  *(((unsigned int*)(&ip)) + 1) = L;

  for (k = 0, iip = 0x0; k < IP_1_SIZE; k++)
  {
     iip |= ((lMask & (ip << (IP_1[k] - 1))) >> k);
  }

  return iip;
}

unsigned int
F (unsigned int c, long long int key)
{
  // constants
  //
  // iMask     : mask for MSB of unsigned int
  // lMask     : mask for MSB of long long unsigned
  // firstMask : mask for first bit of first 6 bits
  // lastMask  : mask for last bit  of first 6 bits
  // midMask   : mask for middle 4 bits of first 6 bits
  const unsigned int       iMask     = 0x80000000;
  const long long unsigned lMask     = 0x8000000000000000;
  const long long unsigned firstMask = 0x8000000000000000;
  const long long unsigned lastMask  = 0x0400000000000000;
  const long long unsigned midMask   = 0x7800000000000000;

  // variables
  long long unsigned e   = 0x0; // holds E-expanded key
  long long unsigned c64 = 0x0; // holds c in 64 bit (originally c is 32bit)
  unsigned int       s   = 0x0; // holds S-subtituted key
  unsigned int       p   = 0x0; // holds P-permuted s
  long long unsigned t   = 0x0; // temporary variable
  int i, j, k;                  // temporary variables

  // copy c to c64 (32 bit to 64 bit)
  memcpy (&c64, &c, 4);
  c64 <<= 32;

  // expand E
  for (k = 0; k < E_SIZE; k++)
  {
    e |= ((lMask & (c64 << (E[k] - 1))) >> k);
  }

  // XOR with round key
  e ^= key;

  // S-Box substitution
  for (k = 0; k < S_SIZE; k++, e <<= 6)
  {
    // get row number
    i = ((firstMask & e) == 0x0) ? 0 : 2;
    i += ((lastMask & e) == 0x0) ? 0 : 1;

    // get coloum number
    t = (midMask & e) >> 59;
    memcpy(&j, &t, 4);

    // merge S
    s |= (S[k][i][j]) << (4 * (S_SIZE - k - 1));
  }

  // permutation
  for (k = 0; k < P_SIZE; k++)
  {
    p |= ((iMask & (s << (P[k] - 1))) >> k);
  }

  return p;
}

void
keySchedule (long long int* keys, long long int key)
{
  // constants
  const long long unsigned lMask = 0x8000000000000000;
  const long long unsigned cMask = 0xFFFFFFF000000000;
  const long long unsigned dMask = 0x0000000FFFFFFF00;

  // variables
  long long unsigned C         = 0x0; // holds C0 to C16
  long long unsigned D         = 0x0; // holds D0 to D16
  long long unsigned p         = 0x0; // holds permuted keys
  long long unsigned t         = 0x0; // temporary variable

  // iteration variables
  int k, round;

  // apply key to PC-1
  for (k = 0; k < PC_1_SIZE; k++)
  {
    p |= ((lMask & (key << (PC_1[k] - 1))) >> k);
  }

  // compute C0, D0
  C = (p & cMask) | ((p & cMask) >> 28);
  D = (p & dMask) | ((p & dMask) << 28);

  // iterate over round
  for (round = 0; round < 16; round++)
  {
    // shift C(round-1) and D(round-1) to produce C(round), D(round)
    C <<= Shifts[round];
    D <<= Shifts[round];

    // merge C and D to apply PC-2
    t = (C & cMask) | ((D & cMask) >> 28);

    // apply PC-2 to produce key(round)
    for (k = 0, p = 0x0; k < PC_2_SIZE; k++)
    {
      p |= ((lMask & (t << (PC_2[k] - 1))) >> k);
    }
    keys[round] = p;
  }
}

// IO helpers for encryption/decryption
void
writefile_helper (char *filename, long long data[], int num_blocks)
{
  FILE *fp;

  fp = fopen(filename, "wb");
  if (fp == NULL)
  {
    fprintf(stderr, "Filepointer failed for %s", filename);
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  fwrite(data, sizeof(long long), num_blocks, fp);
  fclose(fp);
}

int
readfile_helper (long long **dst, char *filename)
{
  FILE *fp;
  long long *buffer;
  unsigned long file_size;
  unsigned long original_file_size;
  int blocks;

  fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    fprintf(stderr, "Filepointer failed for %s\n", filename);
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  fseek(fp, 0, SEEK_END);
  original_file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  blocks = (int) CEIL(original_file_size, sizeof (long long));
  file_size = blocks * sizeof(long long);
  buffer = malloc(file_size);

  if ( ! buffer)
  {
    fprintf(stderr, "dst malloc failed");
    exit(EXIT_FAILURE);
  }

  // Set to null for padding reasons
  if (original_file_size % 8 > 0)
  {
      buffer[blocks-1] = 0;
  }

  fread(buffer, file_size, 1, fp);
  fclose(fp);

  *dst =  buffer;

  return blocks;
}

void
reverse_keys(long long* keys)
{
    long long keys_tmp[16];
    int i;
    memcpy(keys_tmp, keys, 16 * sizeof(long long));
    for (i = 0; i < 16; i++)
    {
      keys[15-i] = keys_tmp[i];
    }
}

void
crypt_des (char *in, char *out, char *key, bool reverse_key)
{
  int NUM_BLOCKS;
  long long *input_data;
  long long *key_data;
  long long keys[16];
  int i;

  readfile_helper(&key_data, key);
  keySchedule(keys, *key_data);

  NUM_BLOCKS = readfile_helper(&input_data, in);
  long long output_data[NUM_BLOCKS];

  if (reverse_key)
  {
    reverse_keys(keys);
  }

  // Do DES
  for (i = 0; i < NUM_BLOCKS; i++)
  {
    output_data[i] = DES(i, input_data, keys);
  }

  writefile_helper(out, output_data, NUM_BLOCKS);

  free(input_data);
  free(key_data);

  return;
}

void
encryption (char *in, char *out, char *key)
{
  crypt_des(in, out, key, false);
  printf("des: encryption: in(%s) out(%s), key(%s)\n", in, out, key);

  return;
}

void
decryption (char *in, char *out, char *key)
{
  crypt_des(in, out, key, true);
  printf("des: decryption: in(%s) out(%s), key(%s)\n", in, out, key);
  return;
}

#ifdef TEST
bool
test (void)
{
  bool test_flag = true;
  long long test_keys[16];
  unsigned int test_f;
  long long test_M, test_C;

  long long int k, t;
  int i;

  printf ("Testing [key scheduling]\n");
  printf ("------------------------\n");

  keySchedule(test_keys, check_key);

  for (i = 0; i < 16; i++)
  {
    if (test_keys[i] == check_keys[i])
    {
      printf("Round %d: PASSED(%llx)\n", i+1, test_keys[i]);
    }
    else
    {
      printf("Round %d: FAILED(Exp:%llx, Got:%llx)\n",
             i+1, check_keys[i], test_keys[i]);
      test_flag = false;
    }
  }

  printf ("\n");
  printf ("Testing [f function]\n");
  printf ("--------------------\n");

  memcpy(&k, &check_keys[0], 8);

  test_f = F (check_R, k);

  if (test_f == check_F)
  {
    printf ("PASSED: f(%x)=%x\n", check_R, test_f);
  }
  else
  {
    printf ("FAILED: f(%x)=%x, Exp: %x\n", check_R, test_f, check_F);
    test_flag = false;
  }

  printf ("\n");
  printf ("Testing [DES operation]\n");
  printf ("-----------------------\n");

  for (i = 0; i < 16; i++)
  {
    test_keys[i] = check_keys[i];
  }

  test_M = check_M;

  test_C = DES (0, &test_M, test_keys);

  if (test_C == check_C)
  {
    printf ("PASSED: DES_encrypt(%llx)=%llx\n", test_M, test_C);
  }
  else
  {
    printf ("FAILED: DES_encrypt(%llx)=%llx, Exp: %llx\n\n", test_M, test_C, check_C);
    test_flag = false;
  }

  for (i = 0; i < 16; i++)
  {
    test_keys[15-i] = check_keys[i];
  }

  test_C = check_C;

  test_M = DES (0, &test_C, test_keys);

  if (test_M == check_M)
  {
    printf ("PASSED: DES_decrypt(%llx)=%llx\n\n", test_C, test_M);
  }
  else
  {
    printf ("FAILED: DES_decrypt(%llx)=%llx, Exp: %llx\n\n", test_C, test_M, check_M);
    test_flag = false;
  }

  return test_flag;
}
#endif // TEST
