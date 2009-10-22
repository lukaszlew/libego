#!/usr/bin/env tclsh


proc log {msg} {
    set tme [clock format [clock seconds] -format "%H:%M:%S"]
    puts  "$tme    $msg" 
}


set cgos_server {}
set cgos_port {}


proc platform {} {
    global tcl_platform
    set plat [lindex $tcl_platform(os) 0]
    set mach $tcl_platform(machine)
    switch -glob -- $mach {
	sun4* { set mach sparc }
	intel -
	i*86* { set mach x86 }
	"Power Macintosh" { set mach ppc }
    }
    switch -- $plat {
	AIX   { set mach ppc }
	HP-UX { set mach hppa }
    }
    return "$plat-$mach"
}



# ----------------------------------------------------------------
# each client should send some string to identify it
# should not have line feeds and may be trucated if unusually long
# ----------------------------------------------------------------
set clientInfo "cgosGtp 0.98 alpha - engine client for CGOS [platform] by Don Dailey"


# Make older tcl versions compatible with 8.5 by
# defining the lassign proc if it doesn't exist
# ------------------------------------------------------
if {[info procs lassign] eq ""} {
    proc lassign {values args} {
	uplevel 1 [list foreach $args [linsert $values end {}] break]
	lrange $values [llength $args] end
    }
}



proc usage {} {
    global argv0
    global clientInfo

    puts stderr "\n$clientInfo\n"
    puts stderr "Usage: $argv0  -c CONFIG_FILE  -k KILL_FILE  -p PLAYER_FIRST -s"
    puts stderr {
       -c specified name of config file - this is MANDATORY
       -k sepcifies a file to create which will stop client after current game
       -p specifies which player plays first game
       -s display a sample configuration file to stdout and exit        
    }
}


proc sample {} {

    puts {
# config file for testing various version of MyBot
# ------------------------------------------------

%section server
    server cgos.boardspace.net
    port 6867

%section player
     name      MyBot-1.0
     password  abcxyz
     invoke    ./mybot-1.0  --gtp --log mybot.log
     priority  7

%section player
     name      MyBot-2.0
     password  abcxyz
     invoke    ./mybot-2.0  --gtp --log mybot.log
     priority  3
    }
}



set sentinel {}
set first {}
set config_file {}
set sample 0


# command line argument parsing
# -----------------------------
if {[llength $argv] == 0} {
    usage
    exit 1
} else {

    set varname {}
    foreach v $argv {
	# Process argument to previous command line switch
        if { $varname != "" } {
            set $varname $v
            set varname ""
        }
	# For switches with arguments, save the variable name
	# to save the next command line argument to.
        if { $v == {-k} } { set varname sentinel }
        if { $v == {-p} } { set varname first }
        if { $v == {-c} } { set varname config_file }
        if { $v == {-s} } { set sample 1 }
    }

}

if { $sample } { sample ; exit 1}
if { $config_file == "" } { usage ; exit 1}



proc readConfigFile {} {

    global players
    global cgos_server
    global cgos_port
    global config_file

    set players {}

    
    set err [catch {set f [open $config_file]}]
    
    if $err {
        puts stderr "Cannot locate file: $cfile"
        exit 1
    }

    # parse config file
    # -----------------
    set expect {}
    set name ""
    set password ""
    set invoke ""
    set priority ""
    set totpr 0
    array set tokens {server 1 port 1 name 1 password 1 invoke 1 priority 1}
    
    set line 0
    while { [gets $f s] >= 0 } {
        
        incr line 
        
        if { [regexp {^%section\s+(\S+)} $s dmy sname] } {

						# This is the start of a new section
						# Record settings from previous section
						# Save results in "expect" variable for later post-processing
            
            if { $expect == "player" } {
                lappend players [list $name $password $invoke $priority]
                incr totpr $priority
            }
            
            if { $expect == "server" } {
                set cgos_server $server
                set cgos_port   $port
            }
            
            set expect $sname

						# Avoid using settings from past section in future section(s)
						array set tokens {server 1 port 1 name 1 password 1 invoke 1 priority 1}
            
        } else {
            
            set s [string trim $s]
            
            set tok [lindex [split $s] 0]
            
            if { [info exists tokens($tok)] } {
                if { [regexp {^\S+\s+(.+)} $s dmy val] } {
                    set $tok $val
                } else {
                    puts stderr "Improperly formatted configuration file line: $line"
                    exit 1
                }
            }
        }
    }

    # Process final section
    # Note: this is a repeat of code inside loop
    
    if { $expect == "player" } {
        lappend players [list $name $password $invoke $priority]
        incr totpr $priority
    }
    
    if { $expect == "server" } {
        set cgos_server $server
        set cgos_port   $port
    }

    if { $totpr <= 0 } {
        puts stderr "\nPriority values do not allow any players on server.  Aborting ...\n"
        exit 1
    }
    
    puts "\nServer: $cgos_server"
    puts "  Port: $cgos_port\n"
    
    puts "PLAYER              PRIORITY"
    puts "------------------  --------"
    foreach s $players {
        lassign $s na pa ik pr
        if { [string is integer $pr] && $pr >= 0 } {
            set pr [expr (100.0 * $pr) / $totpr]
            puts [format "%-18s  %7.1f%%" $na $pr]
        } else {
            puts stderr "Illegal format for priority.  Must be integer >= 0\n"
            exit 0
        }
    }
    
    puts ""

    close $f
}


proc openPlayer {name password invoke} {

    global inout
    global progpid
    global use_time

    set inout [open "|$invoke" r+]
        
    set progpid [pid $inout]
    
    fconfigure $inout -buffering line
    
    
    log  "C->E list_commands"
    puts $inout "list_commands"
    
    while { [gets $inout s] >= 0 } { 

        set s [string trim $s]
        if {[regexp {\=\s+(\S+)} $s dmy val]} {
            set cmd $val
        } else {
            set cmd $s
        }
        log  "E->C $cmd"


        if {$cmd == ""} { break }        
        set known($cmd) 1
    } 
    
    log "recieved full response to list_commands"
    
    
    # determine if engine supports gtp timing commands
    # ------------------------------------------------
    set use_time 0
    
    if {[info exists known(time_left)] && [info exists known(time_settings)]} {
        set use_time 1
    }
    
    if {$use_time} {
        log  "Engine uses  time control commands"
    } else {
        log  "Engine does NOT use time control commands"
    }
}




# ------------------------------------------------------------------------- 
# This procuedure does the following:
#   re-read the config file
#   choose a player, honoring the priority assignments probabilistically
#   if different from previous player, close connection and start a new one
#
#   returns 1 if player are switched from previous round
# ------------------------------------------------------------------------- 

proc switchPlayer {who} {
    global curplayer
    global players
    global inout
    global name
    global password

    set    totpr 0
    set    choice {}

    readConfigFile

    # sum priorities
    foreach p $players {
        lassign $p name password invoke priority
        incr totpr $priority
        if {$name == $who} {
            set choice $who
            break 
        }
    }

    if { $who != "" && $choice != $who } {
        puts stderr "Name \"$who\" does not exist in configuration file.  Aborting ...\n"
        exit 1
    }


    if {$name != $who} {
        set totpr [expr $totpr + 0.0]
        
        set rnum [expr rand()]
        set r 0.0
        foreach p $players {
            lassign $p name password invoke priority
            set choice $name
            set r [expr $r + ($priority / $totpr)]
            if {$rnum < $r} {
                break
            }
        }
    }


    if { $choice == $curplayer } { 
        puts "Same player: $choice\n"
        cgos "ready"    ;# tell server we are ready for another game
        return 0
    } else {
        puts "New player: $choice\n"

        # log curplayer off of cgos
        # -------------------------
        
        if { $curplayer != "" } {
            catch {puts $inout quit}
            after 1000
            catch {close $inout}
        }
        set curplayer $choice
        openPlayer $name $password $invoke
        return 1
    }
}


set curplayer ""
set players {}
readConfigFile   ;# get server and port



# -----------------------------------------------------
# timeout in seconds 
# Don't make this too low - no need to hammer the server
# 
#   When CGOS is restarted it will always wait 45 seconds
#   before scheduling the first round - to allow all the
#   clients time to log back on.   So 30 seconds is
#   a good suggested value for timeout.
# ------------------------------------------------------
set server_retry_timeout  30    


set time_out [expr $server_retry_timeout * 1000]






# ---------------------------------
# send a message and get a response 
# from channel ch
# ---------------------------------
proc  eCmd { ch msg } {
    global  progpid

    log  "C->E $msg"
    puts $ch $msg
    gets $ch r1
    gets $ch r2

    log "E->C $r1"

    if { [string index $r1 0] == "?" } {
	log "Error. Engine returns error error response.  Aborting"
	exit 1
    }

    return $r1
}


# send a message to the cgos server
# ---------------------------------
proc cgos {msg} {
    global   sock

    log "C->S $msg"
    set err [catch {puts $sock $msg}]
}




# setup server restart loop
# -------------------------

set switchedPlayers 0
set firstPass 1

while {1} {

    set err [catch {set sock [socket $cgos_server $cgos_port]} msg]

    if { $err } {
	log "Server startup return code: $err   msg: $msg"
	log "Cannot connect to server.  Will retry shortly"
	after $time_out  ;# Wait several seconds 
	continue
    } else {
	log "Successful connection to CGOS server"
    }

    fconfigure $sock -buffering line

    if { $firstPass == 1 } { 
        set switchedPlayers [switchPlayer $first]
        set first {}
        set firstPass 0
    }
    
    while {[gets $sock s]} {

	if { [regexp {^(\S+)} $s dmy cmd] } {

	    log "S->C $s"
	    
	    switch -exact $cmd {
		
		info { 
		    # informational mssage from CGOS only
		    # -----------------------------------
		    log [string range $s 5 end] 
		}  
		
		protocol { cgos "e1 $clientInfo" }
		username { cgos $name }
		password { cgos $password  }
		Error:   { break }

		setup {
		    set mvlist [lassign $s ignore gid boardsize komi timeLeft white black]

		    set msg [eCmd $inout "boardsize $boardsize"]
		    set msg [eCmd $inout "clear_board"]
		    set msg [eCmd $inout "komi $komi"]
		    
		    if {$use_time} {
			set  tme [expr $timeLeft / 1000]
			set msg [eCmd $inout "time_settings $tme 0 0"]
		    }

		    # catch up game if needed
		    # -----------------------
		    set ctm B 
		    foreach {mv tme} $mvlist {
			set msg [eCmd $inout "play $ctm $mv"]
			if {$ctm == "B"} { set ctm W } else { set ctm B }
		    }

		    # setup requires no response - we are done
		}


		genmove {
		    lassign $s dmy col remaining_time

		    if {$use_time} {
			set tme [expr $remaining_time / 1000]
			set msg [eCmd $inout "time_left $col $tme 0"]
		    }
		    
		    set msg [eCmd $inout "genmove $col"]

                    set mv "crash"
		    regexp {=\s+(\S+)} $msg dmy mv

                    if { $mv == "crash" } {
                        log "Apparently, engine crashed"
                        exit 1
                    }
		    cgos $mv
		}

		gameover {
                    # does user wants to exit?
                    if { $sentinel != "" } {
                        if {[file exists $sentinel]} {
                            file delete $sentinel
                            log "Sentinel file detected.  Aborting"
                            puts $inout quit  ;# shutdown engine (and don't expect a response)
                            after 1000        ;# give engine time to shutdown - does this help?
                            exit 0
                        }
                    }

		    lassign $s res dte err
                    set switchedPlayers [switchPlayer $first]
                    if { $switchedPlayers == 1 } {
                        break
                    }
		}
		

		play {
		    lassign $s dmy col mv tme

		    # some engines do not understand "resign"
		    if { $mv != "resign" } {
			set msg [eCmd $inout "play $col $mv"]
		    }
		}

		
		default { 
		    log  "We do not understand server request."
		    log  "Perhaps you need to update your client?"
		    log  "Exit"
		    eCmd $inout "quit"
		    exit 1
		}
	    }
	} else {
	    log "Irrecgular response from server. Breaking connection."
	    break  ;# something funny here from server.
	}
	
    }


    # if switchedPlayers == 0, then socket probably closed on us.
    #    in this case, try to restart with same player
    # -----------------------------------------------------------

    if { $switchedPlayers == 0 }  {
        log  "Connection to server has closed.  Will try to reconnect shortly." 
        after $time_out  ;# Wait a few seconds 
    } else {
        cgos quit
        after 1000
        catch {close $socket}
    }
}
