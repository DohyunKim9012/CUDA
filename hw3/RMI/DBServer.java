import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.RandomAccessFile;
import java.io.IOException;

public class DBServer implements DBRequest {
  private File dbFile;

  private DBServer(String dbName)
    throws FileNotFoundException
  {
    this.dbFile = new File(dbName);
    this.initializeDB();
  }

  public synchronized int DBWrite(int key, byte[] value)
  {
    int success = -1;

    RandomAccessFile randomAccessFile = null;
  
    try
    {
      randomAccessFile = new RandomAccessFile(dbFile, "rw");
      randomAccessFile.seek((2048 + 8) * key);

      if (randomAccessFile.readInt() == key)
      {
        randomAccessFile.writeInt(value.length);
        randomAccessFile.write(value);
        randomAccessFile.close();

        success = 1;
      }
      else
      {
        randomAccessFile.close();
      }
    }
    catch (Exception e)
    {
      System.out.println("Server Exception: " + e.toString());
      e.printStackTrace();
    }

    return success;
  }

  public synchronized byte[] DBRead(int key, int size)
  {
    byte[] result = null;

    RandomAccessFile randomAccessFile = null;

    try
    {
      randomAccessFile = new RandomAccessFile(dbFile, "r");
      randomAccessFile.seek((2048+8) * key);

      if (randomAccessFile.readInt() == key)
      {
        size = randomAccessFile.readInt();
        result = new byte[size];
        if (randomAccessFile.read(result) < 0)
        {
          throw new Exception("Error message returned");
        }
      }

      randomAccessFile.close();
    }
    catch (Exception e)
    {
      System.out.println("Server Exception: " + e.toString());
      e.printStackTrace();
    }

    return result;
  }

  public void initializeDB()
  {
    System.out.println("Initializing DB:");

    RandomAccessFile randomAccessFile = null;
    try
    {
      randomAccessFile = new RandomAccessFile(dbFile, "rw");

      int key = 0;
      long eof = randomAccessFile.length();

      while(randomAccessFile.getFilePointer() < eof)
      {
        randomAccessFile.writeInt(key++);
        randomAccessFile.writeInt(0);
        randomAccessFile.seek(randomAccessFile.getFilePointer() + 2048);
      }

      randomAccessFile.close();

      System.out.println("Initialized DB: " + key + " keys");
    }
    catch (Exception e)
    {
      System.out.println("Server Exception: " + e.toString());
      e.printStackTrace();
    }

  }

  public static void main(String[] args)
  {
    String db = (args.length < 1) ? "simple.db" : args[0];

    try
    {
      DBServer server = new DBServer(db);

      DBRequest stub = (DBRequest) UnicastRemoteObject.exportObject(server, 0);

      Registry registry = LocateRegistry.getRegistry();
      registry.bind("DBRequest", stub);
      System.out.println("Server Ready");
    }
    catch (Exception e)
    {
      System.out.println("Server Exception: " + e.toString());
      e.printStackTrace();
    }
  }
}
