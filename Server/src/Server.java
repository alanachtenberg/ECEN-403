import javax.swing.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Inet4Address;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by Alan on 3/25/2015.
 */
public class Server implements Runnable{
    private ServerSocket serverSocket;//listening port connections
    private HashMap<String,Socket> clients;//hashmap of client connections, key is username value is clients socket
    public static ArrayList<String> connectedUsers;
    private String user;//current user

    private Inet4Address localHost;
    private int portNum;
    private String pass= "CARS";

    private static final Console console=MainFrame.CONSOLE;//use a local variable so that server class is interchangeable

    public Server(int port){
        try{
            clients= new HashMap<String, Socket>();
            connectedUsers =  new ArrayList<String>();
            portNum=port;
            localHost=(Inet4Address)Inet4Address.getLocalHost();
            serverSocket = new ServerSocket(portNum);//create a socket listening on port num
        }
        catch(Exception e){
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    public void run(){
        try{
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    console.writeln("Server Started address: " + localHost
                            + " port: " + Integer.toString(portNum));
                }
            });
            while(true) {
                Socket clientSocket = serverSocket.accept();
                if (authenticate(clientSocket) == false) {
                    if (clientSocket != null && !clientSocket.isClosed())
                        clientSocket.close();//authentication failed
                }
                else {
                    connectedUsers.add(user);//we have a legit user
                    clients.put(user, clientSocket);//store socket connection to be closed later
                    new Thread(new ClientThread(user,clientSocket)).start();//starts a new thread to handle client connection
                }
            }
        }
        catch(Exception e){
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        finally{
            try {
                for (int i=0;i<connectedUsers.size();++i) {
                    Socket sock = clients.get(connectedUsers.get(i));//close each client socket in our list
                    if (sock != null && sock.isClosed() == false)//make sure socket has not already been closed by client
                        sock.close();
                }
                    serverSocket.close();//finally close listening server socket
                } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    private Boolean authenticate(final Socket client) {
        try {
            InputStreamReader streamReader=new InputStreamReader(client.getInputStream());//clear input stream
            BufferedReader in = new BufferedReader(streamReader);
            PrintWriter out = new PrintWriter(client.getOutputStream(), true);
            //out.flush();//initial flush of output
            out.println("Hello I am your server input password to continue");
            String temp;
            for (int i =0;i<3;++i) {//give them 3 chances to input the password correctly
                if ( (temp=in.readLine()).equals(pass)) {
                    out.println("Connection Established");
                    out.println("Input UserName");
                    user=in.readLine();
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run() {//let the user know a new client connected
                            console.writeln("Client Connected address: " + client.getInetAddress() + " user: " + user);
                        }
                    });
                    return true;
                }
                else
                    out.println("Incorrect Password");
            }
            return false;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        return null;
    }

    public void closeConnection(String userName){
        Socket sock=clients.get(userName);
        if (sock!=null&&sock.isClosed()==false){
            try {
                sock.close();//close the connection
                clients.remove(userName);//remove client from map
                connectedUsers.remove(userName);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }
}
