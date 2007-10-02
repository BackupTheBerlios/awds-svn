package graphlayout;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;

public class JGroupBox extends JPanel{
  private EtchedBorder etched;
  private TitledBorder titled;

  public JGroupBox(LayoutManager layout, boolean isDoubleBuffered){
    super(layout, isDoubleBuffered);
    etched = new EtchedBorder(EtchedBorder.LOWERED, Color.WHITE, Color.DARK_GRAY);
    titled = new TitledBorder(etched, null, TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, getFont());
    setBorder(titled);
  } //of JGroupBox

  public JGroupBox(LayoutManager layout){
    super(layout);
    etched = new EtchedBorder(EtchedBorder.LOWERED, Color.WHITE, Color.DARK_GRAY);
    titled = new TitledBorder(etched, null, TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, getFont());
    setBorder(titled);
  } //of JGroupBox

  public JGroupBox(boolean isDoubleBuffered){
    super(isDoubleBuffered);
    etched = new EtchedBorder(EtchedBorder.LOWERED, Color.WHITE, Color.DARK_GRAY);
    titled = new TitledBorder(etched, null, TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, getFont());
    setBorder(titled);
  } //of JGroupBox

  public JGroupBox(){
    super();
    etched = new EtchedBorder(EtchedBorder.LOWERED, Color.WHITE, Color.DARK_GRAY);
    titled = new TitledBorder(etched, null, TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, getFont());
    setBorder(titled);
  } //of JGroupBox

  public void setHighlightColor(Color c){
    etched = new EtchedBorder(EtchedBorder.LOWERED, c, etched.getShadowColor());
    titled.setBorder(etched);
  } //of setHighlightColor

  public void setShadowColor(Color c){
    etched = new EtchedBorder(EtchedBorder.LOWERED, etched.getHighlightColor(), c);
    titled.setBorder(etched);
  } //of setShadowColor

  public void setText(String text){
    titled.setTitle(text);
  } //of setText
} //of class JGroupBox
