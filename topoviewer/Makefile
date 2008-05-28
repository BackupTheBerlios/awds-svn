
all: TopologyViewer.jar

Xerces-J-bin.2.9.0.zip:
	wget http://archive.apache.org/dist/xml/xerces-j/binaries/Xerces-J-bin.2.9.0.zip

batik-1.6.zip:
	wget http://archive.apache.org/dist/xmlgraphics/batik/batik-1.6.zip
#	wget http://apache.autinity.de/xml/batik/batik-1.6.zip

xerces-2_9_0/xercesImpl.jar: Xerces-J-bin.2.9.0.zip
	jar xvf Xerces-J-bin.2.9.0.zip xerces-2_9_0/xercesImpl.jar

org/apache/xerces/parsers/SAXParser.class: xerces-2_9_0/xercesImpl.jar
	jar xf xerces-2_9_0/xercesImpl.jar

org/apache/batik/swing/JSVGCanvas.class: batik-1.6/lib/batik-swing.jar \
		batik-1.6/lib/batik-gvt.jar
	jar xf batik-1.6/lib/batik-swing.jar
	jar xf batik-1.6/lib/batik-gvt.jar 

TopologyViewer.jar:  org/apache/batik/util/HaltingThread.class \
org/apache/xerces/parsers/SAXParser.class \
org/apache/batik/bridge/UserAgent.class \
org/apache/batik/svggen/SVGGraphics2D.class \
org/apache/batik/dom/svg/SVGGraphicsElement.class \
org/apache/batik/dom/GenericDOMImplementation.class \
org/apache/batik/ext/awt/g2d/AbstractGraphics2D.class \
org/w3c/dom/svg/SVGSVGElement.class \
org/apache/batik/css/engine/CSSContext.class \
org/apache/batik/swing/JSVGCanvas.class\
compile-stamp
	jar cvmf Manifest.txt TopologyViewer.jar graphlayout/ org/

batik-1.6/lib/batik-gvt.jar : batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-swing.jar : batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-svggen.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-dom.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-util.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-bridge.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-awt-util.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-svg-dom.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-ext.jar: batik-1.6.zip
	jar xf $^ $@

batik-1.6/lib/batik-css.jar: batik-1.6.zip
	jar xf $^ $@

org/apache/batik/bridge/UserAgent.class: batik-1.6/lib/batik-bridge.jar
	jar xf $^

org/apache/batik/util/HaltingThread.class: batik-1.6/lib/batik-util.jar 
	jar xf $^

org/apache/batik/svggen/SVGGraphics2D.class: batik-1.6/lib/batik-svggen.jar
	jar xf $^

org/apache/batik/dom/svg/SVGGraphicsElement.class: batik-1.6/lib/batik-svg-dom.jar
	jar xf $^

org/apache/batik/dom/GenericDOMImplementation.class: batik-1.6/lib/batik-dom.jar
	jar xf $^

org/apache/batik/ext/awt/g2d/AbstractGraphics2D.class: batik-1.6/lib/batik-awt-util.jar
	jar xf $^

org/w3c/dom/svg/SVGSVGElement.class: batik-1.6/lib/batik-ext.jar
	jar xf $^

org/apache/batik/css/engine/CSSContext.class: batik-1.6/lib/batik-css.jar
	jar xf $^

compile-stamp: src/graphlayout/*.java src/graphlayout/*/*.java
	javac -d . src/graphlayout/*.java src/graphlayout/*/*.java
	touch $@ 

.PHONY: clean mrproper

clean: 
	rm -rf org xerces-2_9_0 batik-1.6
	rm -f compile-stamp

mrproper: clean
	rm -rf batik*.zip  Xerces*.zip xerces-2_9_0/

