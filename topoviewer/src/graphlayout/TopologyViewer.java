package graphlayout;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import javax.swing.*;
import javax.swing.event.*;

import graphlayout.interaction.*;

public class TopologyViewer extends JFrame{
  public static String VERSION = "v0.7.6";                         //revision of this program
  public static Font STD_FONT = new Font("Arial", Font.PLAIN, 12); //standard font
  public static Color BACKGROUND = Color.LIGHT_GRAY;               //standard backround
  public static Color FOREGROUND = Color.BLACK;                    //standard foreground
  
  private static JFrame topology_viewer;                           //reference to TopologyViewer itself

  private static int port;                                         //port number for socket
  private static String host;                                      //host address for socket
  private static String loc_path;                                  //path to a local file
  private static String map_path;                                  //path to background map
  private static boolean console;                                  //starting programm with console parameters?
  private static boolean reconnect;                                //reconnect if socket error
  
  private JMenuItem map_item;                                      //menu entry for loading map
  private JMenuItem exp_item;                                      //menu entry for export graphic
  private TGPanel tg_panel;                                        //the touchgraph panel
  protected TGLensSet tg_lens_set;

  public HVScroll hv_scroll;
  public LocalityScroll locality_scroll;
  
  /**
   * Constructor for class TopologyViewer
   */
  public TopologyViewer(){
    super("Topology-Viewer " + VERSION);
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    Container frame = getContentPane();
  
    //--parameter for the program itself--//
    frame.setFont(STD_FONT);
    frame.setLayout(new BorderLayout(0, 1));
    frame.setBackground(BACKGROUND);
    frame.setForeground(FOREGROUND);
      
    //--build touchgraph panel--//
    JPanel top = new JPanel();
    top.setFont(STD_FONT);
    top.setLayout(new BorderLayout());
    top.setBackground(BACKGROUND);
    top.setForeground(FOREGROUND);
    frame.add(top, BorderLayout.CENTER);
    tg_panel = new TGPanel();
    tg_panel.setBackColor(Color.WHITE);
    top.add(tg_panel, BorderLayout.CENTER); 
    tg_lens_set = new TGLensSet();
    hv_scroll = new HVScroll(tg_panel, tg_lens_set);
    locality_scroll = new LocalityScroll(tg_panel);
    top.add(hv_scroll.getVerticalSB(), BorderLayout.EAST);
    top.add(hv_scroll.getHorizontalSB(), BorderLayout.SOUTH);

    //--fictive panel--//
    JPanel below = new JPanel();
    below.setFont(STD_FONT);
    below.setLayout(new BorderLayout(0, 0));
    below.setBackground(BACKGROUND);
    below.setForeground(FOREGROUND);
    frame.add(below, BorderLayout.SOUTH);

    //--build control panel--//
    JPanel control = new JPanel();
    control.setFont(STD_FONT);
    control.setLayout(new GridLayout(1, 3, 15, 10));
    control.setBackground(BACKGROUND);
    control.setForeground(FOREGROUND);
    //frame.add(control, BorderLayout.SOUTH);
    below.add(control, BorderLayout.CENTER);
    
    JGroupBox gpb_rot = new JGroupBox();
    gpb_rot.setFont(STD_FONT);
    gpb_rot.setLayout(new GridLayout(3, 1, 15, 10));
    gpb_rot.setBackground(BACKGROUND);
    gpb_rot.setForeground(FOREGROUND);
    control.add(gpb_rot);
    
    JLabel lbl_rot = new JLabel("Rotate", JLabel.CENTER);
    lbl_rot.setFont(new Font("Arial", Font.BOLD, 12));
    lbl_rot.setBackground(BACKGROUND);
    lbl_rot.setForeground(FOREGROUND);
    gpb_rot.add(lbl_rot); 
    
    final JSlider sld_rot = new JSlider(-180, 180);
    sld_rot.setBackground(BACKGROUND);
    sld_rot.setForeground(FOREGROUND);
    gpb_rot.add(sld_rot);
    final RotateScroll rotatebar = new RotateScroll(tg_panel, sld_rot, 0);

    final JLabel lbl_dis_rot = new JLabel("0", JLabel.CENTER);
    lbl_dis_rot.setFont(STD_FONT);
    lbl_dis_rot.setBackground(BACKGROUND);
    lbl_dis_rot.setForeground(FOREGROUND);
    gpb_rot.add(lbl_dis_rot);

    JGroupBox gpb_zoom = new JGroupBox();
    gpb_zoom.setFont(STD_FONT);
    gpb_zoom.setLayout(new GridLayout(3, 1, 15, 10));
    gpb_zoom.setBackground(BACKGROUND);
    gpb_zoom.setForeground(FOREGROUND);
    control.add(gpb_zoom);

    JLabel lbl_zoom = new JLabel("Zoom", JLabel.CENTER);
    lbl_zoom.setFont(new Font("Arial", Font.BOLD, 12));
    lbl_zoom.setBackground(BACKGROUND);
    lbl_zoom.setForeground(FOREGROUND);
    gpb_zoom.add(lbl_zoom);

    final JSlider sld_zoom = new JSlider(-31, 19);
    sld_zoom.setBackground(BACKGROUND);
    sld_zoom.setForeground(FOREGROUND);
    sld_zoom.setValue(0);
    gpb_zoom.add(sld_zoom);
    final ZoomScroll zoombar = new ZoomScroll(tg_panel, sld_zoom, 1.0);

    final JLabel lbl_dis_zoom = new JLabel("20.00 pxpm", JLabel.CENTER);
    lbl_dis_zoom.setFont(STD_FONT);
    lbl_dis_zoom.setBackground(BACKGROUND);
    lbl_dis_zoom.setForeground(FOREGROUND);
    gpb_zoom.add(lbl_dis_zoom);

    JPanel fill = new JPanel();
    fill.setFont(STD_FONT);
    fill.setBackground(BACKGROUND);
    fill.setForeground(FOREGROUND);
    control.add(fill);

    ChangeListener slideListener = new ChangeListener(){
      public void stateChanged(ChangeEvent e){
        if(e.getSource() == sld_rot){
          rotatebar.setRotateValue(sld_rot.getValue());
          lbl_dis_rot.setText(sld_rot.getValue() + " ");
        } if(e.getSource() == sld_zoom){
          zoombar.setZoomValue(Math.pow(2, sld_zoom.getValue() / 10.0));
          String info = "" + (zoombar.getZoomValue() * tg_panel.getPixelForOneMeter());
          int idx = info.indexOf('.');
          if(idx == -1)info += ".00";
          else if((idx + 3) < info.length())info = info.substring(0, idx + 3);
          else info = info.substring(0, idx + 2) + "0";
          lbl_dis_zoom.setText(info + " pxpm");
        } //of if
      } //of stateChanged
    }; //of ChangeListener 
    sld_rot.addChangeListener(slideListener);
    sld_zoom.addChangeListener(slideListener);    
/*
    tg_panel.addComponentListener(new ComponentAdapter(){
      public void componentResized(ComponentEvent e){
        if(tg_panel.isMapLoaded()){
          sld_rot.setValue(0);
          sld_zoom.setValue(0);
          tg_panel.resetBackground();
        } //of if
      } //of componentResized
    }); //of ComponentAdapter 
*/

    //--build statusbar--//
    JTopoStatusBar statusBar = new JTopoStatusBar();
    statusBar.setFont(STD_FONT);
    statusBar.setBackground(BACKGROUND);
    statusBar.setForeground(FOREGROUND);
    below.add(statusBar, BorderLayout.SOUTH);

    //--build main menu--//
    setJMenuBar(buildMenu(sld_rot, sld_zoom, statusBar, control));
    
    setSize(800, 600);
    setLocation((Toolkit.getDefaultToolkit().getScreenSize().width - getWidth()) >> 1,
                (Toolkit.getDefaultToolkit().getScreenSize().height - getHeight()) >> 1);
    
    //--build lens set--//
    tg_lens_set.addLens(hv_scroll.getLens());
    tg_lens_set.addLens(zoombar.getLens());
    tg_lens_set.addLens(rotatebar.getLens());
    tg_lens_set.addLens(tg_panel.getAdjustOriginLens());
    tg_panel.setLensSet(tg_lens_set);

    addUIs();

    setVisible(true);

    //--perhaps load information from console--//
    //--graph--//
    if(console){
      if(loc_path != null){
        new InputParser(tg_panel, statusBar, loc_path);
        map_item.setEnabled(true);
        exp_item.setEnabled(true);
        //--map--//
        tg_panel.loadMap(map_path);
      } else {
        new InputParser(tg_panel, statusBar, host, port, reconnect);
        map_item.setEnabled(true);
        exp_item.setEnabled(true);
        //--map--//
        tg_panel.loadMap(map_path);
      } //of if-else
    } //of if
  } //of TopologyViewer
  
  private JMenuBar buildMenu(final JSlider sld_rot, final JSlider sld_zoom, final JTopoStatusBar statusBar, final JPanel ctrl){
    final String menu_text[] = {"Save topology", "Load topology", "Connect to host", 
                                "Load positions", "Load map", "Export graphic", "Quit",
                                "Toggle Metric panel", "Toggle Control panel", "About"};
    //--action commands of the menubar--//
    ActionListener menuAction = new ActionListener(){
      public void actionPerformed(ActionEvent e){
        String cmd = e.getActionCommand();
        if(cmd.equals(menu_text[0])){
          JFileChooser dialog = new JFileChooser();
          JFileFilter filter = new JFileFilter();
          filter.addExtension("xml");
          filter.setDescription("*.xml");
          dialog.setFileFilter(filter);
          dialog.setDialogType(JFileChooser.SAVE_DIALOG);
          if(dialog.showDialog(topology_viewer, "Save topology") == JFileChooser.APPROVE_OPTION){
            String path = dialog.getSelectedFile().getAbsolutePath();
            if(!path.toLowerCase().endsWith(".xml"))path += ".xml";
            XMLWriter xml_writer = new XMLWriter(tg_panel.getGraphEltSet(), tg_panel.getPixelForOneMeter());
            xml_writer.writeXML(path);
          } //of if          
        } else if(cmd.equals(menu_text[1])){
          JFileChooser dialog = new JFileChooser();
          JFileFilter filter = new JFileFilter();
          filter.addExtension("xml");
          filter.setDescription("*.xml");
          dialog.setFileFilter(filter);
          dialog.setDialogType(JFileChooser.OPEN_DIALOG);
          if(dialog.showDialog(topology_viewer, "Load topology") == JFileChooser.APPROVE_OPTION){
            String tmp_loc_path = dialog.getSelectedFile().getAbsolutePath();
            sld_rot.setValue(0);
            sld_zoom.setValue(0);
            if((tmp_loc_path != null)&&(!tmp_loc_path.equals(loc_path))){
              loc_path = tmp_loc_path;
              tg_panel.clearAll();
              new InputParser(tg_panel, statusBar, loc_path);
              map_item.setEnabled(true);
              exp_item.setEnabled(true);
              System.gc();
            } //of if         
          } //of if          
        } else if(cmd.equals(menu_text[2])){
          class ConnectDialog extends JDialog implements ActionListener{
            private boolean press_ok;
            private JButton ok, cancel;
            private JCheckBox reconnect_field;
            private JTextField host_field, port_field;

            public ConnectDialog(JFrame owner){
              super(owner, "Connect to host");

              press_ok = false;

              setModal(true);
              setSize(300, 200);
              setLocation((owner.getX() + (owner.getX() + owner.getWidth() - getWidth()) >> 1),
                          (owner.getY() + (owner.getY() + owner.getHeight() - getHeight()) >> 1));
              setResizable(false);
              Container dialog = getContentPane();
              dialog.setLayout(null);
              dialog.setBackground(BACKGROUND);
              dialog.setForeground(FOREGROUND);

              JGroupBox groupbox = new JGroupBox();
              groupbox.setFont(STD_FONT);
              groupbox.setLayout(null);
              groupbox.setBackground(BACKGROUND);
              groupbox.setForeground(FOREGROUND);
              groupbox.setBounds(15, 10, 270, 105);
              dialog.add(groupbox);

              JLabel label = new JLabel("Host:");
              label.setFont(new Font("Arial", Font.BOLD, 12));
              label.setBackground(BACKGROUND);
              label.setForeground(FOREGROUND);
              label.setBounds(20, 15, 65, 15);
              groupbox.add(label);

              label = new JLabel("Port:");
              label.setFont(new Font("Arial", Font.BOLD, 12));
              label.setBackground(BACKGROUND);
              label.setForeground(FOREGROUND);
              label.setBounds(20, 45, 65, 15);
              groupbox.add(label);

              label = new JLabel("Reconnect:");
              label.setFont(new Font("Arial", Font.BOLD, 12));
              label.setBackground(BACKGROUND);
              label.setForeground(FOREGROUND);
              label.setBounds(20, 75, 65, 15);
              groupbox.add(label);

              reconnect_field = new JCheckBox("", reconnect);
              reconnect_field.setFont(STD_FONT);
              reconnect_field.setBackground(BACKGROUND);
              reconnect_field.setForeground(FOREGROUND);
              reconnect_field.setBounds(90, 75, 20, 15);
              groupbox.add(reconnect_field);

              host_field = new JTextField(host);
              host_field.setFont(STD_FONT);
              host_field.setBounds(95, 14, 155, 17);
	      host_field.addActionListener(this);

	      groupbox.add(host_field);
	      
              port_field = new JTextField("" + port);
              port_field.setFont(STD_FONT);
              port_field.setBounds(95, 44, 155, 17);
              groupbox.add(port_field);

              ok = new JButton("Ok");
              ok.setFont(STD_FONT);
              ok.setBackground(BACKGROUND);
              ok.setForeground(FOREGROUND);
              ok.setBounds(50, 130, 75, 30);
              ok.setMnemonic('O');
              ok.addActionListener(this);
              dialog.add(ok);

              cancel = new JButton("Cancel");
              cancel.setFont(STD_FONT);
              cancel.setBackground(BACKGROUND);
              cancel.setForeground(FOREGROUND);
              cancel.setBounds(175, 130, 75, 30);
              cancel.setMnemonic('C');
              cancel.addActionListener(this);
              dialog.add(cancel);

              addWindowListener(new WindowAdapter(){
                public void windowClosing(WindowEvent e){
                  dispose();
                } //of windowClosing
              }); //of WindowAdapter
              setVisible(true);
            } //of InfoWindow

            public boolean isAccepted(){
              return press_ok;
            } //of isAccepted

            public void actionPerformed(ActionEvent e){
		if(e.getSource() == ok || e.getSource() == host_field){
                reconnect = reconnect_field.isSelected();
                host = host_field.getText();
                try{
                  port = new Integer(port_field.getText()).intValue();
                } catch(NumberFormatException ex){
                  ex.printStackTrace();
                } //of try-catch
                press_ok = true;
                dispose();
              } if(e.getSource() == cancel){
                press_ok = false;
                dispose();
              } //of if
            } //of actionPerformed
          }; //of class ConnectDialog
          ConnectDialog cd = new ConnectDialog(topology_viewer);
          if(cd.isAccepted()){
            tg_panel.clearAll();
            new InputParser(tg_panel, statusBar, host, port, reconnect);
            map_item.setEnabled(true);
            exp_item.setEnabled(true);
            System.gc();
          } //of if
        } else if(cmd.equals(menu_text[3])){
          JFileChooser dialog = new JFileChooser();
          JFileFilter filter = new JFileFilter();
          filter.addExtension("xml");
          filter.setDescription("*.xml");
          dialog.setFileFilter(filter);
          dialog.setDialogType(JFileChooser.OPEN_DIALOG);
          if(dialog.showDialog(topology_viewer, "Load positions") == JFileChooser.APPROVE_OPTION){
            String tmp_loc_path = dialog.getSelectedFile().getAbsolutePath();
            //sld_rot.setValue(0);
            //sld_zoom.setValue(0);
            if(tmp_loc_path != null){
              new PositionParser(tg_panel, tmp_loc_path);
              System.gc();
            } //of if         
          } //of if
        } else if(cmd.equals(menu_text[4])){
          JFileChooser dialog = new JFileChooser();
          JFileFilter filter = new JFileFilter();
          //#filter.addExtension("gif");
          //#filter.addExtension("jpg");
          //#filter.addExtension("png");
          filter.addExtension("svg");
          //#filter.setDescription("*.gif, *.jpg, *.png, *.svg");
          filter.setDescription("*.svg");
          dialog.setFileFilter(filter);
          dialog.setDialogType(JFileChooser.OPEN_DIALOG);
          if(dialog.showDialog(topology_viewer, "Load map") == JFileChooser.APPROVE_OPTION){
            map_path = dialog.getSelectedFile().getAbsolutePath();
            sld_rot.setValue(0);
            sld_zoom.setValue(0);
            tg_panel.loadMap(map_path);
          } //of if          
        } else if(cmd.equals(menu_text[5])){
          String[] supExtensions = GraphicExporter.getSupportedTypes();
          JFileChooser dialog = new JFileChooser();
          for(int i = 0; i < supExtensions.length; i++){
            JFileFilter filter = new JFileFilter();           
            filter.addExtension(supExtensions[i]);
            filter.setDescription("*."+supExtensions[i]);
            dialog.addChoosableFileFilter(filter);
          } //of for
          dialog.setDialogType(JFileChooser.SAVE_DIALOG);
          if(dialog.showDialog(topology_viewer, "Export graphic") == JFileChooser.APPROVE_OPTION){
            String path = dialog.getSelectedFile().getAbsolutePath();
            boolean foundFilter = false;
            for(int i = 0; i < supExtensions.length; i++)
              if(dialog.getFileFilter().getDescription().equals("*." + supExtensions[i])){
                if(!path.toLowerCase().endsWith("." + supExtensions[i]))path += ("." + supExtensions[i]);
                foundFilter = true;
                break;
              } //of if
            if(!foundFilter)path += ".jpg";
            GraphicExporter exp = new GraphicExporter(tg_panel);
            try{
              try{
                if(path.toLowerCase().endsWith(".bmp"))exp.createBMPFile(path);
                else if(path.toLowerCase().endsWith(".gif"))exp.createGIFFile(path);
                //else if(path.toLowerCase().endsWith(".jpg"))exp.createJPGFile(path, 100);
                else if(path.toLowerCase().endsWith(".png"))exp.createPNGFile(path);
                else if(path.toLowerCase().endsWith(".svg"))exp.createSVGFile(path);
              } catch(java.io.IOException eio){
                System.out.println("Couldn't create graphic file.");
              } //of try-catch
            } catch(Throwable th){
              System.out.println("Internal error during create graphic file.");
            } //of try-catch
          } //of if
        } else if(cmd.equals(menu_text[6])){
          dispose();
          System.exit(0);
        } else if(cmd.equals(menu_text[7])){
          tg_panel.setMetricVisible(!tg_panel.isMetricVisible());
        } else if(cmd.equals(menu_text[8])){
          ctrl.setVisible(!ctrl.isVisible());
        } else if(cmd.equals(menu_text[9])){
          class InfoDialog extends JDialog implements ActionListener{
            private JButton button;
            
            public InfoDialog(JFrame owner){
              super(owner, "About");
              
              String info[] = {"IVS Otto-von-Guericke University Magdeburg, Germany",
                               "",
                               "Based on TouchGraph libary from Alexander Shapiro",
                               "",
                               "Modified by Christian Fackroth",
                               "E-Mail: cfackrot@cs.uni-magdeburg.de"};
              
              setModal(true);
              setSize(400, 300);
              setLocation((owner.getX() + (owner.getX() + owner.getWidth() - getWidth()) >> 1),
                          (owner.getY() + (owner.getY() + owner.getHeight() - getHeight()) >> 1));
              setResizable(false);
              Container dialog = getContentPane();
              dialog.setLayout(null);
              dialog.setBackground(BACKGROUND);
              dialog.setForeground(FOREGROUND);

              JLabel label = new JLabel("Topology-Viewer " + VERSION, JLabel.CENTER);
              label.setFont(new Font("Arial", Font.BOLD, 14));
              label.setBackground(BACKGROUND);
              label.setForeground(FOREGROUND);
              label.setBounds(20, 30, 360, 15);
              dialog.add(label);
              for(int i=0; i<info.length; i++){
                label = new JLabel(info[i], JLabel.LEFT);
                label.setFont(STD_FONT);
                label.setBackground(BACKGROUND);
                label.setForeground(FOREGROUND);
                label.setBounds(20, 80 + (i << 4), 360, 15);
                dialog.add(label);
              } //of for

              button = new JButton("Exit");
              button.setFont(STD_FONT);
              button.setBackground(BACKGROUND);
              button.setForeground(FOREGROUND);
              button.setBounds(150, 230, 100, 30);
              button.setMnemonic('E');
              button.addActionListener(this);
              dialog.add(button);
             
              addWindowListener(new WindowAdapter(){
                public void windowClosing(WindowEvent e){
                  dispose();
                } //of windowClosing
              }); //of WindowAdapter
              setVisible(true);
            } //of InfoDialog
         
            public void actionPerformed(ActionEvent e){
              if(e.getSource() == button)dispose();
            } //of actionPerformed
          }; //of class InfoWindow
          new InfoDialog(topology_viewer);
        } //of if
      } //of actionPerformed
    }; //of ActionListener
    
    //--construct the menu--//  
    JMenuBar menubar = new JMenuBar();
    menubar.setBackground(BACKGROUND);
    menubar.setForeground(FOREGROUND);
    menubar.setOpaque(true);
    
    JMenu menu = new JMenu("File");
    menu.setFont(STD_FONT);
    menu.setMnemonic('F');
    menu.setBackground(BACKGROUND);
    menu.setForeground(FOREGROUND);
    menu.setOpaque(true);
    
    JMenuItem item = new JMenuItem(menu_text[0]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('S', Event.CTRL_MASK));
    menu.add(item);

    item = new JMenuItem(menu_text[1]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('L', Event.CTRL_MASK));
    menu.add(item);

    item = new JMenuItem(menu_text[2]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('H', Event.CTRL_MASK));
    menu.add(item);

    JSeparator separ = new JSeparator();
    separ.setOpaque(true);
    menu.add(separ);

    item = new JMenuItem(menu_text[3]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('P', Event.CTRL_MASK));
    menu.add(item);

    separ = new JSeparator();
    separ.setOpaque(true);
    menu.add(separ);

    map_item = new JMenuItem(menu_text[4]);
    map_item.setFont(STD_FONT);
    map_item.setEnabled(false);
    map_item.setBackground(BACKGROUND);
    map_item.setForeground(FOREGROUND);
    map_item.setOpaque(true);
    map_item.addActionListener(menuAction);
    map_item.setAccelerator(KeyStroke.getKeyStroke('M', Event.CTRL_MASK));
    menu.add(map_item);
    
    separ = new JSeparator();
    separ.setOpaque(true);
    menu.add(separ);

    exp_item = new JMenuItem(menu_text[5]);
    exp_item.setFont(STD_FONT);
    exp_item.setEnabled(false);
    exp_item.setBackground(BACKGROUND);
    exp_item.setForeground(FOREGROUND);
    exp_item.setOpaque(true);
    exp_item.addActionListener(menuAction);
    exp_item.setAccelerator(KeyStroke.getKeyStroke('E', Event.CTRL_MASK));
    menu.add(exp_item);    

    separ = new JSeparator();
    separ.setOpaque(true);
    menu.add(separ);

    item = new JMenuItem(menu_text[6]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    //item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F4, Event.ALT_MASK));
    item.setAccelerator(KeyStroke.getKeyStroke('Q', Event.CTRL_MASK));
    menu.add(item);

    menubar.add(menu);    

    menu = new JMenu("View");
    menu.setFont(STD_FONT);
    menu.setMnemonic('V');
    menu.setBackground(BACKGROUND);
    menu.setForeground(FOREGROUND);
    menu.setOpaque(true);

    item = new JMenuItem(menu_text[7]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('V', Event.CTRL_MASK));
    menu.add(item);

    item = new JMenuItem(menu_text[8]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('C', Event.CTRL_MASK));
    menu.add(item);

    menubar.add(menu);

    menu = new JMenu("Info");
    menu.setFont(STD_FONT);
    menu.setMnemonic('I');
    menu.setBackground(BACKGROUND);
    menu.setForeground(FOREGROUND);
    menu.setOpaque(true);

    item = new JMenuItem(menu_text[9]);
    item.setFont(STD_FONT);
    item.setBackground(BACKGROUND);
    item.setForeground(FOREGROUND);
    item.setOpaque(true);
    item.addActionListener(menuAction);
    item.setAccelerator(KeyStroke.getKeyStroke('A', Event.CTRL_MASK));
    menu.add(item);
    
    menubar.add(menu);
    
    return menubar;
  } //of buildMenu

  //--import from GLPanel--//
  public void addUIs(){
    TGUIManager tgUIManager = new TGUIManager();
    GLEditUI editUI = new GLEditUI(/*this*/tg_panel);
    GLNavigateUI navigateUI = new GLNavigateUI(this, tg_panel);
    tgUIManager.addUI(editUI, "Edit");
    tgUIManager.addUI(navigateUI, "Navigate");
    tgUIManager.activate("Navigate");
  } //of addUIs
  //--end import from GLPanel--//

  public static void main(String args[]){
    int offset = 0;
    port = 8333;
    host = "localhost";
    console = false;
    reconnect = false;
    loc_path = null;
    map_path = null;
    if(args.length > 0){
      if(args[offset++].toLowerCase().equals("-net")){
        if((args.length <= 5)&&(args.length >= 3)){
          console = true;
          if(args[offset].toLowerCase().equals("-reconnect")){
            reconnect = true;
            offset++;
      } //of if
          host = args[offset++];
          try{
            port = (new Integer(args[offset++])).intValue();
          } catch(NumberFormatException ex){
            ex.printStackTrace();
          } //of try-catch
          if(offset < args.length)map_path = args[offset];
        } else System.out.println("TopologyViewer <\"-net\"> [\"-reconnect\"] <host> <port> [map]");
      } else if(args[0].toLowerCase().equals("-loc")){
        if((args.length == 2)||(args.length == 3)){
          console = true;
          loc_path = args[1];
          if(args.length == 3)map_path = args[2];
        } else System.out.println("TopologyViewer <\"-loc\"> <path> [map]");
      } else {
        System.out.println("Valid commands:");
        System.out.println("TopologyViewer <\"-loc\"> <path> [map]");
        System.out.println("TopologyViewer <\"-net\"> [\"-reconnect\"] <host> <port> [map]");
      } //of if-else
    } //of if
    //else{
    //  loc_path = "a.xml";
    //  map_path = "plan.svg";
    //}
    topology_viewer = new TopologyViewer();
  } //of main
} //of class TopologyViewer
