package alan.cardiac;

import android.content.Context;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.Socket;

/**
 * Created by Alan on 4/15/2015.
 */
public class TcpCommThread implements Runnable{
    private Context context;

    private Socket socket;
    private InetAddress hostAddress;
    private BufferedReader input;
    private PrintWriter output;
    private String serverPass;
    private String userName;
    private int port;



    TcpCommThread(String host,String portNum, String user, String pass, Context c) throws IOException {
        c=context;
        hostAddress= Inet4Address.getByName(host);
        userName=user;
        serverPass=pass;
        port=Integer.parseInt(portNum);

    }

    public void run(){
        if  (!connect()) {//try to connect
            Toast.makeText(context, "Failed to connect to server", Toast.LENGTH_SHORT);
        }
        else if (!authenticate()) {//try to authenticate
            Toast.makeText(context,"Failed to authenticate with Server, incorrect password",Toast.LENGTH_SHORT);
            close();//close the connection
        }
        else{//proceed to logic
            //TODO cool stuff man
            writeLn("Hello i am an android mobile device");

            //close();
        }

    }

    private Boolean authenticate(){
        writeLn(serverPass);
        String message=readLn();
        if (message.toLowerCase().startsWith("incorrect")){
            return false;
        }
        writeLn(userName);
        return true;
    }
    public void writeLn(String s){
        output.println(s);
        output.flush();
    }
    public String readLn(){
        try {
            String m=null;
            m = input.readLine();
            return m;
        }
        catch (IOException e){
            e.printStackTrace();
            close();
        }
        return null;
    }

    private Boolean connect(){
        try {
            socket = new Socket(hostAddress, port);
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(socket.getOutputStream());
            return true;
        }
        catch (IOException e){
            close();
        }
        return false;
    }

    public void close(){
        if (socket!=null&& socket.isConnected()){
            try {
                TcpService.connected=false;//let service know
                socket.close();
            } catch (IOException e) {
                socket=null;
            }
        }
    }
}
