/*
 * TouchGraph LLC. Apache-Style Software License
 *
 *
 * Copyright (c) 2001-2002 Alexander Shapiro. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by
 *        TouchGraph LLC (http://www.touchgraph.com/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "TouchGraph" or "TouchGraph LLC" must not be used to endorse
 *    or promote products derived from this software without prior written
 *    permission.  For written permission, please contact
 *    alex@touchgraph.com
 *
 * 5. Products derived from this software may not be called "TouchGraph",
 *    nor may "TouchGraph" appear in their name, without prior written
 *    permission of alex@touchgraph.com.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL TOUCHGRAPH OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 */

package graphlayout;

import graphlayout.interaction.*;
import graphlayout.graphelements.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

//--added by christian--//
import java.awt.geom.*;
import org.apache.batik.swing.*;
import org.apache.batik.swing.gvt.*;
import org.apache.batik.swing.svg.*;
//--end added by christian--//

/** TGPanel contains code for drawing the graph, and storing which nodes
  * are selected, and which ones the mouse is over.
  *
  * It houses methods to activate TGLayout, which performs dynamic layout.
  * Whenever the graph is moved, or repainted, TGPanel fires listner
  * methods on associated objects.
  *
  * <p><b>
  * Parts of this code build upon Sun's Graph Layout example.
  * http://java.sun.com/applets/jdk/1.1/demo/GraphLayout/Graph.java
  * </b></p>
  *
  * @author   Alexander Shapiro
  * @author   Murray Altheim  (2001-11-06; 2002-01-14 cleanup)
  * @version  1.22-jre1.1  $Id: TGPanel.java,v 1.2 2002/09/20 14:26:17 ldornbusch Exp $
  */
public class TGPanel extends JSVGCanvas{
  // static variables for use within the package
  public static Color BACK_COLOR = Color.WHITE;
  public static int RENDER_TIME = 500; //render time in ms

  // ....
  //--added by christian--//
  private int map_w, map_h;            //width, height of background image
  private int pixOneMeter;             //how much pixel are one meter
  private double alpha;                //angle of map
  private double scale;                //absolut scale
  private double trans_x, trans_y;     //translate of map
  private boolean loadMap;             //is map loaded?
  private boolean metricViewable;      //true, if the metric panel is visible, else false
  private JSVGCanvasRenderer renderer; //renderer for SVG-Graphic
  //--end of added by christian--//
  private GraphEltSet completeEltSet;
  private VisibleLocality visibleLocality;
  private LocalityUtils localityUtils;
  public TGLayout tgLayout;
  protected BasicMouseMotionListener basicMML;

  protected Edge mouseOverE;  //mouseOverE is the edge the mouse is over
  protected Node mouseOverN;  //mouseOverN is the node the mouse is over
  protected boolean maintainMouseOver = false; //If true, then don't change mouseOverN or mouseOverE

  protected Node select;

  Node dragNode; //Node currently being dragged

  protected Point mousePos; //Mouse location, updated in the mouseMotionListener

  private Vector graphListeners;
  private Vector paintListeners;
  TGLensSet tgLensSet;  // Converts between a nodes visual position (drawx, drawy),
                        // and its absolute position (x,y).
  AdjustOriginLens adjustOriginLens;
  SwitchSelectUI switchSelectUI;

  /** Default constructor. */
  public TGPanel(){
    setLayout(null);
    setDoubleBuffered(true);

    setGraphEltSet(new GraphEltSet());
    addMouseListener(new BasicMouseListener());
    basicMML = new BasicMouseMotionListener();
    addMouseMotionListener(basicMML);

    graphListeners=new Vector();
    paintListeners=new Vector();

    adjustOriginLens = new AdjustOriginLens();
    switchSelectUI = new SwitchSelectUI();

    TGLayout tgLayout = new TGLayout(this);
    setTGLayout(tgLayout);
    tgLayout.start();
    setGraphEltSet(new GraphEltSet());

    alpha = 0.0;
    scale = 1.0;
    loadMap = false;
    metricViewable = true;
    pixOneMeter = 20;
    renderer = null;

    addComponentListener(new ComponentAdapter(){
	  public void componentResized(ComponentEvent e){
	    if(loadMap){
          AffineTransform invTrans = null;
          try{
            invTrans = getRenderingTransform().createInverse();
          } catch(NoninvertibleTransformException ex){
            System.out.println("Can't create inverse matrix");
          } //of try-catch

          AffineTransform renTrans = new AffineTransform();
          renTrans.translate(getWidth() / 2, getHeight() / 2);
          renTrans.rotate(alpha);
          renTrans.scale(scale, scale);
          renTrans.translate(trans_x, trans_y);

          AffineTransform paintTrans = new AffineTransform(renTrans);
          paintTrans.concatenate(invTrans);

          setPaintingTransform(paintTrans);
          renderer.startWaiting(/*RENDER_TIME*/100, renTrans);
          //setRenderingTransform(renTrans);
	    } //of if
	    repaintAfterMove();
	  } //of componentResized
	}); //of ComponentAdapter
  } //of TGPanel

  public GraphEltSet getGraphEltSet(){
    return completeEltSet;
  } //of getGraphEltSet

  private void updatePartitions(){
    //--first remove all invisible edges--//
    //Vector invisibleEdges = new Vector();
    //for(int i=0; i<completeEltSet.edgeCount(); i++){
    //  Edge edge = completeEltSet.edgeAt(i);
    //  if(!edge.isVisible())invisibleEdges.add(edge);
    //} //of for
    //for(int i=0; i<invisibleEdges.size(); i++){
    //  Edge edge = (Edge)invisibleEdges.elementAt(i);
    //  synchronized(localityUtils){
    //    visibleLocality.deleteEdge(edge);
    //    resetDamper();
    //  } //of synchronized
    //} //of for
    for(int i=completeEltSet.edgeCount()-1; i>=0; i--){
      Edge edge = completeEltSet.edgeAt(i);
      if(!edge.isVisible()){
      	synchronized(localityUtils){
    	    visibleLocality.deleteEdge(edge);
    	    resetDamper();
        } //of synchronized
      } //of if
    } //of for

    //--build new invisible edges--//
    Vector partitions = new Vector();
    for(int i=0; i<completeEltSet.nodeCount(); i++){
      Node node = completeEltSet.nodeAt(i);
      boolean new_partition = true;
      for(int j=0; j<partitions.size(); j++)
        if(node.getPartID() == ((Node)partitions.elementAt(j)).getPartID()){
          new_partition = false;
          break;
        } //of if
      if(new_partition)partitions.add(node);
    } //of for
    for(int i=1; i<partitions.size(); i++){
      Edge edge = new Edge((Node)partitions.elementAt(i-1), (Node)partitions.elementAt(i), 128);
      synchronized(localityUtils){
        edge.setVisible(false);
        visibleLocality.addEdge(edge);
        resetDamper();
      } //of synchronized
    } //of for

    //--finally update partition id of each node--//
    /*
    Node.glob_part_cnt = 0;
    int part_id = completeEltSet.nodeAt(0).getPartID();
    for(int i=0; i<completeEltSet.nodeCount(); i++){
      Node node = completeEltSet.nodeAt(i);
      if(part_id != node.getPartID())Node.glob_part_cnt++;
      part_id = node.getPartID();
      node.setPartID(Node.glob_part_cnt);
    } //of for
    */
  } //of updatePartitions

  public void setPixelForOneMeter(int pixOneMeter){
    this.pixOneMeter = pixOneMeter;
    repaint();
  } //of setPixelForOneMeter

  public int getPixelForOneMeter(){
    return pixOneMeter;
  } //of getPixelForOneMeter

  public void setMetricVisible(boolean metricViewable){
    this.metricViewable = metricViewable;
    repaint();
  } //of setMetricVisible

  public boolean isMetricVisible(){
    return metricViewable;
  } //of isMetricVisible
  //--end added by christian--//

  public void setLensSet(TGLensSet lensSet){
    tgLensSet = lensSet;
  } //of setLensSet

  public void setTGLayout(TGLayout tgl){
    tgLayout = tgl;
  } //of setTGLayout

  public void setGraphEltSet(GraphEltSet ges){
    completeEltSet = ges;
    visibleLocality = new VisibleLocality(completeEltSet);
    localityUtils = new LocalityUtils(visibleLocality, this);
  } //of setGraphEltSet

  public AdjustOriginLens getAdjustOriginLens(){
    return adjustOriginLens;
  } //of getAdjustOriginLens

  public SwitchSelectUI getSwitchSelectUI() {
    return switchSelectUI;
  } //of getSwitchSelectUI

  // color and font setters ......................

  public void setBackColor(Color color){
    BACK_COLOR = color;
  } //of setBackColor

  //--added by christian--//
  public void loadMap(String path){
    if(path != null){
      try{
        loadSVGDocument(new java.io.File(path).toURL().toString());
      } catch(java.net.MalformedURLException ex){
        ex.printStackTrace();
      } //of try-catch
      if(renderer == null)renderer = new JSVGCanvasRenderer(this);
      addGVTTreeBuilderListener(new GVTTreeBuilderAdapter(){
        public void gvtBuildCompleted(GVTTreeBuilderEvent e){
          Dimension2D tmp_size = getSVGDocumentSize();
          map_w = (int)tmp_size.getWidth();
          map_h = (int)tmp_size.getHeight();
          resetBackground();
          System.out.println("Load map successful");
        } //of gvtBuildCompleted
      }); //of GVTTreeBuilderAdapter
      addGVTTreeRendererListener(new GVTTreeRendererAdapter(){
        public void gvtRenderingCompleted(GVTTreeRendererEvent e){
          loadMap = true;
        } //of gvtRenderingComplete
      }); //of GVTTreeRendererAdapter
    } //of if
  } //of loadMap

  public void resetBackground(){
    alpha = 0.0;
    scale = 1.0;
    trans_x = -map_w / 2;
    trans_y = -map_h / 2;

    AffineTransform renTrans = new AffineTransform();
    renTrans.translate(getWidth() / 2, getHeight() / 2);
    renTrans.rotate(alpha);
    renTrans.scale(scale, scale);
    renTrans.translate(trans_x, trans_y);

    setRenderingTransform(renTrans);

    repaint();
  } //of resetBackground

  public boolean isMapLoaded(){
    return loadMap;
  } //of isMapLoaded
  //--end added by christian--//

  // Node manipulation ...........................

  /** Returns an Iterator over all nodes in the complete graph. */
  /*
  public Iterator getAllNodes() {
    return completeEltSet.getNodes();
  } //of getAllNodes
  */

  /** Return the current visible locality. */
  public ImmutableGraphEltSet getGES(){
    return visibleLocality;
  } //of getGES

  /** Returns the current node count. */
  public int getNodeCount(){
    return completeEltSet.nodeCount();
  } //of getNodeCount

  /** Returns the current node count within the VisibleLocality. */
  public int visibleNodeCount(){
    return visibleLocality.nodeCount();
  } //of visibleNodeCount

  /** Return the Node whose ID matches the String <tt>id</tt>, null if no match is found.
    *
    * @param id The ID identifier used as a query.
    * @return The Node whose ID matches the provided 'id', null if no match is found.
    */
  public Node findNode(String id){
    if(id == null)return null; // ignore
    return completeEltSet.findNode(id);
  } //of findNode

  /** Return the Node whose URL matches the String <tt>strURL</tt>, null if no match is found.
    *
    * @param strURL The URL identifier used as a query.
    * @return The Node whose URL matches the provided 'URL', null if no match is found.
    */
  public Node findNodeByURL(String strURL){
    if(strURL == null)return null; // ignore
    return completeEltSet.findNodeByURL(strURL);
  } //of findNodeByURL

  /** Return a Collection of all Nodes whose label matches the String <tt>label</tt>,
    * null if no match is found.
    */
  /*
  public Collection findNodesByLabel(String label){
    if(label == null)return null; // ignore
    return completeEltSet.findNodesByLabel(label);
  } //of findNodesByLabel
  */

  /** Return the first Nodes whose label contains the String <tt>substring</tt>,
    * null if no match is found.
    * @param substring The Substring used as a query.
    */
  public Node findNodeLabelContaining(String substring){
    if(substring == null)return null; //ignore
    return completeEltSet.findNodeLabelContaining(substring);
  } //of findNodeLabelContaining

  /** Adds a Node, with its ID and label being the current node count plus 1.
    * @see com.touchgraph.graphlayout.Node
    */
  public Node addNode() throws TGException{
    String id = String.valueOf(getNodeCount() + 1);
    return addNode(id, null);
  } //of addNode

  /** Adds a Node, provided its label.  The node is assigned a unique ID.
    * @see com.touchgraph.graphlayout.graphelements.GraphEltSet
    */
  public Node addNode(String label) throws TGException{
    return addNode(null, label);
  } //of addNode

  /** Adds a Node, provided its ID and label.
     * @see com.touchgraph.graphlayout.Node
     */
  public Node addNode(String id, String label) throws TGException{
    Node node;
    if(label == null)node = new Node(id);
    else node = new Node(id, label);

    updateDrawPos(node); // The addNode() call should probably take a position, this just sets it at 0,0
    addNode(node);
    return node;
  } //of addNode

  /** Add the Node <tt>node</tt> to the visibleLocality, checking for ID uniqueness. */
  public void addNode(final Node node)throws TGException{
    synchronized(localityUtils){
      visibleLocality.addNode(node);
      resetDamper();
    } //of synchronized
    updatePartitions();
  } //of addNode

  /** Remove the Node object matching the ID <code>id</code>, returning true if the
    * deletion occurred, false if a Node matching the ID does not exist (or if the ID
    * value was null).
    *
    * @param id The ID identifier used as a query.
    * @return true if the deletion occurred.
    */
  public boolean deleteNodeById(String id){
    if(id == null)return false; // ignore
    Node node = findNode(id);
    if( node == null)return false;
    else return deleteNode(node);
  } //of deleteNodeById

  public boolean deleteNode(Node node){
    synchronized(localityUtils){
      if(visibleLocality.deleteNode(node)){ // delete from visibleLocality, *AND completeEltSet
        if(node == select)clearSelect();
        resetDamper();
        updatePartitions();
        return true;
      } //of if
      updatePartitions();
      return false;
    } //of synchronized
  } //of deleteNode

  public void clearAll(){
    synchronized(localityUtils){
      visibleLocality.clearAll();
    } //of synchronized
  } //of clearAll

  public Node getSelect(){
    return select;
  } //of getSelect

  public Node getMouseOverN(){
    return mouseOverN;
  } //of getMouseOverN

  public /*synchronized*/ void setMouseOverN(Node node){
    if((dragNode != null)||(maintainMouseOver))return;  // So you don't accidentally switch nodes while dragging
    if(mouseOverN != node){
      mouseOverN = node;
    } //of if

    if(mouseOverN == null)setCursor(new Cursor(Cursor.MOVE_CURSOR));
    else setCursor(new Cursor(Cursor.HAND_CURSOR));
  } //of setMouseOverN

  // Edge manipulation ...........................
  /** Returns an Iterator over all edges in the complete graph.
    */
  /*
  public Iterator getAllEdges(){
    return completeEltSet.getEdges();
  } //of getAllEdges
  */
  public void deleteEdge(Edge edge){
    synchronized(localityUtils){
      visibleLocality.deleteEdge(edge);
      resetDamper();
    } //of synchronized
    updatePartitions();
  } //of deleteEdge

  public void deleteEdge(Node from, Node to){
    synchronized(localityUtils){
      visibleLocality.deleteEdge(from, to);
      resetDamper();
    } //of synchronized
    updatePartitions();
  } //of deleteEdge

  /** Returns the current edge count in the complete graph.
    */
  public int getEdgeCount(){
    return completeEltSet.edgeCount();
  } //of getEdgeCount

  /** Return the number of Edges in the Locality.
    */
  public int visibleEdgeCount(){
    return visibleLocality.edgeCount();
  } //of visibleEdgeCount

  public Edge findEdge(Node f, Node t){
    return visibleLocality.findEdge(f, t);
  } //of findEdge

  public void addEdge(Edge e){
    synchronized(localityUtils){
      visibleLocality.addEdge(e);
      resetDamper();
    } //of synchronized
    updatePartitions();
  } //of addEdge

  public Edge addEdge(Node f, Node t, int tens){
    synchronized(localityUtils){
      Edge edge = visibleLocality.addEdge(f, t, tens);
      updatePartitions();
      return edge;
    } //of synchronized
  } //of addEdge

  public Edge getMouseOverE(){
    return mouseOverE;
  } //of getMouseOverE

  public synchronized void setMouseOverE(Edge edge){
    if((dragNode != null)||(maintainMouseOver))return; // No funny business while dragging
    if(mouseOverE != edge){
      mouseOverE = edge;
    } //of if
  } //of setMouseOverE

  // miscellany ..................................

  protected class AdjustOriginLens extends TGAbstractLens{
    protected void applyLens(TGPoint2D p){
      p.x += TGPanel.this.getSize().width / 2;
      p.y += TGPanel.this.getSize().height / 2;
    } //of applyLens

    protected void undoLens(TGPoint2D p){
      p.x -= TGPanel.this.getSize().width / 2;
      p.y -= TGPanel.this.getSize().height / 2;
    } //of undoLens
  } //of class AdjusOriginLens

    public class SwitchSelectUI extends TGAbstractClickUI {
        public void mouseClicked(MouseEvent e) {
            if ( mouseOverN != null ) {
                if ( mouseOverN != select )
                    setSelect(mouseOverN);
                else
                    clearSelect();
            }
        }
    }

    void fireMovedEvent() {
        Vector listeners;

        synchronized(this) {
              listeners = (Vector)graphListeners.clone();
           }

        for (int i=0;i<listeners.size();i++){
              GraphListener gl = (GraphListener) listeners.elementAt(i);
              gl.graphMoved();
        }
      }

    public void fireResetEvent() {
        Vector listeners;

        synchronized(this) {
              listeners = (Vector)graphListeners.clone();
        }

        for (int i=0;i<listeners.size();i++){
              GraphListener gl = (GraphListener) listeners.elementAt(i);
              gl.graphReset();
        }
    }

    public synchronized void addGraphListener(GraphListener gl){
        graphListeners.addElement(gl);
    }


    public synchronized void removeGraphListener(GraphListener gl){
        graphListeners.removeElement(gl);
    }


    public synchronized void addPaintListener(TGPaintListener pl){
        paintListeners.addElement(pl);
    }


    public synchronized void removePaintListener(TGPaintListener pl){
        paintListeners.removeElement(pl);
    }

    public void setMaintainMouseOver( boolean maintain ) {
        maintainMouseOver = maintain;
    }

    public void clearSelect() {
        if ( select != null ) {
            select = null;
            repaint();
        }
    }

   /** A convenience method that selects the first node of a graph, so that hiding works.
     */
    public void selectFirstNode() {
        setSelect(getGES().getFirstNode());
    }

    public void setSelect( Node node ) {
          if ( node != null ) {
              select = node;
              repaint();
          } else if ( node == null ) clearSelect();
    }

    public void multiSelect( TGPoint2D from, TGPoint2D to ) {
        final double minX,minY,maxX,maxY;

        if ( from.x > to.x ) { maxX = from.x; minX = to.x; }
        else                 { minX = from.x; maxX = to.x; }
        if ( from.y > to.y ) { maxY = from.y; minY = to.y; }
        else                 { minY = from.y; maxY = to.y; }

        final Vector selectedNodes = new Vector();

        TGForEachNode fen = new TGForEachNode() {
            public void forEachNode( Node node ) {
                double x = node.drawx;
                double y = node.drawy;
                if ( x > minX && x < maxX && y > minY && y < maxY ) {
                    selectedNodes.addElement(node);
                }
            }
        };

        visibleLocality.forAllNodes(fen);

        if ( selectedNodes.size() > 0 ) {
            int r = (int)( Math.random()*selectedNodes.size() );
            setSelect((Node)selectedNodes.elementAt(r));
        } else {
            clearSelect();
        }
    }


    public void updateLocalityFromVisibility() throws TGException {
        visibleLocality.updateLocalityFromVisibility();
    }

    public void setLocale( Node node, int radius, int maxAddEdgeCount, int maxExpandEdgeCount,
                           boolean unidirectional ) throws TGException {
        localityUtils.setLocale(node,radius, maxAddEdgeCount, maxExpandEdgeCount, unidirectional);
    }

    public void fastFinishAnimation() { //Quickly wraps up the add node animation
        localityUtils.fastFinishAnimation();
    }

    public void setLocale( Node node, int radius ) throws TGException {
        localityUtils.setLocale(node,radius);
    }

    public void expandNode( Node node ) {
        localityUtils.expandNode(node);
    }

    public void hideNode( Node hideNode ) {
        localityUtils.hideNode(hideNode );
    }

    public void collapseNode( Node collapseNode ) {
        localityUtils.collapseNode(collapseNode );
    }

    public void hideEdge( Edge hideEdge ) {
        visibleLocality.removeEdge(hideEdge);
        if ( mouseOverE == hideEdge ) setMouseOverE(null);
        resetDamper();
    }

    public void setDragNode( Node node ) {
        dragNode = node;
        tgLayout.setDragNode(node);
    }

    public Node getDragNode() {
        return dragNode;
    }

    void setMousePos( Point p ) {
        mousePos = p;
    }

    public Point getMousePos() {
        return mousePos;
    }

    /** Start and stop the damper.  Should be placed in the TGPanel too. */
    public void startDamper() {
        if (tgLayout!=null) tgLayout.startDamper();
    }

    public void stopDamper() {
        if (tgLayout!=null) tgLayout.stopDamper();
    }

    /** Makes the graph mobile, and slowly slows it down. */
    public void resetDamper() {
        if (tgLayout!=null) tgLayout.resetDamper();
    }

    /** Gently stops the graph from moving */
    public void stopMotion() {
        if (tgLayout!=null) tgLayout.stopMotion();
    }

    class BasicMouseListener extends MouseAdapter {

        public void mouseEntered(MouseEvent e) {
            addMouseMotionListener(basicMML);
        }

        public void mouseExited(MouseEvent e) {
            removeMouseMotionListener(basicMML);
            mousePos = null;
            setMouseOverN(null);
            setMouseOverE(null);
            repaint();
        }
    }

    class BasicMouseMotionListener implements MouseMotionListener {
        public void mouseDragged(MouseEvent e) {
            mousePos = e.getPoint();
            findMouseOver();
            try {
                Thread.currentThread().sleep(6); //An attempt to make the cursor flicker less
            } catch (InterruptedException ex) {
                //break;
            }
        }

        public void mouseMoved( MouseEvent e ) {
            mousePos = e.getPoint();
            synchronized(this) {
                findMouseOver();
                //if((oldMouseOverE != mouseOverE)||(oldMouseOverN != mouseOverN)){
                //    //System.out.println("DEBUG: TGPanel.mouseMoved");
                //    if(mouseOverN != null)System.out.println("Partition-ID from Node " +
                //      mouseOverN.getLabel() + " is " + mouseOverN.getPartID());
                //    repaint();
                //} //of if
                // Replace the above lines with the commented portion below to prevent whole graph
                // from being repainted simply to highlight a node On mouseOver.
                // This causes some annoying flickering though.
                /*
                if(oldMouseOverE!=mouseOverE) {
                    if (oldMouseOverE!=null) {
                        synchronized(oldMouseOverE) {
                            oldMouseOverE.paint(TGPanel.this.getGraphics(),TGPanel.this);
                            oldMouseOverE.from.paint(TGPanel.this.getGraphics(),TGPanel.this);
                            oldMouseOverE.to.paint(TGPanel.this.getGraphics(),TGPanel.this);

                        }
                    }

                    if (mouseOverE!=null) {
                        synchronized(mouseOverE) {
                            mouseOverE.paint(TGPanel.this.getGraphics(),TGPanel.this);
                            mouseOverE.from.paint(TGPanel.this.getGraphics(),TGPanel.this);
                            mouseOverE.to.paint(TGPanel.this.getGraphics(),TGPanel.this);
                        }
                    }
                }

                if(oldMouseOverN!=mouseOverN) {
                    if (oldMouseOverN!=null) oldMouseOverN.paint(TGPanel.this.getGraphics(),TGPanel.this);
                    if (mouseOverN!=null) mouseOverN.paint(TGPanel.this.getGraphics(),TGPanel.this);
                }
                */
            }
        }
    }

    protected /*synchronized*/ void findMouseOver() {

        if ( mousePos == null ) {
            setMouseOverN(null);
            setMouseOverE(null);
            return;
        }

        final int mpx=mousePos.x;
        final int mpy=mousePos.y;

        final Node[] monA = new Node[1];
        final Edge[] moeA = new Edge[1];

        TGForEachNode fen = new TGForEachNode() {

            double minoverdist = 100; //Kind of a hack (see second if statement)
                                        //Nodes can be as wide as 200 (=2*100)
            public void forEachNode( Node node ) {
                double x = node.drawx;
                double y = node.drawy;

                double dist = Math.sqrt((mpx-x)*(mpx-x)+(mpy-y)*(mpy-y));

                  if ( ( dist < minoverdist ) && node.containsPoint(mpx,mpy) ) {
                      minoverdist = dist;
                      monA[0] = node;
                  }
            }
        };
        visibleLocality.forAllNodes(fen);

        TGForEachEdge fee = new TGForEachEdge() {

            double minDist = 8; // Tangential distance to the edge
            double minFromDist = 1000; // Distance to the edge's "from" node

            public void forEachEdge( Edge edge ) {
                double x = edge.from.drawx;
                double y = edge.from.drawy;
                double dist = edge.distFromPoint(mpx,mpy);
                if ( dist < minDist ) { // Set the over edge to the edge with the minimun tangential distance
                    minDist = dist;
                    minFromDist = Math.sqrt((mpx-x)*(mpx-x)+(mpy-y)*(mpy-y));
                    moeA[0] = edge;
                } else if ( dist == minDist ) { // If tangential distances are identical, chose
                                                // the edge whose "from" node is closest.
                    double fromDist = Math.sqrt((mpx-x)*(mpx-x)+(mpy-y)*(mpy-y));
                    if ( fromDist < minFromDist ) {
                        minFromDist = fromDist;
                        moeA[0] = edge;
                    }
                }
            }
        };
        visibleLocality.forAllEdges(fee);

        setMouseOverN(monA[0]);
        if ( monA[0] == null )
            setMouseOverE(moeA[0]);
        else
            setMouseOverE(null);
    }

    TGPoint2D topLeftDraw = null;
    TGPoint2D bottomRightDraw = null;

    public TGPoint2D getTopLeftDraw() {
        return new TGPoint2D(topLeftDraw);
    }

    public TGPoint2D getBottomRightDraw() {
        return new TGPoint2D(bottomRightDraw);
    }

    public TGPoint2D getCenter() {
        return tgLensSet.convDrawToReal(0.5 * getSize().width, 0.5 * getSize().height);
    }

    public TGPoint2D getDrawCenter() {
        return new TGPoint2D(getSize().width/2,getSize().height/2);
    }

  public void updateGraphSize(){
    if(topLeftDraw == null)topLeftDraw = new TGPoint2D(0, 0);
    if(bottomRightDraw == null)bottomRightDraw = new TGPoint2D(0, 0);

    TGForEachNode fen = new TGForEachNode(){
      boolean firstNode=true;
      public void forEachNode(Node node){
        if(firstNode){ //initialize topRight + bottomLeft
          topLeftDraw.setLocation(node.drawx, node.drawy);
          bottomRightDraw.setLocation(node.drawx, node.drawy);
          firstNode = false;
        } else { //Standard max and min finding
          topLeftDraw.setLocation(Math.min(node.drawx, topLeftDraw.x),
                                  Math.min(node.drawy, topLeftDraw.y));
          bottomRightDraw.setLocation(Math.max(node.drawx, bottomRightDraw.x),
                                      Math.max(node.drawy, bottomRightDraw.y));
        } //of if-else
      } //of forEachNode
    }; //of TGForEachNode
    visibleLocality.forAllNodes(fen);
  } //of updateGraphSize

  public synchronized void processGraphMove(){
    updateDrawPositions();
    updateGraphSize();
  } //of processGraphMove

  public void repaintAfterMove(){ // Called by TGLayout + others to indicate that graph has moved
    processGraphMove();
    findMouseOver();
    fireMovedEvent();
    repaint();
  } //of repaintAfterMove

  //--added by christian--//
  public void repaintAfterMoveBackground(double dx, double dy){ //called by "HVScroll"
    if((dx == 0.0)&&(dy == 0.0))return;
    if(loadMap){
      trans_x += dx;
      trans_y += dy;

      AffineTransform invTrans = null;
      try{
        invTrans = getRenderingTransform().createInverse();
      } catch(NoninvertibleTransformException ex){
        System.out.println("Can't create inverse matrix");
      } //of try-catch

      AffineTransform renTrans = new AffineTransform();
      renTrans.translate(getWidth() / 2, getHeight() / 2);
      renTrans.rotate(alpha);
      renTrans.scale(scale, scale);
      renTrans.translate(trans_x, trans_y);

      AffineTransform paintTrans = new AffineTransform(renTrans);
      paintTrans.concatenate(invTrans);

      setPaintingTransform(paintTrans);
      renderer.startWaiting(RENDER_TIME, renTrans);

/*
      AffineTransform affTrans = new AffineTransform();
      //affTrans.rotate(alpha, 0.5 * getWidth(), 0.5 * getHeight());
      affTrans.translate(dx, dy);
      //affTrans.rotate(-alpha, 0.5 * getWidth(), 0.5 * getHeight());
      affTrans.concatenate(paintTrans);

      AffineTransform renTrans = new AffineTransform(affTrans);
      renTrans.concatenate(invTrans);

      paintTrans = affTrans;

      setPaintingTransform(renTrans);
      renderer.startWaiting(RENDER_TIME, paintTrans);
*/
    } //of if
    repaint();
  } //of repaintAfterMoveBackground

  public void repaintAfterZoomBackground(double scale/*, double offset, double delta*/){ //called by "ZoomScroll"
    //if(offset == 0.0)return;
    this.scale = scale;
    if(loadMap){
      AffineTransform invTrans = null;
      try{
        invTrans = getRenderingTransform().createInverse();
      } catch(NoninvertibleTransformException ex){
        System.out.println("Can't create inverse matrix");
      } //of try-catch

      AffineTransform renTrans = new AffineTransform();
      renTrans.translate(getWidth() / 2, getHeight() / 2);
      renTrans.rotate(alpha);
      renTrans.scale(scale, scale);
      renTrans.translate(trans_x, trans_y);

      AffineTransform paintTrans = new AffineTransform(renTrans);
      paintTrans.concatenate(invTrans);

      setPaintingTransform(paintTrans);
      renderer.startWaiting(RENDER_TIME, renTrans);

/*
      AffineTransform affTrans = new AffineTransform();
      affTrans.rotate(alpha, getWidth() / 2, getHeight() / 2);
      affTrans.translate(delta * trans_x, delta * trans_y);
      affTrans.translate(offset * map_w, offset * map_h);
      affTrans.translate(-trans_x, -trans_y);
      affTrans.rotate(-alpha, getWidth() / 2, getHeight() / 2);
      affTrans.concatenate(paintTrans);
      affTrans.scale(delta, delta);

      AffineTransform renTrans = new AffineTransform(affTrans);
      renTrans.concatenate(invTrans);

      paintTrans = affTrans;

      setPaintingTransform(renTrans);
      renderer.startWaiting(RENDER_TIME, paintTrans);

      trans_x *= delta;
      trans_y *= delta;
*/
    } //of if
    repaint();
  } //of repaintAfterZoomBackground

  public void repaintAfterRotateBackground(double alpha/*, double delta*/){ //called by "RotateScroll"
    //if(delta == 0.0)return;
    this.alpha = alpha;
    if(loadMap){
      AffineTransform invTrans = null;
      try{
        invTrans = getRenderingTransform().createInverse();
      } catch(NoninvertibleTransformException ex){
        System.out.println("Can't create inverse matrix");
      } //of try-catch

      AffineTransform renTrans = new AffineTransform();
      renTrans.translate(getWidth() / 2, getHeight() / 2);
      renTrans.rotate(alpha);
      renTrans.scale(scale, scale);
      renTrans.translate(trans_x, trans_y);

      AffineTransform paintTrans = new AffineTransform(renTrans);
      paintTrans.concatenate(invTrans);

      setPaintingTransform(paintTrans);
      renderer.startWaiting(RENDER_TIME, renTrans);
/*
      AffineTransform affTrans = new AffineTransform();
      affTrans.rotate(delta, getWidth() / 2, getHeight() / 2);
      affTrans.concatenate(paintTrans);

      AffineTransform renTrans = new AffineTransform(affTrans);
      renTrans.concatenate(invTrans);

      paintTrans = affTrans;

      setPaintingTransform(renTrans);
      renderer.startWaiting(RENDER_TIME, paintTrans);
*/
    } //of if
    repaint();
  } //of repaintAfterRotateBackground
  //--end added by christian--//

  public void updateDrawPos(Node node){ //sets the visual position from the real position
    TGPoint2D p = tgLensSet.convRealToDraw(node.x, node.y);
    node.drawx = p.x;
    node.drawy = p.y;
  } //of updateDrawPos

  public void updatePosFromDraw(Node node){ //sets the real position from the visual position
    TGPoint2D p = tgLensSet.convDrawToReal(node.drawx, node.drawy);
    node.x = p.x;
    node.y = p.y;
  } //of updatePosFromDraw

  public void updateDrawPositions(){
    TGForEachNode fen = new TGForEachNode(){
      public void forEachNode(Node node){
        updateDrawPos(node);
      } //of forEachNode
    }; //of TGForEachNode
    visibleLocality.forAllNodes(fen);
  } //of updateDrawPositions

    Color myBrighter( Color c ) {
        int r = c.getRed();
        int g = c.getGreen();
        int b = c.getBlue();

        r=Math.min(r+96, 255);
        g=Math.min(g+96, 255);
        b=Math.min(b+96, 255);

        return new Color(r,g,b);
    }

  public void paint(final Graphics g){
    super.paint(g);

    synchronized(this){
      paintListeners = (Vector)paintListeners.clone();
    } //of synchronized

    for(int i = 0; i < paintListeners.size(); i++){
      TGPaintListener pl = (TGPaintListener)paintListeners.elementAt(i);
      pl.paintFirst(g);
    } //of for

    TGForEachEdge fee = new TGForEachEdge(){
      public void forEachEdge(Edge edge){
        edge.paint(g, TGPanel.this);
      } //of forEachEdge
    }; //of TGForEachEdge

    visibleLocality.forAllEdges(fee);

    for(int i = 0; i < paintListeners.size() ; i++){
      TGPaintListener pl = (TGPaintListener)paintListeners.elementAt(i);
      pl.paintAfterEdges(g);
    } //of for

    TGForEachNode fen = new TGForEachNode(){
      public void forEachNode(Node node){
        node.paint(g,TGPanel.this);
      } //of forEachNode
    }; //of TGForEachNode

    visibleLocality.forAllNodes(fen);

    if(mouseOverE != null){ //Make the edge the mouse is over appear on top.
      mouseOverE.paint(g, this);
      mouseOverE.from.paint(g, this);
      mouseOverE.to.paint(g, this);
    } //of if

    if(select != null){ //Make the selected node appear on top.
      select.paint(g, this);
    } //of if

    if(mouseOverN != null){ //Make the node the mouse is over appear on top.
      mouseOverN.paint(g, this);
    } //of if

    for(int i = 0; i < paintListeners.size(); i++){
      TGPaintListener pl = (TGPaintListener)paintListeners.elementAt(i);
      pl.paintLast(g);
    } //of for

    if(metricViewable){
      int met_x = getWidth() - 70;
      int met_y = getHeight() - 120;
      String half_info = "" + (50.0 / (scale * pixOneMeter));
      String full_info = "" + (100.0 / (scale * pixOneMeter));
      int idx = half_info.indexOf('.');
      if(idx == -1)half_info += ".0";
      else half_info = half_info.substring(0, idx + 2);
      idx = full_info.indexOf('.');
      if(idx == -1)full_info += ".0";
      else full_info = full_info.substring(0, idx + 2);

      g.setColor(Color.ORANGE);
      g.setFont(new Font("Arial", Font.PLAIN, 12));
      g.fillRect(met_x,     met_y,      20,  3);
      g.fillRect(met_x + 8, met_y +  3,  3, 97);
      g.fillRect(met_x,     met_y + 97, 20,  3);
      g.fillRect(met_x + 4, met_y + 24, 11,  3);
      g.fillRect(met_x + 4, met_y + 49, 11,  3);
      g.fillRect(met_x + 4, met_y + 74, 11,  3);
      g.drawString(half_info + " m", met_x + 23, met_y + 55);
      g.drawString(full_info + " m", met_x + 23, met_y + 5);
      g.drawString("0.0 m", met_x + 23, met_y + 103);
    } //of if

    paintComponents(g); //Paint any components that have been added to this panel
  } //of paint

  public void update(Graphics g){
    paint(g);
  } //of update
} //of class TGPanel
