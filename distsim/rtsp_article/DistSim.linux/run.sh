#!/bin/sh -e

PLANT=Plant
SIMINPUT=CommandGenerator
CONTROLLER=Controller
SIMULATION_CONTROLLER=../RTSP.linux/TkController/TkController

do_stop() {
	if pidof $1 > /dev/null 2>&1 ; then
		kill -9 `pidof $1`
	fi
}

do_start_simulation_controller() {
	if pidof $1 > /dev/null 2>&1 ; then
		echo "Simulation controller $1 is already running"
	else
		$1 $2 &
  fi
}

do_start_federate() {
	if pidof $1 > /dev/null 2>&1 ; then
		echo "Federate $1 is already running"
	else
		gnome-terminal --title=$1 --geometry=40x12 --execute ./$1 &
  fi
}
case "$1" in
start)
	do_start_simulation_controller $SIMULATION_CONTROLLER DistSim.mdf
	do_start_federate $PLANT
	do_start_federate $SIMINPUT
	do_start_federate $CONTROLLER
	;;

stop)
  do_stop $PLANT
	do_stop $SIMINPUT
	do_stop $CONTROLLER
	do_stop $SIMULATION_CONTROLLER
	;;
*)
	echo "Usage: run.sh {start|stop}"
	exit 1
	;;
esac

