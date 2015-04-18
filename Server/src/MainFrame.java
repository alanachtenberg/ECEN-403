import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.io.IOException;

/**
 * Created by Alan on 3/24/2015.
 */
public class MainFrame extends JFrame{
    public static final Console CONSOLE= new Console("CONSOLE");//our GUI console for printing information to the user.
    private static final String IMAGE_FILE="/res/ic_logo.png";
    private Server server;
    private JLabel dummy;
    private Image logo;

    MainFrame(){
        super();
    }

    MainFrame(String name){
        super(name);
    }


    public void initialize(){
        try {
            logo = ImageIO.read(getClass().getResource(IMAGE_FILE));
        }
        catch(IOException e){
            System.out.println("ERROR: IMAGE FILE FAILED TO READ");
            e.printStackTrace();
        }

        //TODO implement inteface
        dummy=new JLabel(new ImageIcon(logo));

        //Layout controls
        this.setLayout(new GridBagLayout());
        GridBagConstraints c=new GridBagConstraints();
        c.fill=GridBagConstraints.BOTH;
        c.weightx=1;
        c.weighty=1;
        c.gridx=0;c.gridy=0;
        c.gridwidth=2;c.gridheight=1;
        //ADD Console
        this.add(CONSOLE,c);
        //ADD Content
        c.gridx=2;
        c.gridy=0;
        c.gridwidth=3;
        c.gridheight=1;
        this.add(dummy,c);
        server= new Server(1234);
        new Thread(server).start();//start server on seperate thread
    }
}
