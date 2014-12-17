import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.RemoteException;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.BadPaddingException;
import javax.crypto.KeyAgreement;
import javax.crypto.spec.DHParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import java.math.BigInteger;
import java.security.PublicKey;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidKeyException;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.RSAPrivateKeySpec;
import java.security.spec.RSAPrivateCrtKeySpec;
import java.security.spec.RSAPublicKeySpec;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.X509EncodedKeySpec;

import java.util.Scanner;
import java.io.File;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;

public class DBClient {
  private DBClient() {}

  public enum Encryption {
    UnEncrypted, AES, DES, TripleDES, RSA, DH }

  public static SecretKey AESKey = null;
  public static SecretKey DESKey = null;
  public static SecretKey TripleDESKey = null;
  public static RSAPublicKey RSAPubKey = null;
  public static RSAPrivateKey RSAPriKey = null;
  public static KeyPair DHKeyPair = null;

  public static void main(String[] args)
  {
    if (System.getSecurityManager() == null)
    {
      System.setSecurityManager(new SecurityManager());
    }

    if (args.length < 3)
    {
      System.out.println("Usage <host> <trace_file> <response>");
      System.out.println("Insufficient number of option parameters given(> 4)");
    }

    String host = args[0];
    String trace = args[1];
    String response = args[2];

    long time = 0;

    try
    {
      System.out.println ("Client Start");

      System.out.println ("Key Generation");
      initializeKeys();

      Registry registry = LocateRegistry.getRegistry(host);
      DBRequest stub = (DBRequest) registry.lookup("DBRequest");

      System.out.println("Connection Check");
      stub.DBWrite (0, new byte[1024]);
      stub.DBRead (0, 1024);
      stub.DBGetPublic();

      System.out.println("Start Unencrypted Write");
      System.out.println("End Unencrypted Write @ " + doWrite (stub, trace, response + ".UnEncryptedWrite.out", Encryption.UnEncrypted) + " ms\n");
      System.out.println("Start Unencrypted Read");
      System.out.println("End Unencrypted Read @ "  + doRead (stub, trace, response + ".UnEncryptedRead.out", Encryption.UnEncrypted) + " ms\n");

      System.out.println("Start AES Write");
      System.out.println("End AES Write @ " + doWrite (stub, trace, response + ".AESWrite.out", Encryption.AES)  + " ms\n");
      System.out.println("Start AES Read");
      System.out.println("End AES Read @ " + doRead (stub, trace, response + ".AESRead.out", Encryption.AES) + " ms\n");

      System.out.println("Start DES Write");
      System.out.println("End DES Write @ " + doWrite (stub, trace, response + ".DESWrite.out", Encryption.DES) + " ms\n");
      System.out.println("Start DES Read");
      System.out.println("End DES Read @ " + doRead (stub, trace, response + ".DESRead.out", Encryption.DES) + " ms\n");

      System.out.println("Start 3DES Write");
      System.out.println("End 3DES Write @ " + doWrite (stub, trace, response + ".3DESWrite.out", Encryption.TripleDES) + " ms\n");
      System.out.println("Start 3DES Read");
      System.out.println("End 3DES Read @ " + doRead (stub, trace, response + ".3DESRead.out", Encryption.TripleDES) + " ms\n");

      System.out.println("Start RSA Write");
      System.out.println("End RSA Write @ " + doWrite (stub, trace, response + ".RSAWrite.out", Encryption.RSA) + " ms\n");
      System.out.println("Start RSA Read");
      System.out.println("End RSA Read @ " + doRead (stub, trace, response + ".RSARead.out", Encryption.RSA) + " ms\n");

      System.out.println("Start AES+DH Write");
      System.out.println("End AES+DH Write @ " + doWrite (stub, trace, response + ".DHWrite.out", Encryption.DH) + "ms\n");

      System.out.println("Start AES+DH Read");
      System.out.println("End AES+DH Read @ " + doRead (stub, trace, response + ".DHRead.out", Encryption.DH) + " ms\n");
    }
    catch (Exception e)
    {
      System.err.println("Client Exception " + e.toString());
      e.printStackTrace();
    }
  }

  public static long doRead (DBRequest stub, String trace, String response, Encryption e)
  {
    System.out.println("Client Start Read Request");

    long total_time = System.currentTimeMillis();

    try
    {
      // To read in trace file
      Scanner scanner = new Scanner(new File(trace));

      // To write out response time
      PrintWriter writer = new PrintWriter(response, "UTF-8");
      writer.println("READ");

      while (scanner.hasNext())
      {
        int key = scanner.nextInt();
        int length = scanner.nextInt();

        long time = System.currentTimeMillis();

        byte[] data = stub.DBRead(key, length);

        long rTime = System.currentTimeMillis() - time;

        time = System.currentTimeMillis();

        data = decrypt(e, data, stub);

        long eTime = System.currentTimeMillis() - time;

        writer.println("" + eTime + "\t" + rTime + "\t" + (eTime + rTime));
      }

      System.out.println("Client End Read Request");

      scanner.close();
      writer.close();
    }
    catch (Exception ex)
    {
      System.err.println("Client Exception " + ex.toString());
      ex.printStackTrace();
    }

    return (System.currentTimeMillis() - total_time);
  }

  public static long doWrite (DBRequest stub, String trace, String response, Encryption e)
  {
    System.out.println("Client Start Write Request");

    long total_time = System.currentTimeMillis();

    try
    {
      // To read in trace file
      Scanner scanner = new Scanner(new File(trace));

      // To write out response time
      PrintWriter writer = new PrintWriter(response, "UTF-8");
      writer.println("WRITE");

      while (scanner.hasNext())
      {
        int key = scanner.nextInt();
        int length = scanner.nextInt();

        byte[] data = new byte[length];

        long time = System.currentTimeMillis();

        data = encrypt(e, data, stub);

        long eTime = System.currentTimeMillis() - time;

        time = System.currentTimeMillis();

        stub.DBWrite(key, data);

        long rTime = System.currentTimeMillis() - time;

        writer.println("" + eTime + "\t" + rTime + "\t" + (eTime + rTime));
      }

      System.out.println("Client End Write Request");

      scanner.close();
      writer.close();
    }
    catch (Exception ex)
    {
      System.err.println("Client Exception " + ex.toString());
      ex.printStackTrace();
    }

    return (System.currentTimeMillis() - total_time);
  }

  public static void initializeKeys() throws InvalidKeySpecException, NoSuchAlgorithmException
  {
    KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");
    keyGenerator.init(128);
    AESKey = keyGenerator.generateKey();

    keyGenerator = KeyGenerator.getInstance("DES");
    keyGenerator.init(56);
    DESKey = keyGenerator.generateKey();

    keyGenerator = KeyGenerator.getInstance("DESede");
    keyGenerator.init(112);
    TripleDESKey = keyGenerator.generateKey();

    KeyFactory keyFactory = KeyFactory.getInstance("RSA");
    BigInteger modulus = new BigInteger("00db72574f94eb6c3a7b4804d2bb2393b33ad71764cc61e1c5d11217daabeb42be974ce3def1155601b169bbfe7e1cdc973f74d90d2af91da08cf93ee073487cc1d3f684b3972a26171f701a9f5ac9a5e8c83db8dd05e4bac24b66b9565762ac320a3ba446021414381b471d1ae8426e99e869261e740e589ebd0bfc2a3ef7a60b", 16);
    BigInteger publicExponent = new BigInteger("65537");
    BigInteger privateExponent = new BigInteger("406088d0a5e572b8ad55f16448d6250c5d64a04e47cf2746bb10cdc2aed0b3a1ea0cfa847ac1419bf98a0546a184a20a14e9988b4e620a99010255b9ced82310c66f3292dfcb10c81211d36dec623f66f1823e351152af31bacb79d9b0efea04e7dbec04aee4e5c32156a13b4fd7577a8e34ee3c14d0beaaefad33b568888001", 16);
    BigInteger P = new BigInteger("00faa6babad8c2ceaed32154105861708419175fe4afa03c4875f29d7fbcd2f9848e88c0edcebb78753f10461dd96b8d94b9407ef99d4fc3853f163f57e59a6001", 16);
    BigInteger Q = new BigInteger("00e021253507d336e05666efadfbb8730dcd92a1a0a60e5faf77cde607683c177d7f18b2d0c5949263075515a6ae3dd0b943b976d9615f54e5be82ba6ab315860b", 16);
    BigInteger exponent1 = new BigInteger("00a760f789167ee06e3667272fd3151e81d80f97e5aab2220f71541680daa33824a0da099bc455c456e74a02c8a40c027bb249b38114dfd2fd03e7e35cdae28001", 16);
    BigInteger exponent2 = new BigInteger("00abd19d156f5f43456dd751ccbe1963d2641311e6f70633e26f64a00073af058f069d279016b7d41528acd88144384d47899ca778f445c1eeb242c3a8428bb08b", 16);
    BigInteger coefficient = new BigInteger("00cc058c2f1b36c0d3781c7d67af707509fc60d00dc0e0b1a7e1bdbf725a59ef2a8a407a1800db40cde392e6f45424509c87e06679ae7ad9fbea8a184f24f27231", 16);

    RSAPrivateKeySpec privateKeySpec =
      new RSAPrivateCrtKeySpec (
          modulus, publicExponent, privateExponent, P, Q, exponent1, exponent2, coefficient);
    RSAPublicKeySpec publicKeySpec =
      new RSAPublicKeySpec (
          modulus, publicExponent);
    RSAPubKey = (RSAPublicKey) keyFactory.generatePublic(publicKeySpec);
    RSAPriKey = (RSAPrivateKey) keyFactory.generatePrivate(privateKeySpec);

    KeyPairGenerator keyGen = KeyPairGenerator.getInstance("DH");
    keyGen.initialize(1024);
    DHKeyPair = keyGen.genKeyPair();
  }

  public static byte[] encrypt(Encryption e, byte[] text, DBRequest stub)
    throws InvalidKeyException,
           NoSuchAlgorithmException,
           IllegalBlockSizeException,
           NoSuchPaddingException,
           BadPaddingException,
           RemoteException,
           InvalidKeySpecException
  {
    Cipher cipher = null;
    switch (e)
    {
      case UnEncrypted:
        return text;
      case AES:
        cipher = Cipher.getInstance("AES");
        cipher.init(Cipher.ENCRYPT_MODE, AESKey);
        break;
      case DES:
        cipher = Cipher.getInstance("DES");
        cipher.init(Cipher.ENCRYPT_MODE, DESKey);
        break;
      case TripleDES:
        cipher = Cipher.getInstance("DESede");
        cipher.init(Cipher.ENCRYPT_MODE, TripleDESKey);
        break;
      case RSA:
        return encrypt_rsa(text);
      case DH:
        return encrypt_dh(text, stub);
    }
    return cipher.doFinal(text);
  }

  public static byte[] encrypt_dh(byte[] text, DBRequest stub)
    throws NoSuchAlgorithmException,
           RemoteException,
           InvalidKeySpecException,
           InvalidKeyException,
           NoSuchPaddingException,
           IllegalBlockSizeException,
           BadPaddingException
  {
    PublicKey publicKey = KeyFactory.getInstance("DH").generatePublic(new X509EncodedKeySpec(stub.DBGetPublic()));

    KeyAgreement keyAgreement = KeyAgreement.getInstance("DH");
    keyAgreement.init(DHKeyPair.getPrivate());
    keyAgreement.doPhase(publicKey, true);

    Cipher cipher = null;
    cipher = Cipher.getInstance("AES");
    
    SecretKey aesKey = new SecretKeySpec (keyAgreement.generateSecret(), 0, 16, "AES");

    cipher.init(Cipher.ENCRYPT_MODE, aesKey);

    return cipher.doFinal(text);
  }

  public static byte[] encrypt_rsa(byte[] text) throws InvalidKeyException, IllegalBlockSizeException, NoSuchAlgorithmException, BadPaddingException, NoSuchPaddingException
  {
    int numBlocks = (text.length % 117 == 0) ?
                    (text.length / 117) : (text.length / 117) + 1;

    Cipher cipher = null;
    cipher = Cipher.getInstance("RSA");
    cipher.init(Cipher.ENCRYPT_MODE, RSAPriKey);

    byte[] result = new byte[128*numBlocks];

    for (int i = 0; i < numBlocks; i++)
    {
      int low = i * 117;
      int high = ((i + 1) * 117 > text.length) ?
                 text.length : ((i + 1) * 117);

      byte[] block = new byte[117];
      for (int j = 0; j < 117; j++)
      {
        block[j] = (i * 117 + j >= text.length) ?
                   0 : text[i * 117 + j];
      }

      byte[] blockCipher = cipher.doFinal(block);

      for (int j = 0; j < 128; j++)
      {
        result[i * 128 + j] = blockCipher[j];
      }
    }

    return result;
  }

  public static byte[] decrypt(Encryption e, byte[] text, DBRequest stub)
    throws InvalidKeyException,
           NoSuchAlgorithmException,
           IllegalBlockSizeException,
           InvalidKeySpecException,
           NoSuchPaddingException,
           BadPaddingException,
           RemoteException
  {
    Cipher cipher = null;
    switch (e)
    {
      case UnEncrypted:
        return text;
      case AES:
        cipher = Cipher.getInstance("AES");
        cipher.init(Cipher.DECRYPT_MODE, AESKey);
        break;
      case DES:
        cipher = Cipher.getInstance("DES");
        cipher.init(Cipher.DECRYPT_MODE, DESKey);
        break;
      case TripleDES:
        cipher = Cipher.getInstance("DESede");
        cipher.init(Cipher.DECRYPT_MODE, TripleDESKey);
        break;
      case RSA:
        return decrypt_rsa(text);
      case DH:
        return decrypt_dh(text, stub);
    }
    return cipher.doFinal(text);
  }

  public static byte[] decrypt_dh(byte[] text, DBRequest stub)
    throws NoSuchAlgorithmException,
           RemoteException,
           InvalidKeySpecException,
           InvalidKeyException,
           NoSuchPaddingException,
           IllegalBlockSizeException,
           BadPaddingException
  {
    PublicKey publicKey = KeyFactory.getInstance("DH").generatePublic(new X509EncodedKeySpec(stub.DBGetPublic()));

    KeyAgreement keyAgreement = KeyAgreement.getInstance("DH");
    keyAgreement.init(DHKeyPair.getPrivate());
    keyAgreement.doPhase(publicKey, true);

    Cipher cipher = null;
    cipher = Cipher.getInstance("AES");
    
    SecretKey aesKey = new SecretKeySpec (keyAgreement.generateSecret(), 0, 16, "AES");

    cipher.init(Cipher.DECRYPT_MODE, aesKey);

    return cipher.doFinal(text);
  }

  public static byte[] decrypt_rsa(byte[] text) throws NoSuchAlgorithmException, InvalidKeyException, IllegalBlockSizeException, NoSuchPaddingException, BadPaddingException
  {
    Cipher cipher = null;
    cipher = Cipher.getInstance("RSA");
    cipher.init(Cipher.DECRYPT_MODE, RSAPubKey);

    int numBlocks = text.length / 128;
    byte[] result = new byte[numBlocks * 117];

    for (int i = 0; i < numBlocks; i++)
    {
      byte[] blockText = new byte[128];

      for (int j = 0; j < 128; j++)
      {
        blockText[j] = text[i*128 + j];
      }

      byte[] decrypt = cipher.doFinal(blockText);

      for (int j = 0; j < 117; j++)
      {
        result[117 * i + j]= decrypt[j];
      }
    }

    return result;
  }
}
