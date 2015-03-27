import javax.swing.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
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
    public void run(){//main logic thread
        initIO();//initialize IO on our thread
        try {
            while(true){
                message=clientInput.readLine();
                if(message!=null) {
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run() {
                            console.writeln(message);
                        }
                    });
                }
                else{
                    if (socket.getInputStream().read(new byte[1])==-1){//attempt to read from input stream, returns -1 if fails
                        socket.close();//close socket
                        SwingUtilities.invokeLater(new Runnable() {
                            @Override
                            public void run() {
                                console.writeln(user+" disconnected  address: "+socket.getRemoteSocketAddress());
                            }
                        });
                        break;//break execution since socket is no longer valid
                    }

                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private void initIO(){
        try {
            clientInput= new BufferedReader(new InputStreamReader(socket.getInputStream()));//set up reader for input
            clientOutput= new PrintWriter(socket.getOutputStream());//set up writer for output
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}
