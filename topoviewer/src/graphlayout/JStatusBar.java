package graphlayout;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;

public class JStatusBar extends JPanel{
  private JStatusBarField[] fields;

  public JStatusBar(LayoutManager layout, boolean isDoubleBuffered){
    super(layout, isDoubleBuffered);
    fields = new JStatusBarField[1];

    //--simple layout manager--/
    setLayout(new GridLayout(1, 1));

    fields[0] = new JStatusBarField();
    add(fields[0]);
  } //of JStatusBar

  public JStatusBar(LayoutManager layout){
    super(layout);
    fields = new JStatusBarField[1];

    //--simple layout manager--/
    setLayout(new GridLayout(1, 1));

    fields[0] = new JStatusBarField();
    add(fields[0]);
  } //of JStatusBar

  public JStatusBar(boolean isDoubleBuffered){
    super(isDoubleBuffered);
    fields = new JStatusBarField[1];

    //--simple layout manager--/
    setLayout(new GridLayout(1, 1));

    fields[0] = new JStatusBarField();
    add(fields[0]);
  } //of JStatusBar 

  public JStatusBar(){
    super();
    fields = new JStatusBarField[1];

    //--simple layout manager--/
    setLayout(new GridLayout(1, 1));

    fields[0] = new JStatusBarField();
    add(fields[0]);
  } //of JStatusBar

  public JStatusBar(int numFields){
    super();
    fields = new JStatusBarField[numFields];

    //--simple layout manager--/
    setLayout(new GridLayout(1, numFields));

    for(int i = 0; i < numFields; i++){
      fields[i] = new JStatusBarField();
      add(fields[i]);
    } //of for
  } //of JStatusBar

  public void setFieldNum(int numFields){
    fields = new JStatusBarField[numFields];

    for(int i = 0; i < numFields; i++){
      fields[i] = new JStatusBarField();
      add(fields[i]);
    } //of for
  } //of setFieldNum

  public int getFieldNum(){
    return fields.length;
  } //of getFieldNum

  public void setText(int num, String text){
    fields[num].setText(" " + text);
  } //of setText

  public String getText(int num){
    return fields[num].getText().substring(1);
  } //of getText

  class JStatusBarField extends JLabel{
    public JStatusBarField(){
      super();
      setBorder(new BevelBorder(BevelBorder.LOWERED));
      setPreferredSize(new Dimension(50, 20));
      setFont(new Font("Dialog", Font.PLAIN, 12));
    } //of JStatusBarField
  } //of JStatusBarField
} //of class JStatusBar
