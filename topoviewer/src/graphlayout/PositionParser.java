package graphlayout;

import java.io.*;
import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

public class PositionParser extends DefaultHandler{
  private TGPanel tg_panel = null; //touchgraph panel for drawing

  public PositionParser(TGPanel tg_panel, String path){
    this.tg_panel = tg_panel;
    try{
      SAXParser parser = SAXParserFactory.newInstance().newSAXParser();
      parser.parse(new File(path), this);
    } catch(Throwable t){
      System.out.println("Error/Exception in <PositionParser.PositionParser>");
      System.out.print("\"");
      t.printStackTrace();
      System.out.println("\"");
    } //of try-catch
  } //of PositionParser

  //--methods for the handling--//
  public void startDocument() throws SAXException{
  } //of startDocument

  public void endDocument() throws SAXException{
    tg_panel.repaintAfterMove();
  } //of endDocument

  public void startElement(String uri, String localName, String qName,
                           Attributes attributes) throws SAXException{
    String ntag = qName.toLowerCase();
    String attrib = "";

    if(attributes != null){
      for(int i = 0; i < attributes.getLength(); i++){
        attrib = attributes.getQName(i).toLowerCase();
        if(attrib.equals("id")){
          Node node = tg_panel.findNode(attributes.getValue(i));
          if(node == null)continue;
          if((ntag.equals("node"))||(ntag.equals("add_node"))){
            double x = Double.MIN_VALUE, y = Double.MIN_VALUE; //position of node
            while((++i) < attributes.getLength()){
              if(attributes.getQName(i).toLowerCase().equals("x")){
                try{
                  x = (new Double(attributes.getValue(i))).doubleValue();
                } catch(NumberFormatException nfe){
                  System.out.println("NumberFormat-Exception in method <PositionParser.startElement>");
                  System.out.print("\"");
                  nfe.printStackTrace();
                  System.out.println("\"");
                } //of try-catch
              } //of if
              if(attributes.getQName(i).toLowerCase().equals("y")){
                try{
                  y = (new Double(attributes.getValue(i))).doubleValue();
                } catch(NumberFormatException nfe){
                  System.out.println("NumberFormat-Exception in method <PositionParser.startElement>");
                  System.out.print("\"");
                  nfe.printStackTrace();
                  System.out.println("\"");
                } //of try-catch
              } //of if
              if(attributes.getQName(i).toLowerCase().equals("fixed")){
                if(attributes.getValue(i).toLowerCase().equals("true"))node.setFixed(true);
                //break;
              } //of if
            } //of while
            if((x != Double.MIN_VALUE)||(y != Double.MIN_VALUE)){
              if(x == Double.MIN_VALUE)x = (double)(java.awt.Toolkit.getDefaultToolkit().getScreenSize().width >> 1);
              if(y == Double.MIN_VALUE)y = (double)(java.awt.Toolkit.getDefaultToolkit().getScreenSize().height >> 1);
              node.setLocation(x, y);
            } //of if
          } //of if
        } //of if
      } //of for
    } //of if
  } //of startElement

  public void endElement(String uri, String localName, String qName) throws SAXException{
  } //of endElement

  public void characters(char ch[], int start, int length) throws SAXException{
  } //of characters
} //of class PositionParser
