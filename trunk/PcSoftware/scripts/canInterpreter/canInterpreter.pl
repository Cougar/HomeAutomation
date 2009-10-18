#!/usr/bin/perl -w
use IO::Socket;
use integer;
use threads;

@class = ();		#Klassid
@class_txt = ();	#Klassnamn
@class_layout = ();	#Bitlayout för id-fältet för resp klass
@class_layout_txt = (); #Namn på fälten i id
@class_type_txt = ();	#textöversättningar för olika fält i id ( dela av med ';' för olika fält, ',' mellan översättningspar och ':' mellan 
			# txt och siffra

# Paketklasser:
push (@class , "0x00");
push (@class_txt , "CAN_NMT");
push (@class_layout , "1,8,8,8");
push (@class_layout_txt , "Reserved,TYPE,SID,RID"); 
push (@class_type_txt, " 
;0x00:TIME,0x04:RESET,0x08:BIOS_START,0x0c:PGM_START,0x10:PGM_DATA,0x14:PGM_END,0x18:PGM_COPY,0x1c:PGMACK,0x20:PGM:NACK,0x24:START_APP,0x28:APP_START,0x2c:HEARTBEAT; 
; 
");

push (@class , "0x02");
push (@class_txt , "CAN_SNS");
push (@class_layout , "10,6,9");
push (@class_layout_txt , "SNS_TYPE,SNS_ID,SID");
push (@class_type_txt, "0x10:STATUS,0x12:IR,0x14:TEMPERATUR,0x18:LIGHT,0x0b:RELAY_STATUS,0x0d:SERVO_STATUS; ; ");

push (@class , "0x04");
push (@class_txt , "CAN_ACT");
push (@class_layout , "10,6,9");
push (@class_layout_txt , "ACT_TYPE,ACT_ID,SID");
push (@class_type_txt, "0x01:RELAY,0x02:SERVO,0x05:PWM,0x06:PID,0x03:RGBLED; ; ");

push (@class , "0x06");
push (@class_txt , "CAN_PKT");
push (@class_layout , "25");
push (@class_layout_txt , "DATA");
push (@class_type_txt, " ");

push (@class , "0x08");
push (@class_txt , "CAN_CON");
push (@class_layout , "25");
push (@class_layout_txt , "DATA");
push (@class_type_txt, " ");

push (@class , "0x0f");
push (@class_txt , "CAN_TST");
push (@class_layout , "25");
push (@class_layout_txt , "DATA");
push (@class_type_txt, " ");

push (@class , "0x05");
push (@class_txt , "CAN_ACT_CONF");
push (@class_layout , "10,6,9");
push (@class_layout_txt , "ACT_TYPE,ACT_ID,SID");
push (@class_type_txt, " ;0x05:PWM,0x06:PID; ");

print "---------------------------------\n";
print "CAN-Interpreter 0.1\n";
print "Makes the output from canDaemon easy to read\n";
print "---------------------------------\n";

print "Initiating sockets to canDaemon...";
$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => "deep.arune.se",
                    PeerPort => "1200",
                )
               or die " FAIL: cannot connect to port 1200 at localhost";

print "Success!\n";

print "Checking the length of the class arrays...";
#get the lengths of the class arrays:
$class = @class;
$class_txt = @class_txt;
$class_layout = @class_layout;
$class_layout_txt = @class_layout_txt;
$class_type_txt = @class_type_txt;
if ($class != $class_txt || $class != $class_layout || $class != $class_layout_txt || $class != $class_type_txt ) {
        print "All class vectors must be same length\n";
        exit;
}

print "Success!\n";
@curr_class_layout = ();
@curr_class_layout_txt = ();
#read line from socket (blocking)
while ( $line = <$remote> ) {
	if (length($line) > 12) {
		$class_found = 0;			#used to print raw data if the packet type isn't defined
		$id = hex(substr($line, 4, 8));		#id as a hex string
		$bin_id = sprintf ("%.29b", $id);	#id as a binary string
		$pkt_class = sprintf("%#.2x" , oct("0b".substr($bin_id , 0 , 4))); #first four bits as hex
		if ($pkt_class eq "00") {	#Fix to make class id 0x00 work
			$pkt_class = "0x00";
		}
		for ($i = 0; $i < $class; $i++) {
			if ($class[$i] eq $pkt_class) {		#determine class
				$class_found = 1;
				@curr_class_layout = split (',',$class_layout[$i]);
				@curr_class_layout_txt = split (',',$class_layout_txt[$i]);
				@curr_class_type_txt = split (';',$class_type_txt[$i]);
				print "Klass: ".$class_txt[$i]." >";
				$id_index = 4;
				for ($j = 0; $j < @curr_class_layout ; $j++) {	#replaces hex numbers with the text defined in 
										# $class_type_txt
					@id_field_txt = split (',' , $curr_class_type_txt[$j]);
					$data = sprintf("%#.2x" , oct("0b".substr($bin_id , $id_index , $curr_class_layout[$j])));
					for ($n = 0; $n < @id_field_txt; $n++) {
						#print $id_field_txt[$n]."\n";
						@field = split(':' , $id_field_txt[$n]);
						if ($data eq $field[0]) {
							$data = $field[1];
							#break;
						}
					}				
					$id_index = $id_index + $curr_class_layout[$j];
					print " ".$curr_class_layout_txt[$j].": ".$data;
				}
				print " Data: ".substr($line, 17, length($line)-18)."\n";
			}
		}
		if ($class_found == 0) {
			print $line;
		}
	}
}
