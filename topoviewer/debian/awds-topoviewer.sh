#! /bin/sh

CP=/usr/share/java/batik.jar:/usr/share/java/xercesImpl.jar:/usr/share/java/awds-topoviewer.jar

PARAMS="-net localhost 8333"

[ "$#" -eq 1 ] && PARAMS="-net $1 8333" 
[ "$#" -eq 2 ] && PARAMS="-net $1 $2" 

exec java -classpath "$CP" graphlayout/TopologyViewer $PARAMS

