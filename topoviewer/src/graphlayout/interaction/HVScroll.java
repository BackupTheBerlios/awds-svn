package graphlayout.interaction;

import graphlayout.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class HVScroll implements GraphListener{
  private DScrollbar horizontalSB;
  private DScrollbar verticalSB;

  HVLens hvLens;
  HVDragUI hvDragUI;
  HVScrollToCenterUI hvScrollToCenterUI;
  public boolean scrolling;

  private boolean adjustment_internal;
  private TGPanel tg_panel;
  private TGLensSet tg_lens_set;

  TGPoint2D offset;

  public HVScroll(TGPanel tg_panel, TGLensSet tg_lens_set){
    this.tg_panel = tg_panel;
    this.tg_lens_set = tg_lens_set;

    offset = new TGPoint2D(0, 0);
    scrolling = false;
    adjustment_internal = false;

    horizontalSB = new DScrollbar(JScrollBar.HORIZONTAL, 0, 100, -1000, 1100);
    horizontalSB.setBlockIncrement(100);
    horizontalSB.setUnitIncrement(20);
    horizontalSB.addAdjustmentListener(new horizAdjustmentListener());

    verticalSB = new DScrollbar(JScrollBar.VERTICAL, 0, 100, -1000, 1100);
    verticalSB.setBlockIncrement(100);
    verticalSB.setUnitIncrement(20);
    verticalSB.addAdjustmentListener(new vertAdjustmentListener());

    hvLens = new HVLens();
    hvDragUI = new HVDragUI(); //Hopefully this approach won't eat too much memory
    hvScrollToCenterUI = new HVScrollToCenterUI();

    tg_panel.addGraphListener(this);
  } //of HVScroll

  public JScrollBar getHorizontalSB(){
    return horizontalSB;
  } //og getHorizontalSB

  public JScrollBar getVerticalSB(){
    return verticalSB;
  } //of getVerticalSB

  public HVDragUI getHVDragUI(){
    return hvDragUI;
  } //of getHVDragUI

  public HVLens getLens(){
    return hvLens;
  } //of getLens

  public TGAbstractClickUI getHVScrollToCenterUI(){
    return hvScrollToCenterUI;
  } //of getHVScrollToCenterUI

  public TGPoint2D getTopLeftDraw(){
    TGPoint2D tld = tg_panel.getTopLeftDraw();
    tld.setLocation(tld.x - tg_panel.getSize().width / 4, tld.y- tg_panel.getSize().height / 4);
    return tld;
  } //of getTopLeftDraw

  public TGPoint2D getBottomRightDraw(){
    TGPoint2D brd = tg_panel.getBottomRightDraw();
    brd.setLocation(brd.x + tg_panel.getSize().width / 4, brd.y + tg_panel.getSize().height / 4);
    return brd;
  } //of getTopLeftDraw

  public TGPoint2D getDrawCenter(){ //Should probably be called from tg_panel
    return new TGPoint2D(tg_panel.getSize().width / 2, tg_panel.getSize().height / 2);
  } //of getDrawCenter

	Thread noRepaintThread;
  public void graphMoved(){ //From GraphListener interface
    if((tg_panel.getDragNode() == null)&&(tg_panel.getSize().height > 0)){
      TGPoint2D drawCenter = getDrawCenter();

      TGPoint2D tld = getTopLeftDraw();
      TGPoint2D brd = getBottomRightDraw();

      double newH = (-(tld.x - drawCenter.x) / (brd.x - tld.x) * 2000 - 1000);
      double newV = (-(tld.y - drawCenter.y) / (brd.y - tld.y) * 2000 - 1000);

      boolean beyondBorder = true;
      if((newH < horizontalSB.getMaximum())&&(newH > horizontalSB.getMinimum())&&
         (newV < verticalSB.getMaximum())&&(newV > verticalSB.getMinimum()))beyondBorder = false;

      adjustment_internal = true;
      horizontalSB.setDValue(newH);
      verticalSB.setDValue(newV);
      adjustment_internal = false;

      if(beyondBorder){
        adjustHOffset();
        adjustVOffset();
        tg_panel.repaint();
      } //of if
    } //of if
    if((noRepaintThread != null)&&(noRepaintThread.isAlive()))noRepaintThread.interrupt();
		noRepaintThread = new Thread(){
      public void run(){
        try{
          Thread.currentThread().sleep(40); //Wait 40 milliseconds before repainting
        } catch(InterruptedException ex){} //of try-catch
      } //of run
    }; //of new Thread
    noRepaintThread.start();
  } //of graphMoved

  public void graphReset(){ //From GraphListener interface
    horizontalSB.setDValue(0);
    verticalSB.setDValue(0);

    adjustHOffset();
    adjustVOffset();
  } //of graphReset

  class DScrollbar extends JScrollBar{
    private double doubleValue;

    DScrollbar(int orient, int val, int vis, int min, int max){
      super(orient, val, vis, min, max);
      doubleValue = val;
    } //of DScrollbar

    public void setValue(int v){
      doubleValue = v;
      super.setValue(v);
    } //of setValue

    public void setIValue(int v){
      super.setValue(v);
    } //of setIValue

    public void setDValue(double v){
      doubleValue = Math.max(getMinimum(), Math.min(getMaximum(), v));
      setIValue((int)v);
    } //of setDValue

    public double getDValue(){
      return doubleValue;
    } //of getDValue
  } //of class DScrollbar

  private void adjustHOffset(){ //the inverse of the "graphMoved" function.
    TGPoint2D offsetOld = new TGPoint2D();
    for(int iterate = 0; iterate < 3; iterate++){ //iteration needed to yeild cerrect results depite warping lenses
      TGPoint2D center= tg_panel.getCenter();
      TGPoint2D tld = getTopLeftDraw();
      TGPoint2D brd = getBottomRightDraw();

      double newx = ((horizontalSB.getDValue() + 1000.0) / 2000) * (brd.x - tld.x) + tld.x;
      double newy = tg_panel.getSize().height * 0.5;
      TGPoint2D newCenter = tg_lens_set.convDrawToReal(newx, newy);

      offsetOld.setX(offset.x);
      offsetOld.setY(offset.y);
      offset.setX(offset.x + (newCenter.x - center.x));
      //offset.setY(offset.y + (newCenter.y - center.y));

      tg_panel.processGraphMove();
      tg_panel.repaintAfterMoveBackground(offsetOld.x - offset.x, offsetOld.y - offset.y);
    } //of for
    //TGPoint2D drawFrom = tg_lens_set.convRealToDraw(offset.x, offset.y);
    //TGPoint2D drawTo = tg_lens_set.convRealToDraw(offsetOld.x, offsetOld.y);
    //tg_panel.repaintAfterMoveBackground(drawTo.x - drawFrom.x, drawTo.y - drawFrom.y);
  } //of adjustHOffset

  private void adjustVOffset(){ //the inverse of the "graphMoved" function.
    TGPoint2D offsetOld = new TGPoint2D();
    for(int iterate = 0; iterate < 10; iterate++){ //iteration needed to yeild cerrect results depite warping lenses
      TGPoint2D center= tg_panel.getCenter();
      TGPoint2D tld = getTopLeftDraw();
      TGPoint2D brd = getBottomRightDraw();

      double newx = tg_panel.getSize().width * 0.5;
      double newy = ((verticalSB.getDValue() + 1000.0) / 2000) * (brd.y - tld.y) + tld.y;
      TGPoint2D newCenter = tg_lens_set.convDrawToReal(newx, newy);

      offsetOld.setX(offset.x);
      offsetOld.setY(offset.y);
      //offset.setX(offset.x + (newCenter.x - center.x));
      offset.setY(offset.y + (newCenter.y - center.y));
      
      tg_panel.processGraphMove();
      tg_panel.repaintAfterMoveBackground(offsetOld.x - offset.x, offsetOld.y - offset.y);
    } //of for
    //TGPoint2D drawFrom = tg_lens_set.convRealToDraw(offset.x, offset.y);
    //TGPoint2D drawTo = tg_lens_set.convRealToDraw(offsetOld.x, offsetOld.y);
    //tg_panel.repaintAfterMoveBackground(drawTo.x - drawFrom.x, drawTo.y - drawFrom.y);
  } //of adjustVOffset

  private class horizAdjustmentListener implements AdjustmentListener{
    public void adjustmentValueChanged(AdjustmentEvent e){
      if(!adjustment_internal){
        adjustHOffset();
        tg_panel.repaintAfterMove();
      } //of if
    } //of adjustmentValueChanged
  } //of AdjustmentListener

  private class vertAdjustmentListener implements AdjustmentListener{
    public void adjustmentValueChanged(AdjustmentEvent e){
      if(!adjustment_internal){
        adjustVOffset();
        tg_panel.repaintAfterMove();
      } //of if
    } //of adjustmentValueChanged
  } //of AdjustmentListener

  class HVLens extends TGAbstractLens{
    protected void applyLens(TGPoint2D p){
      p.x -= offset.x;
      p.y -= offset.y;
    } //of applyLens

    protected void undoLens(TGPoint2D p){
      p.x += offset.x;
      p.y += offset.y;
    } //of undoLens
  } //of class HVLens

  public void setOffset(Point p){
    offset.setLocation(p.x, p.y);
    tg_panel.processGraphMove(); //Adjust draw coordinates to include new offset
    graphMoved(); //adjusts scrollbars to fit draw coordinates
  } //of setOffset

  public Point getOffset(){
    return new Point((int)offset.x, (int)offset.y);
  } //of getOffset

  public void scrollAtoB(TGPoint2D drawFrom, TGPoint2D drawTo){
    TGPoint2D offsetOld = new TGPoint2D(offset.x, offset.y);
    TGPoint2D from = tg_lens_set.convDrawToReal(drawFrom);
    TGPoint2D to = tg_lens_set.convDrawToReal(drawTo);
    offset.setX(offset.x + (from.x - to.x));
    offset.setY(offset.y + (from.y - to.y));
    //tg_panel.repaintAfterMoveBackground(drawTo.x - drawFrom.x, drawTo.y - drawFrom.y);
    tg_panel.repaintAfterMoveBackground(offsetOld.x - offset.x, offsetOld.y - offset.y);
  } //of scrollAtoB

     Thread scrollThread;

     public void slowScrollToCenter(final Node n) {
         scrolling = true;
         if (scrollThread!=null && scrollThread.isAlive()) scrollThread.interrupt();
         scrollThread = new Thread() {
             public void run() {
                double nx=-999;
                double ny=-999;
                double cx;
                double cy;
                double distFromCenter;
                boolean keepScrolling = true;
                int scrollSteps=0;

                while(keepScrolling && scrollSteps++<250) {
                	nx= n.drawx;
                    ny= n.drawy;
                    cx= getDrawCenter().x;
                    cy= getDrawCenter().y;

                	distFromCenter = Math.sqrt((nx-cx)*(nx-cx)+(ny-cy)*(ny-cy));

					double newx, newy;
					if(distFromCenter>5) {
                    	newx = cx + (nx-cx)*((distFromCenter-5)/distFromCenter);
						newy = cy + (ny-cy)*((distFromCenter-5)/distFromCenter);
                	}
                	else {
                		newx = cx;
						newy = cy;
                	}

                    scrollAtoB(new TGPoint2D(nx,ny), new TGPoint2D(newx,newy));

                    if (noRepaintThread==null || !noRepaintThread.isAlive()) {
    	               	tg_panel.repaintAfterMove(); //only repaint if 40 milliseconds have not ellapsed since last repaint
                	}
                    else {
                    	tg_panel.processGraphMove(); //otherwise, register scroll internally
                	}

                    try {
                           Thread.currentThread().sleep(20);
                    } catch (InterruptedException ex) { keepScrolling=false; }

                    if(distFromCenter<3) {
	                    try {
    	                       Thread.currentThread().sleep(200); //Wait a little to make sure
        	            } catch (InterruptedException ex) { keepScrolling=false; }
        	            nx= n.drawx;
                    	ny= n.drawy;
                    	cx= getDrawCenter().x;
                    	cy= getDrawCenter().y;
                		distFromCenter = Math.sqrt((nx-cx)*(nx-cx)+(ny-cy)*(ny-cy));
            			if (distFromCenter<3) keepScrolling=false;

                    }
                }
                scrollAtoB(new TGPoint2D(n.drawx,n.drawy),getDrawCenter()); //for good measure
                tg_panel.repaintAfterMove();
                HVScroll.this.scrolling = false;
            }
        };
        scrollThread.start();
     }

    class HVScrollToCenterUI extends TGAbstractClickUI {
        public void mouseClicked(MouseEvent e) {
/*            Node mouseOverN=tg_panel.getMouseOverN();
            if(!scrolling && mouseOverN!=null)
                slowScrollToCenter(mouseOverN);
*/        }
    }

    class HVDragUI extends TGAbstractDragUI{
        TGPoint2D lastMousePos;
        HVDragUI() { super(HVScroll.this.tg_panel); }

        public void preActivate() {}
        public void preDeactivate() {}

        public void mousePressed(MouseEvent e) {
            lastMousePos = new TGPoint2D(e.getX(), e.getY());
        }
        public void mouseReleased(MouseEvent e) {}
        public void mouseDragged(MouseEvent e) {
            if(!scrolling)scrollAtoB(lastMousePos, new TGPoint2D(e.getX(), e.getY()));
            lastMousePos.setLocation(e.getX(),e.getY());
            this.tgPanel.repaintAfterMove();
        }
    }

} //of class HVScroll
