import javax.swing.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Inet4Address;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.Buffer;

/**
 * Created by Alan on 3/25/2015.
 */
public class ClientThread implements Runnable{
    private String user;
    private Socket socket;
    private BufferedReader clientInput;
    private PrintWriter    clientOutput;
    private String message;
    private Console console=MainFrame.CONSOLE;

    ClientThread(String userName,Socket client){
        this.user=userName;
        socket=client;
        message=null;
    }
    public void run() {//main logic thread
        initIO();//initialize IO on our thread
        try {
            while (true) {
                message = clientInput.readLine();
                if (message != null && message != "") {
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run() {
                            console.writeln(user + ":" + message);
                        }
                    });
                } else {
                    if (socket.getInputStream().read(new byte[1]) == -1) {//attempt to read from input stream, returns -1 if fails
                        close();//close the bad connection
                        break;//stop trying to read from the closed connection
                    }

                }
            }
        } catch (IOException e) {
            //e.printStackTrace();
            close();
        }
    }

    private void initIO(){
        try {
            clientInput= new BufferedReader(new InputStreamReader(socket.getInputStream()));//set up reader for input
            clientOutput= new PrintWriter(socket.getOutputStream());//set up writer for output
        } catch (IOException e) {
            e.printStackTrace();
            close();
        }
    }
    public void close() {
        try {
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    console.writeln("Attempting close");
                }
            });
            final SocketAddress address=socket.getRemoteSocketAddress();//address of the client we are trying to close
            if (socket != null && socket.isClosed() == false) {
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        console.writeln(user + " closed connection @address: " + address);//use final var for address, so that we do not access socket
                    }
                });
                socket.close();//close socket
                socket=null;//set socket to null to prevent further attempts to access it
            }
        }
        catch(IOException e){
            e.printStackTrace();
        }
    }

}
