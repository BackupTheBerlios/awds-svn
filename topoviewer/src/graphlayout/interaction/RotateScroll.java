package graphlayout.interaction;

import graphlayout.*;

import javax.swing.*;

public class RotateScroll implements GraphListener{
  private double alpha;
  private JSlider rotatebar;
  private TGPanel tg_panel;
  private RotateLens rotate_lens;

  public RotateScroll(TGPanel tg_panel, JSlider rotatebar, int alpha){
    rotate_lens=new RotateLens();
    this.alpha = alpha * Math.PI / 180.0;
    this.rotatebar = rotatebar;
    this.tg_panel = tg_panel;
    this.tg_panel.addGraphListener(this);
  } //of RotateScroll

  public RotateLens getLens(){
    return rotate_lens;
  } //of getLens

  public void setRotateValue(int alpha){
    //double delta_alpha = alpha * Math.PI / 180.0 - this.alpha;
    this.alpha = alpha * Math.PI / 180.0;
    tg_panel.repaintAfterMove();
    tg_panel.repaintAfterRotateBackground(this.alpha);
  } //of setRotateValue

  public int getRotateValue(){
    return(int)(alpha * 180.0 / Math.PI + 0.5);
  } //of getRotateValue

  public void graphMoved(){} //from GraphListener interface
  public void graphReset(){  //from GraphListener interface
    alpha = 0;
    rotatebar.setValue(0);
  } //of graphReset

  private double computeAngle(double x, double y){
    double angle = Math.atan(y / x);

    if(x == 0.0) //There is probably a better way of hangling boundary conditions, but whatever
      if(y > 0.0)angle = Math.PI * 0.5;
      else angle =- Math.PI * 0.5;
    if(x < 0.0)angle += Math.PI;

    return angle;
  } //of computeAngle

  class RotateLens extends TGAbstractLens{
    protected void applyLens(TGPoint2D p){
      double current_angle = computeAngle(p.x, p.y);
      double dist = Math.sqrt((p.x * p.x) + (p.y * p.y));
      p.x = dist * Math.cos(current_angle + alpha);
      p.y = dist * Math.sin(current_angle + alpha);
    } //of applyLens

    protected void undoLens(TGPoint2D p){
      double current_angle = computeAngle(p.x, p.y);
      double dist = Math.sqrt((p.x * p.x) + (p.y * p.y));
      p.x = dist * Math.cos(current_angle - alpha);
      p.y = dist * Math.sin(current_angle - alpha);
    } //of undoLens
  } //of class RotateLens
} //of class RotateScroll
