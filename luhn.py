#!/usr/bin/python
'''
Enter 16 digit card number to verify luhn checksum.
Or Enter 15 digit card number to compute luhn checksum.

Ex:
./luhn.py 4147202126396546
Input number: 4147202126396546
True

./luhn.py 414720212639654
Input number: 414720212639654
6
'''

import sys

def checkCC(cc):
    check_num = int(cc[-1])

    return calculateLuhn(cc[:-1]) == check_num

def calculateLuhn(cc):
    a = [int(i) for i in cc]

    for i in range(0,len(a),2):
        a[i] *= 2
        if a[i] >= 10:
            a[i] -= 9

    return 10-sum(a)%10

cc = sys.argv[1]
print "Input number: %s" % (cc) 

if len(cc) == 16:
    print checkCC(cc)
elif len(cc) == 15:
    print calculateLuhn(cc)
else:
    print "Input needs to be 15 or 16 digits long. %d provided" % (len(cc))
    sys.exit(1)


