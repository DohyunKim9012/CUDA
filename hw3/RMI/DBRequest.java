import java.rmi.Remote;
import java.rmi.RemoteException;

import java.io.FileNotFoundException;
import java.io.IOException;

public interface DBRequest extends Remote {
  public int DBWrite(int key, byte[] value) throws RemoteException;
  public byte[] DBRead(int key, int size) throws RemoteException;
  public byte[] DBGetPublic() throws RemoteException;
}
