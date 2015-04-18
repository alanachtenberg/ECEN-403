package alan.cardiac;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.UUID;

/**
 * Created by Alan on 3/31/2015.
 */
public class BtCommThread extends Thread {
    private static final String TAG="Bluetooth";
    private static UUID uuid=UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private Context context;

    private BluetoothAdapter mBlueToothAdapter;
    private BluetoothDevice device;
    private BluetoothSocket socket;

    private StringBuffer inBuffer;//buffer for recieved messages from server
    private BufferedReader input;
    private PrintWriter output;

    /*
    *@param c pass in context for making toasts to user
    */
    BtCommThread(BluetoothDevice btDevice, Context c) throws IOException{//let caller handle Exception with constructor
        context=c;
        device=btDevice;
        mBlueToothAdapter=BluetoothAdapter.getDefaultAdapter();
        socket = device.createInsecureRfcommSocketToServiceRecord(uuid);
        socket.connect();//attempt connection
        input= new BufferedReader(new InputStreamReader(socket.getInputStream()));//get input
        output = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));//get output
    }

    @Override
    public void run(){
        try {
                String message="";
                message=input.readLine();
                Log.i(TAG,message);//show console
                output.write("Hello I am your android master");//say hello to server
                output.flush();//flush tells the socket to send the data immediately, rather than wait for local buffer to fill
        }
         catch (IOException e) {
            e.printStackTrace();
            close();//close the connection
        }
    }

    public void write(String s){
            output.write(s);
            output.flush();
    }
    public String read(){
        try {
            return input.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
    /*
    *Closes Socket connection, must be called when thread is being disposed
    * to prevent socket.read() from preventing interruption of thread
     */
    public void close(){
        try {
            if (socket!=null&& socket.isConnected()==true) {
                this.socket.close();
                this.socket = null;//set socket to null
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
