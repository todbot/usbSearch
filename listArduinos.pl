#!/usr/bin/perl -w
#
# listArduinos.pl -- look for Arduino (UNOs) and print out useful information
#                    about them like VID/PID and serial port name
#                    For Mac OS X only at the moment
#
# 
# http://github.com/todbot/usbSearch/
#
# 2012, Tod E. Kurt, http://todbot.com/blog/
#
#

my $debug = 0;

my $output = `/usr/sbin/system_profiler SPUSBDataType`;

my @lines = split "\n", $output;


print "Finding Arduino UNOs...\n";

#
sub make_portname {
    my ($location_id) = @_;
    my $base = substr( $location_id, 2,4 );
    return "/dev/tty.usbmodem" . $base . "1";            
}

my $look_state = 0;
my $arduinos = {};
my $arduino = {};
foreach my $line (@lines) {
    chomp($line);
    print "line: '$line'\n" if( $debug>1 );
    $line =~ s/\s+/ /;
    if( $look_state == 0 ) { 
        if( $line =~ /Arduino Uno/ ) {
            $look_state = 1;
        }
    }
    elsif( $look_state == 1 ) { 
        if( $line =~ /^$/ ) {
            $look_state = 2;
        }
    }
    elsif( $look_state == 2 ) {
        if( $line =~ /Serial Number: (.+)$/ ) {
            $arduino->{'serial_num'} = $1;
        }
        if( $line =~ /Location ID: (.+)$/ ) { 
            $arduino->{'location_id'} = $1;
        }
        if( $line =~ /Product ID: (.+)$/ ) { 
            $arduino->{'pid'} = $1;
        }
        if( $line =~ /Vendor ID: (.+)$/ ) { 
            $arduino->{'vid'} = $1;
        }
        if( $line =~ /^$/ ) {
            $look_state = 0;
            $arduino->{'portname'} = make_portname($arduino->{'location_id'});
            $arduinos->{ $arduino->{'serial_num'} } = $arduino;
            $arduino = {}; # reset
        }
    }
}

my @serial_nums = keys %$arduinos;
my $count = scalar @serial_nums;
foreach my $serial (@serial_nums) { 
    my $ard = $arduinos->{$serial};
    printf("%s - %s/%s - %s\n", 
           $ard->{'portname'},
           $ard->{'vid'}, 
           $ard->{'pid'},
           $ard->{'serial_num'}
        );
}

print "Found ". $count ." Arduino UNO". (($count==1)?"\n":"s\n");

