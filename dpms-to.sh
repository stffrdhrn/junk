#!/bin/bash

# watch for DMPS to be shut off by dumb programs
# Make sure we turn it back on to "Save the Trees" (tm)

TIMEOUT=$1
DISPLAY=":0"

#----

XSET="xset -display $DISPLAY"
LOG="/dev/null"

function dpms_on {
  to=$1 ; shift

  tos=`expr $to \* 60`

  if $XSET q | grep -q "DPMS is Disabled"; then
    echo "dpms on" >> $LOG
    $XSET +dpms
  fi
  if ! $XSET q | grep -q "Standby: $tos"; then
    echo "dpms timeout $tos" >> $LOG
    $XSET dpms $tos $tos $tos
  fi
}

dpms_on $TIMEOUT
echo >> $LOG
