import javax.swing.*;
import java.awt.*;

/**
 * Created by Alan on 3/24/2015.
 */
public class MainFrame extends JFrame{
    public static final Console CONSOLE= new Console("CONSOLE");//our GUI console for printing information to the user.
    private Server server;
    private JButton dummy;
    MainFrame(){
        super();
    }

    MainFrame(String name){
        super(name);
    }


    public void initialize(){

        //TODO implement inteface
        dummy= new JButton("dummy");

        //Layout controls
        this.setLayout(new GridLayout(0,3));

        //ADD Console
        this.add(CONSOLE);
        //ADD Content
        this.add(dummy);
        this.add(new JButton("dummy 2"));
        server= new Server(1234);
        new Thread(server).start();//start server on seperate thread
    }
}
