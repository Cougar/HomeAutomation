#              device name   high    low     ext     boot start protocol device name
my @device0 = ('unknown',    '',     '',     '',     '',        'N/A');
my @device1 = ('atmega8',    '0xd4', '0x24', '',     '0x1e00',  'mega8');
my @device2 = ('atmega48',   '',     '',     '',     '',        'mega48');
my @device3 = ('atmega88',   '0xdc', '0xe2', '0x04', '0x1e00',  'mega88');
my @device4 = ('atmega168',  '0xdc', '0xe2', '0x02', '0x3c00',  'mega168');
my @device5 = ('atmega328p', '0xdc', '0xe2', '0x04', '0x7c00',  'mega328');

@devices = (\@device0, \@device1, \@device2, \@device3, \@device4, \@device5);

