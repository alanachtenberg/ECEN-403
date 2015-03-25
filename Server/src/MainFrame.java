import javax.swing.*;
import java.awt.*;

/**
 * Created by Alan on 3/24/2015.
 */
public class MainFrame extends JFrame{
    private static Console console;
    private JButton dummy;
    MainFrame(){
        super();
    }

    MainFrame(String name){
        super(name);
    }


    public void initialize(){
        //init console
        console= new Console("CONSOLE");

        //TODO implement inteface
        dummy= new JButton("dummy");
        //Create LayoutS
        this.setLayout(new GridLayout(0,2));

        this.add(console);
        //ADD Content
        this.add(dummy);

        for(int i=0;i<100;++i){
            console.writeln(Integer.toString(i));
        }
        console.writeln("testing scroll text output");
    }
}
