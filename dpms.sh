#!/bin/bash

# watch for DMPS to be shut off by dumb programs
# Make sure we turn it back on to "Save the Trees" (tm)

PROGS="mplayer xine"
DISPLAY=":0"

#----

XSET="xset -display $DISPLAY"
LOG="/dev/null"

function dpms_on {
  if $XSET q | grep -q "DPMS is Disabled"; then
    echo "dpms on" >> $LOG
    $XSET +dpms
  fi
  if $XSET q | grep -q "Standby: 0"; then
    echo "dpms timeout" >> $LOG
    $XSET dpms 500 530 600 
  fi
}

### Main Loop

flag=1

for p in $PROGS ; do
   pids=`pgrep $p`
   echo "Check: $p" >> $LOG
   if [ "$pids" ]  ; then
	echo "Pids found: $pids" >> $LOG
	flag=0
   fi
done

if [ $flag == 1 ] ; then
  # No nasty progs, clean up their mess
  echo "No nasty progs, clean up their mess" >> $LOG
  dpms_on
fi

echo >> $LOG
