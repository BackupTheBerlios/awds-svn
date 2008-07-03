package graphlayout;

public class JTopoStatusBar extends JStatusBar{
  private int nodeCnt, edgeCnt;
  
  public JTopoStatusBar(){
    super(2);
    resetInfo();
    resetCounter();
  } //of JTopoStatusBar
  
  public void resetInfo(){
    super.setText(0, "Ready");
  } //of resetInfo
  
  public void showConnect(String host, int port){
    super.setText(0, "Connected to: " + host + " : " + port);
  } //of showConnect
  
  public void showDisconnect(){
    super.setText(0, "Disconnected");
  } //of showDisonnect
  
  public void showLocal(String filename){
    super.setText(0, "Topology from: " + filename);
  } //of showLocal  
  
  public void incNodeCnt(){
    nodeCnt++;
    refreshCntDisplay();
  } //of incNodeCnt
  
  public void incEdgeCnt(){
    edgeCnt++;
    refreshCntDisplay();
  } //of incEdgeCnt
  
  public void decNodeCnt(){
    nodeCnt--;
    refreshCntDisplay();
  } //of decNodeCnt
  
  public void decEdgeCnt(){
    edgeCnt--;
    refreshCntDisplay();
  } //of decEdgeCnt
  
  public void resetCounter(){
    nodeCnt = 0;
    edgeCnt = 0;
    refreshCntDisplay();
  } //of resetCounter
  
  private void refreshCntDisplay(){
    super.setText(1, "Number of nodes: " + nodeCnt + "     Number of edges: " + edgeCnt);
  } //of refreshCntDisplay
} //of class JTopoStatusBar
