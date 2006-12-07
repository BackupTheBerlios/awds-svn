package graphlayout;

import java.awt.geom.*;
import org.apache.batik.swing.*;

public class JSVGCanvasRenderer extends Thread{
  private long time;             //time counter to perform a redraw
  private long millis;           //help variable to calculate the real time
  private boolean thread_wait;   //true, if the thread wait for new render perform
  private boolean thread_exist;  //true if the thread exists, otherwise false
  private JSVGCanvas svg_canvas; //the JSVGCanvas for the renderer
  
  private AffineTransform aff_trans;

  public JSVGCanvasRenderer(final JSVGCanvas svg_canvas){
    thread_wait = true;
    thread_exist = true;
    this.svg_canvas = svg_canvas;
    start();
  } //of JSVGRenderer

  public synchronized void startWaiting(long time, AffineTransform aff_trans){
    this.time = time;
    this.aff_trans = aff_trans;
    millis = System.currentTimeMillis();
    thread_wait = false;
    notify();
  } //of startWaiting

  public synchronized void stopWaiting(){
    thread_wait = true;
    notify();
  } //of stopWaiting
  
  public synchronized void destroy(){
    thread_wait = true;
    thread_exist = false;
    notify();
  } //of destroy
    
  public synchronized void run(){
    while(thread_exist){
      if(thread_wait){
        try{         
          wait();
        } catch(InterruptedException ex){
          ex.printStackTrace();
        } //of try-catch
      } else {
        try{
          wait(100);
        } catch(InterruptedException ex){
          ex.printStackTrace();
          return;
        } //of try-catch
        time -= (System.currentTimeMillis() - millis);
        millis = System.currentTimeMillis();
        if(time < 0){
          thread_wait = true;
          svg_canvas.setRenderingTransform(aff_trans, true);
        } //of if
      } //of if-else
    } //of while
  } //of run
} //of class JSVGCanvasRenderer
