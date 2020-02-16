nn=0
for i in `cat test.txt` ;  do 
  nn=`expr $nn + 1`
  gs=0
  found=0
  while [ $found != 1 ] ; do
    gs_md5=`echo -n $gs | md5sum | awk '{print $1}'`
    if [ $i == $gs_md5 ] ; then
       echo "s$nn: Found answer $i = $gs"
       found=1
    else
       if [ $gs -gt 100000 ] ; then
          echo "s$nn: Search too large $i > $gs"
	  found=1
       fi
       gs=`expr $gs + 1`
    fi
   done
 done

