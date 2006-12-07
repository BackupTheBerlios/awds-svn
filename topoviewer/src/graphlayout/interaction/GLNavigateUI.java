package graphlayout.interaction;

import graphlayout.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;

public class GLNavigateUI extends TGUserInterface{
  TGPanel tg_panel;
  TopologyViewer viewer;

  GLNavigateMouseListener ml;

  TGAbstractDragUI hvDragUI;
  TGAbstractDragUI rotateDragUI;

  DragNodeUI dragNodeUI;
  LocalityScroll locality_scroll;
  JPopupMenu nodePopup;
  Node popupNode;
  Edge popupEdge;

  public GLNavigateUI(TopologyViewer viewer, TGPanel tg_panel){
    this.viewer = viewer;
    this.tg_panel = tg_panel;

    locality_scroll = viewer.locality_scroll;
    hvDragUI = viewer.hv_scroll.getHVDragUI();
    dragNodeUI = new DragNodeUI(tg_panel);

    ml = new GLNavigateMouseListener();
    setUpNodePopup(viewer);
  } //of GLNavigateUI

  public void activate(){
    tg_panel.addMouseListener(ml);
  } //of activate

  public void deactivate(){
    tg_panel.removeMouseListener(ml);
  } //of deactivate

  class GLNavigateMouseListener extends MouseAdapter{
    public void mousePressed(MouseEvent e){
      Node mouseOverN = tg_panel.getMouseOverN();
      if(e.getModifiers() == MouseEvent.BUTTON1_MASK){
        if(mouseOverN == null)hvDragUI.activate(e);
        else dragNodeUI.activate(e);
      } //of if
    } //of mousePressed

    public void mouseClicked(MouseEvent e){
    } //of mouseClicked

    public void mouseReleased(MouseEvent e){
      if(e.getButton() == MouseEvent.BUTTON3){
        popupNode = tg_panel.getMouseOverN();
        if(popupNode != null){
          tg_panel.setMaintainMouseOver(true);
					nodePopup.show(tg_panel, e.getX(), e.getY());
        } //of if
      }	else tg_panel.setMaintainMouseOver(false);
    } //of mouseReleased
  } //of class GLNavigateMouseListener

  private void setUpNodePopup(final TopologyViewer viewer){
    nodePopup = new JPopupMenu();
    nodePopup.setFont(TopologyViewer.STD_FONT);
    nodePopup.setBackground(TopologyViewer.BACKGROUND);
    nodePopup.setForeground(TopologyViewer.FOREGROUND);
    nodePopup.setOpaque(true);
    //viewer.getContentPane().add(nodePopup);
    tg_panel.add(nodePopup);
    JMenuItem item = new JMenuItem("Center Node");
    item.setFont(TopologyViewer.STD_FONT);
    item.setBackground(TopologyViewer.BACKGROUND);
    item.setForeground(TopologyViewer.FOREGROUND);
    item.setOpaque(true);
    item.setAccelerator(KeyStroke.getKeyStroke('C', Event.ALT_MASK));
    ActionListener centerAction = new ActionListener(){
      public void actionPerformed(ActionEvent e){
        if(popupNode != null)
          viewer.hv_scroll.slowScrollToCenter(popupNode);
          tg_panel.setMaintainMouseOver(false);
          tg_panel.setMouseOverN(null);
          tg_panel.repaint();
       } //of actionPerformed
     }; //of ActionListener
     item.addActionListener(centerAction);
     nodePopup.add(item);

     item = new JMenuItem("Fixed Node");
     item.setFont(TopologyViewer.STD_FONT);
     item.setBackground(TopologyViewer.BACKGROUND);
     item.setForeground(TopologyViewer.FOREGROUND);
     item.setOpaque(true);
     item.setAccelerator(KeyStroke.getKeyStroke('F', Event.ALT_MASK));
     ActionListener fixedAction = new ActionListener(){
       public void actionPerformed(ActionEvent e){
         if(popupNode != null)popupNode.setFixed(!popupNode.isFixed());
         tg_panel.setMaintainMouseOver(false);
         tg_panel.setMouseOverN(null);
         tg_panel.repaint();
       } //of actionPerformed
     }; //of ActionListener
     item.addActionListener(fixedAction);
     nodePopup.add(item);        
   } //of setUpNodePopup
} //of class GLNavigateUI
