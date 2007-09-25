package graphlayout;

import java.io.*;
import java.net.*;
import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

public class InputParser extends DefaultHandler implements Runnable{
  private int port = -1;             //port number for input socket
  private boolean reconnect = false; //reconnect option for socket
  private Edge connect = null;       //connection between two nodes
  private String host = null;        //host adress for input socket
  private TGPanel tg_panel = null;   //touchgraph panel for drawing

  public InputParser(TGPanel tg_panel, String host, int port, boolean reconnect){
    this.host = host;
    this.port = port;
    this.tg_panel = tg_panel;
    this.reconnect = reconnect;
    Thread thread = new Thread(this);
    thread.start();
  } //of InputParser

  public InputParser(TGPanel tg_panel, String path){
    this.tg_panel = tg_panel;
    try{
      SAXParser parser = SAXParserFactory.newInstance().newSAXParser();
      parser.parse(new File(path), this);
    } catch(Throwable t){
      //System.out.println("Error/Exception in <InputParser.InputParser>");
      //System.out.print("\"");
      //t.printStackTrace();
      //System.out.println("\"");
      System.out.println("Parsing Error");
    } //of try-catch
  } //of InputParser

  public void run(){
    int try_cnt = 0;
    do{
      try{
        Socket sock = new Socket(host, port);
        OutputStream os = sock.getOutputStream();
        InputStream is = sock.getInputStream();
        os.write('x'); //XML-Type
        try{
          SAXParser parser = SAXParserFactory.newInstance().newSAXParser();
          parser.parse(is, this);
        } catch(Throwable t){
          //System.out.println("Error/Exception in method <InputParser.run>");
          //System.out.print("\"");
          //t.printStackTrace();
          //System.out.println("\"");
          System.out.println("Parsing Error");
        } //of try-catch
        is.close();
        os.close();
        sock.close();
      } catch(IOException e){
        //System.out.println("I/O-Exception in method <InputParser.run>");
        //System.out.print("\"");
        //e.printStackTrace();
        //System.out.println("\"");
        System.out.println("Connection refused");
        try_cnt++;
      } //of try-catch
      try{
        Thread.currentThread().sleep(1000);
      } catch(InterruptedException e){
      } //of try-catch
    } while((reconnect)&&(try_cnt < 3));
  } //of run

  //--methods for handling--//
  public void startDocument() throws SAXException{
  } //of startDocument

  public void endDocument() throws SAXException{
    tg_panel.repaintAfterMove();
  } //of endDocument

  public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException{
    String ntag = qName.toLowerCase();
    String attrib = "";

    if(ntag.equals("topology")){
      tg_panel.clearAll();
      return;
    } //of if

    if(attributes != null){
      for(int i = 0; i < attributes.getLength(); i++){
        attrib = attributes.getQName(i).toLowerCase();
        if(attrib.equals("id")){
          boolean newNode = ntag.equals("node") || ntag.equals("add_node");
          String id = attributes.getValue(i);
          Node node = newNode ? new Node(id, id) : tg_panel.findNode(id);
          if((newNode)||(ntag.equals("modify_node"))){
            try{
              if(newNode)tg_panel.addNode(node);
            } catch(TGException tge){
              System.out.println("TouchGraph-Exception in method <InputParser.startElement>");
              System.out.print("\"");
              tge.printStackTrace();
              System.out.println("\"");
            } //of try-catch
            double x = Double.MIN_VALUE, y = Double.MIN_VALUE; //position of node
            while((++i) < attributes.getLength()){
              if(attributes.getQName(i).toLowerCase().equals("name")){
                node.setLabel(attributes.getValue(i));
                //break;
              } //of if
              if(attributes.getQName(i).toLowerCase().equals("x")){
                try{
                  x = /*tg_panel.getPixelForOneMeter()*/(new Double(attributes.getValue(i))).doubleValue();
                } catch(NumberFormatException nfe){
                  System.out.println("NumberFormat-Exception in method <InputParser.startElement>");
                  System.out.print("\"");
                  nfe.printStackTrace();
                  System.out.println("\"");
                } //of try-catch
                //break;
              } //of if
              if(attributes.getQName(i).toLowerCase().equals("y")){
                try{
                  y = /*tg_panel.getPixelForOneMeter()*/(new Double(attributes.getValue(i))).doubleValue();
                } catch(NumberFormatException nfe){
                  System.out.println("NumberFormat-Exception in method <InputParser.startElement>");
                  System.out.print("\"");
                  nfe.printStackTrace();
                  System.out.println("\"");
                } //of try-catch
                //break;
              } //of if
              if(attributes.getQName(i).toLowerCase().equals("fixed")){
                if(attributes.getValue(i).toLowerCase().equals("true"))node.setFixed(true);
                //break;
              } //of if
            } //of while
            if((x != Double.MIN_VALUE)||(y != Double.MIN_VALUE)){
              if(x == Double.MIN_VALUE)x = (double)(java.awt.Toolkit.getDefaultToolkit().getScreenSize().width / 2);
              if(y == Double.MIN_VALUE)y = (double)(java.awt.Toolkit.getDefaultToolkit().getScreenSize().height / 2);
              node.setLocation(x, y);
            } //of if
            continue;
          } //of if
          if(ntag.equals("remove_node")){
            tg_panel.deleteNodeById(attributes.getValue(i));
            continue;
          } //of if
          continue;
        } //of if
		
        if((i > 0) && attrib.equals("to")){
          if((ntag.equals("edge"))||(ntag.equals("add_edge"))||(ntag.equals("modify_edge"))){
            Node from = tg_panel.findNode(attributes.getValue(i-1));
            Node to = tg_panel.findNode(attributes.getValue(i));
            if(from == null){
              try{
                tg_panel.addNode(from = new Node(attributes.getValue(i-1), attributes.getValue(i-1)));
              } catch(TGException tge){
                System.out.println("TouchGraph-Exception in method <InputParser.startElement>");
                System.out.print("\"");
                tge.printStackTrace();
                System.out.println("\"");
              } //of try-catch
              System.out.println("Node with id=" + attributes.getValue(i-1) + " is non-existent!");
			      } //of if
            if(to == null){
              try{
                tg_panel.addNode(to = new Node(attributes.getValue(i), attributes.getValue(i)));
              } catch(TGException tge){
                System.out.println("TouchGraph-Exception in method <InputParser.startElement>");
                System.out.print("\"");
                tge.printStackTrace();
                System.out.println("\"");
              } //of try-catch
              System.out.println("Node with id=" + attributes.getValue(i) + " is non-existent!");
            } //of if
            connect = ntag.equals("modify_edge") ? tg_panel.findEdge(from, to) : new Edge(from, to);
            //if((connect.getFrom().getLabel().equals("1"))||(connect.getTo().getLabel().equals("1")))connect.setVisible(false);
            tg_panel.addEdge(connect);
            continue;
          } //of if
          if(ntag.equals("remove_edge")){
            Node from = tg_panel.findNode(attributes.getValue(i-1));
            Node to = tg_panel.findNode(attributes.getValue(i));
            if((from == null)||(to == null)){
              System.out.println("Couldn't delete Edge because they is non-existent!");
              continue;
            } //of if
            tg_panel.deleteEdge(from, to);
            continue;
          } //of if
          continue;
        } //of if
		
        if(attrib.equals("value")){
          if(connect != null){
            int val = 100;
            try{
              String param_val = attributes.getValue(i);
              String number = param_val.substring(0, param_val.indexOf("%"));
              val = new Double(number).intValue();
            } catch(NumberFormatException nfe){
              System.out.println("NumberFormat-Exception in method <InputParser.startElement>");
              System.out.print("\"");
              nfe.printStackTrace();
              System.out.println("\"");
            } //of try-catch
            val = (255 * Math.min(Math.max(val, 1), 100)) / 100;
            connect.setColor(new java.awt.Color(255 - val, 255 - val, val));
          } //of if
          continue;
        } //of if

        if(attrib.equals("pix")){
          int pixOneMeter = 1;
          try{
            pixOneMeter = (new Integer(attributes.getValue(i))).intValue();
          } catch(NumberFormatException nfe){
            System.out.println("NumberFormat-Exception in method <InputParser.startElement>");
            System.out.print("\"");
            nfe.printStackTrace();
            System.out.println("\"");
          } //of try-catch
          if(pixOneMeter < 1)pixOneMeter = 1;
          tg_panel.setPixelForOneMeter(pixOneMeter);
          continue;
        } //of if
      } //of for
    } //of if
  } //of startElement

  public void endElement(String uri, String localName, String qName) throws SAXException{
    String ntag = qName.toLowerCase();
    if((ntag.equals("edge"))||(ntag.equals("add_edge"))||(ntag.equals("remove_edge")))connect = null;
  } //of endElement

  public void characters(char ch[], int start, int length) throws SAXException{
  } //of characters
} //of class InputParser
