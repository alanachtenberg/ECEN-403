/**
 * Created by Alan on 3/24/2015.
 */
import javax.swing.*;
import java.awt.*;

public class Main {
    public static final Dimension SCREEN_SIZE=Toolkit.getDefaultToolkit().getScreenSize();//gets screensize in pixels
    public static void main(String[] args){
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                //TODO move main method into its own class
                MainFrame mainWindow= new MainFrame("CARdiac");
                mainWindow.setMinimumSize(new Dimension(SCREEN_SIZE.width * 2 / 3, SCREEN_SIZE.height * 2 / 3));
                mainWindow.initialize();//must call initialize after setting size, to size elements of window properly
                mainWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);//exit program when main window closess
                mainWindow.setVisible(true);
            }
        });

    }
}
