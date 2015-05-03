#! /usr/bin/tclsh

package require Tk

#val = IntVar( 0 )
frame .fr
pack .fr -fill both -expand 1
radiobutton .fr.a -text "Emergency Mode" -command onClick -variable val -value 1
radiobutton .fr.b -text "Normal Mode" -command onClick -variable val -value 0
radiobutton .fr.c -text "Stand alone Mode" -command onClick -variable val -value 2

#.fr.cb select 
#place .fr.cb -x 50 -y 50 

pack .fr.a -anchor w
pack .fr.b -anchor w
pack .fr.c -anchor w

proc onClick {} {

    global val

    if {$val==true} {
    
        wm title . radiobutton
    } else {
        wm title . ""
    }
}


wm title . radiobutton
wm geometry . 250x150+300+300
