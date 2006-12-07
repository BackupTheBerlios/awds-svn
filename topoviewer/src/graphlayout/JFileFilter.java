package graphlayout;

import java.io.*;
import java.util.*;

public class JFileFilter extends javax.swing.filechooser.FileFilter{
  private Hashtable filters;
  private String description;

  public JFileFilter(){
    filters = new Hashtable();
    description = "All files";
  } //of JFileFilter

  public void addExtension(String extension){
    filters.put(extension.toLowerCase(), this);
  } //of setDescription

  public String getExtension(File f){
    if(f != null){
      String filename = f.getName();
      int k = filename.lastIndexOf('.');
      if((k > 0)&&(k < filename.length()-1))
        return filename.substring(k + 1).toLowerCase();
    } //of if
    return null;
  } //of getExtension

  public void setDescription(String description){
    this.description = description;
  } //of setDescription
  
  public boolean accept(File f){
    if(f != null){
      if(f.isDirectory())return true;
      String extension = getExtension(f);
      if((extension != null)&&(filters.get(extension) != null))return true;
    } //of if
    return false;
  } //of accept
  
  public String getDescription(){
    return description;
  } //of getDescription
} //of JFileFilter
