package graphlayout;

import java.io.*;
import java.awt.*;
import javax.imageio.*;
import java.awt.image.*;
import com.sun.image.codec.jpeg.*;

import org.w3c.dom.*;
import org.apache.batik.dom.*;
import org.apache.batik.svggen.*;

public class GraphicExporter{
  private TGPanel tg_panel = null;
  
  public GraphicExporter(TGPanel tg_panel){
    this.tg_panel = tg_panel;
  } //of GraphicExporter

  public static String[] getSupportedTypes(){
    String[] supWriters = ImageIO.getWriterFormatNames();
    String[] result = null;
    int cnt = 2;
    for(int i = 0; i < supWriters.length; i++){
      if(supWriters[i].equals("bmp"))cnt++;
      if(supWriters[i].equals("gif"))cnt++;
      if(supWriters[i].equals("png"))cnt++;
    } //of for
    result = new String[cnt];
    cnt = 0;
    for(int i = 0; i < supWriters.length; i++)
      if(supWriters[i].equals("bmp")){
        result[cnt++] = "bmp";
        break;
      } //of if
    for(int i = 0; i < supWriters.length; i++)
      if(supWriters[i].equals("gif")){
        result[cnt++] = "gif";
        break;
      } //of if
    result[cnt++] = "jpg";
    for(int i = 0; i < supWriters.length; i++)
      if(supWriters[i].equals("png")){
        result[cnt++] = "png";
        break;
      } //of if
    result[cnt] = "svg";
    return result;
  } //of getSupportedTypes

  //--Simple BMP Exporter--//
  public void createBMPFile(String path) throws Throwable{
    BufferedImage bufImg = new BufferedImage(tg_panel.getWidth(), tg_panel.getHeight(), BufferedImage.TYPE_INT_RGB);
    tg_panel.createGraphic(bufImg.createGraphics(), true);
    ImageIO.write(bufImg, "bmp", new File(path));
  } //of createBMPFile

  //--Simple GIF Exporter--//
  public void createGIFFile(String path) throws Throwable{
    BufferedImage bufImg = new BufferedImage(tg_panel.getWidth(), tg_panel.getHeight(), BufferedImage.TYPE_INT_RGB);
    tg_panel.createGraphic(bufImg.createGraphics(), true);
    ImageIO.write(bufImg, "gif", new File(path));
  } //of createGIFFile
  
  //--Simple JPG Exporter--//
  public void createJPGFile(String path) throws Throwable{
    BufferedImage bufImg = new BufferedImage(tg_panel.getWidth(), tg_panel.getHeight(), BufferedImage.TYPE_INT_RGB);
    tg_panel.createGraphic(bufImg.createGraphics(), true);
    ImageIO.write(bufImg, "jpg", new File(path));
  } //of createJPGFile

  //--Simple JPG Exporter--//
  public void createJPGFile(String path, int quality) throws Throwable{
    float qual = quality / 100.0f;
    BufferedImage bufImg = new BufferedImage(tg_panel.getWidth(), tg_panel.getHeight(), BufferedImage.TYPE_INT_RGB);
    tg_panel.createGraphic(bufImg.createGraphics(), true);
    JPEGImageEncoder enc = JPEGCodec.createJPEGEncoder(new FileOutputStream(path));
    JPEGEncodeParam param = enc.getDefaultJPEGEncodeParam(bufImg);
    param.setQuality(qual, false);
    enc.setJPEGEncodeParam(param);
    enc.encode(bufImg);
  } //of createJPGFile

  //--Simple PNG Exporter--//
  public void createPNGFile(String path) throws Throwable{
    BufferedImage bufImg = new BufferedImage(tg_panel.getWidth(), tg_panel.getHeight(), BufferedImage.TYPE_INT_RGB);
    tg_panel.createGraphic(bufImg.createGraphics(), true);
    ImageIO.write(bufImg, "png", new File(path));
  } //of createPNGFile

  //--Simple SVG Exporter--//
  public void createSVGFile(String path) throws Throwable{
    DOMImplementation domImpl = GenericDOMImplementation.getDOMImplementation();
    Document document = domImpl.createDocument("http://www.w3.org/2000/svg", "svg", null);
    SVGGraphics2D svgGenerator = new SVGGraphics2D(document);
    tg_panel.createGraphic(svgGenerator, tg_panel.isMapLoaded());
    svgGenerator.stream(new OutputStreamWriter(new FileOutputStream(path), "UTF-8"), true); //using CSS style
  } //of createSVGFile
} //of class Graphic Exporter
