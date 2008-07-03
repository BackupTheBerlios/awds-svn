package graphlayout;

import java.io.*;
import  graphlayout.graphelements.*;

public class XMLWriter{
  private int pixOneMeter;
  private FileWriter fwriter;
  private GraphEltSet completeEltSet;
  
  public XMLWriter(GraphEltSet completeEltSet, int pixOneMeter){
    fwriter = null;
    this.pixOneMeter = pixOneMeter;
    this.completeEltSet = completeEltSet;
  } //of XMLWriter
  
  public void writeXML(String path){
    try{
      fwriter = new FileWriter(path);
    } catch (IOException e){
      System.out.println("Couldn't create \"" + path + "\"!");
      return;
    } //of try-catch
    writeLine("<?xml version=\"1.0\"?>");
    //writeLine("<!DOCTYPE graph SYSTEM \"grammer.dtd\">");
    writeLine("");
    writeLine("<graph>");
    writeLine("\t<topology>");
    writeLine("\t\t<metric pix=\"" + pixOneMeter + "\"/>");

    //--save all nodes--//
    for(int i=0; i<completeEltSet.nodeCount(); i++){
      Node node = completeEltSet.nodeAt(i);
      writeLine("\t\t<node id=\"" + node.getID() + "\" name=\"" + node.getLabel() +
                "\" x=\"" + node.getLocationX() + "\" y=\"" + node.getLocationY() +
                "\" fixed=\"" + node.isFixed() + "\"/>");
    } //of for
    //--save all edges--//
    for(int i=0; i<completeEltSet.edgeCount(); i++){
      Edge edge = completeEltSet.edgeAt(i);
      if(edge.isVisible()){
        byte value = (byte)(((edge.getColor().getBlue() * 100.0) / 255.0) + 0.5);
        writeLine("\t\t<edge from=\"" + edge.getFrom().getID() + "\" to=\"" + edge.getTo().getID() + "\">");
        writeLine("\t\t\t<quality value=\"" + value + "%\"/>");
        writeLine("\t\t</edge>");
      } //of if
    } //of for
    
    writeLine("\t</topology>");
    writeLine("</graph>");
    closeFile();
  } //of writeXML
  
  private void writeLine(String line){
    if(fwriter != null){
      try{
        fwriter.write(line + "\r\n");
      } catch (IOException e){
        System.out.println("Couldn't write \"" + line + "\"!");
        return;
      } //of try-catch
    } //of if
  } //of writeLine
  
  private void closeFile(){
    if(fwriter != null){
      try{
        fwriter.close();
      } catch (IOException e){
        System.out.println("Couldn't complete XML-File!");
        return;
      } //of try-catch
    } //of if
  } //of fclose
} //of class XMLWriter
