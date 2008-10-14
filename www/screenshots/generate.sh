#! /bin/sh

exec >x-screenshot.html

inheader=yes
while read l; do
    test $inheader = yes && echo "$l" 
    (echo "$l" | grep -q "shots start here") && inheader=no
done < screenshots.html


grep shot: screenshots.html | sed 's/<!--//;s/-->//' | 
while read tmp file comment; do
#echo using $file with commment $comment
    d=$(dirname $file)
    f=$(basename $file)
#    echo $d/$f
    convert -resize 200  $d/$f $d/thumb-$f 
    echo -n '<p><a href="'$d/$f'"><img src="'$d/thumb-$f'" width="'
    identify $d/thumb-$f | cut -d" " -f 3 | cut -dx -f1 | (read a; echo -n $a)
    echo -n '" height="' 
    identify $d/thumb-$f | cut -d" " -f 3 | cut -dx -f2 | (read a; echo -n $a)
    alt=$(echo "$comment" | sed 's/<[^>]*>//g')
    echo '" alt="'$alt'" /><br /> '$comment'</a></p>'

done


inheader=yes
while read l; do
    test $inheader = no && echo "$l" 
    (echo "$l" | grep  "shots end here") && inheader=no
done < screenshots.html

mv x-screenshot.html screenshots.html

