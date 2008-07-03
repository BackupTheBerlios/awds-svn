package graphlayout.interaction;

import graphlayout.*;

import javax.swing.*;

public class ZoomScroll implements GraphListener{
  private double scale;
  private JSlider zoombar;
  private TGPanel tg_panel;
  private ZoomLens zoomlens;

  public ZoomScroll(TGPanel tg_panel, JSlider zoombar, double scale){
    zoomlens = new ZoomLens();
    this.scale = scale;
    this.zoombar = zoombar;
    this.tg_panel = tg_panel;
    this.tg_panel.addGraphListener(this);
  } //of ZoomScroll

  public ZoomLens getLens(){
    return zoomlens;
  } //of getLens

  public void graphMoved(){} //from GraphListener interface
  public void graphReset(){  //from GraphListener interface
    zoombar.setValue(0);
  } //of graphReset

  public void setZoomValue(double scale){
    //double delta_scale = scale / this.scale;
    //double offset_scale = 0.5 * (this.scale - scale);
    this.scale = scale;
    tg_panel.repaintAfterMove();
    tg_panel.repaintAfterZoomBackground(this.scale);
  } //of setZoomValue
  
  public double getZoomValue(){
    return scale;
  } //of getZoomValue

  class ZoomLens extends TGAbstractLens{
    protected void applyLens(TGPoint2D p){
      p.x *= scale;
      p.y *= scale;
    } //of applyLens

    protected void undoLens(TGPoint2D p){
      p.x /= scale;
      p.y /= scale;
    } //of undoLen
  } //of class ZoomLens
} //of class ZoomScroll
