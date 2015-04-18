package alan.cardiac;

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
    Socket socket;
    InetAddress hostAddress;
    BufferedReader input;
    PrintWriter output;
    String serverPass;
    private int port;
    TcpCommThread(String host,String portNum, String pass) throws IOException {
        hostAddress= Inet4Address.getByName(host);
        serverPass=pass;
        port=Integer.parseInt(portNum);
    }

    public void run(){
        try {
            socket= new Socket(hostAddress,port);
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(socket.getOutputStream());
            while (true) {
                String message = input.readLine();
            }
        }
        catch (IOException e){

        }
    }
}
