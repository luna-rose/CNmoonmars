#!/usr/bin/expect -f
set USERNAME [lindex $argv 0]
set CORNSERVER [lindex $argv 1]
set SERVER [lindex $argv 2]
set PASSWORD [lindex $argv 3]
set STARTINDEX [lindex $argv 4] 
set ENDINDEX [lindex $argv 5]
set DIRECTORY [lindex $argv 6]

spawn ssh $USERNAME@$CORNSERVER

expect {
	-nocase "Password: " {
		send "$PASSWORD\r"
	}
	"yes/no" {
		send "yes\r"
		exp_continue
	}
}
expect "~>"

send "tmux\r"
expect "~>"

send "ssh $USERNAME@$SERVER\r"
expect {
	-nocase "Password: " {
		send "$PASSWORD\r"
	}
	"yes/no" {
		send "yes\r"
		exp_continue
	}
	"~>" {}
}
expect "~>"

send "cd /farmshare/user_data/$USERNAME/$DIRECTORY\r"
expect "/farmshare/user_data/$USERNAME/$DIRECTORY"

send "./cnmoonmars.sh -gridRes 100 -startIndex $STARTINDEX -endIndex $ENDINDEX\r"
expect "Precomputed"

send "\002"
send "d"

expect "~>"

send "logout"