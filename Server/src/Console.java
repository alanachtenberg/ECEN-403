import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

/**
 * Created by Alan on 3/24/2015.
 */
public class Console  extends JPanel{
    private JLabel label;//name of console
    private JScrollPane scrollPane;//contains output textarea
    private JPopupMenu menu;
    private JTextArea output;//system output
    private JTextField input;//for user input

    Console(String name){
        super();

        //init Console Label
        label=new JLabel(name,JLabel.LEFT);
        label.setOpaque(true);//sets the background to solid instead of transparent
        label.setBackground(Color.WHITE);//white to match background of text areas

        initScrollPanel();//inits output textfield and adds to scroll pane

        initMenu();//inits popup menu for console

        initInput();//inits input textField

        initFonts();//Set fonts for label and output

        //LAYOUT OBJECTS
        this.setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();

        setGridPos(c, 0, 0, 1, 1);
        c.fill=GridBagConstraints.HORIZONTAL;
        c.anchor=GridBagConstraints.CENTER;
        this.add(label,c);

        setGridPos(c,0, 1, 1, 1);
        c.fill=GridBagConstraints.BOTH;
        c.anchor=GridBagConstraints.CENTER;
        c.weightx=1;//sets the scrollpane to get all the extra whitespace
        c.weighty=1;
        this.add(scrollPane,c);

        setGridPos(c,0, 2, 1, 1);
        c.fill=GridBagConstraints.HORIZONTAL;
        c.anchor=GridBagConstraints.CENTER;
        c.weightx=0;
        c.weighty=0;
        this.add(input,c);
    }

    private void initScrollPanel(){
        //init text area
        output= new JTextArea();//for whatever reason adding initial text prevents auto-scrolling, so no initial text :'(
        output.setLineWrap(true);
        output.setAlignmentY(JTextArea.BOTTOM_ALIGNMENT);
        output.setEditable(false);//sets text field as read only, only displays output
        output.setAutoscrolls(true);

        //init scroll pane
        scrollPane=new JScrollPane(output);//inits scroll pane to contain output text
        scrollPane.setVerticalScrollBarPolicy ( ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS );//always displays scroll bar
        scrollPane.setAutoscrolls(true);//sets pane to autoscroll to new content
    }
    private void initMenu(){
        //create menu
        menu= new JPopupMenu();

        //create clear button item
        JMenuItem menuItem = new JMenuItem("Clear");
        menuItem.addActionListener(new ActionListener() {//listens for item click
            @Override
            public void actionPerformed(ActionEvent e) {
                clear();//clear the console
            }
        });
        //add clear item
        menu.add(menuItem);

        //add menu to scroll pane
        output.add(menu);

        output.addMouseListener(new MouseListener() {//listen for clicks on text area
            @Override
            public void mouseClicked(MouseEvent e) {
                if (e.isPopupTrigger())
                    menu.show(e.getComponent(), e.getX(), e.getY());
            }

            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger())
                    menu.show(e.getComponent(), e.getX(), e.getY());
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger())
                    menu.show(e.getComponent(), e.getX(), e.getY());
            }

            @Override
            public void mouseEntered(MouseEvent e) {
                if (e.isPopupTrigger())
                    menu.show(e.getComponent(), e.getX(), e.getY());
            }

            @Override
            public void mouseExited(MouseEvent e) {
                if (e.isPopupTrigger())
                    menu.show(e.getComponent(), e.getX(), e.getY());
            }
        });
    }
    private void initInput(){
        input= new JTextField("Input");
        input.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {//enter button was pressed
                writeln(input.getText());//write text to the console
                input.setText("");//clear text input

                //TODO handle input for commands
            }
        });
    }
    private void initFonts(){
        label.setFont(new Font("Courier New", Font.ITALIC, 18));
        label.setForeground(Color.BLACK);
        output.setFont(new Font("Serif",Font.PLAIN,12));
        output.setForeground(Color.GRAY);
    }
    //helper function for setting up layout
    private void setGridPos(GridBagConstraints c,int x, int y, int w, int h){
        c.gridx=x;
        c.gridy=y;
        c.gridwidth=w;
        c.gridheight=h;
    }
    //Basic Interaction with the console
    //addCommand allows an actionlistener to be added to the input, to be defined elsewhere
    public void writeln(String s){
        output.append(s + "\n");
    }
    public void write(String s){
        output.append(s);
    }
    public void clear(){
        output.setText("");
    }
    public void addCommand(ActionListener al){
        input.addActionListener(al);
    }
}
