
def swap_dictionary(original_dict):
	return dict([(v, k) for (k, v) in original_dict.iteritems()]) 

def int2hex(number):
	return str("%X" % number)

int2bin = lambda n: n>0 and int2bin(n>>1)+str(n&1) or ''

def bin2hex(number):
	s = ''
	t = {	'0000':'0',
		'0001':'1',
		'0010':'2',
		'0011':'3',
		'0100':'4',
		'0101':'5',
		'0110':'6',
		'0111':'7',
		'1000':'8',
		'1001':'9',
		'1010':'a',
		'1011':'b',
		'1100':'c',
		'1101':'d',
		'1110':'e',
		'1111':'f'}
	c = 0
	while c < len(number)-1:
		s += t[number[c:c+4]]
		c += 4
		
	return s


def hex2bin(number):
	number = number.lower()
	s = ''
	t = {	'0':'0000',
		'1':'0001',
		'2':'0010',
		'3':'0011',
		'4':'0100',
		'5':'0101',
		'6':'0110',
		'7':'0111',
		'8':'1000',
		'9':'1001',
		'a':'1010',
		'b':'1011',
		'c':'1100',
		'd':'1101',
		'e':'1110',
		'f':'1111'}
	c = 0
	while c < len(number):
#		print "\thex2bin: " + number[c] + " - " + t[number[c]]
		s += t[number[c]]
		c += 1
		
	return s