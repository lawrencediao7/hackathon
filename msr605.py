import time
import serial
import re
import codecs
import sys

class MSRException(Exception):
    pass

class ReadError(MSRException):
    pass

class ReadWriteError(MSRException):
    pass

class CommandFormatError(MSRException):
    pass

class InvalidCommand(MSRException):
    pass

class InvalidCardSwipeForWrite(MSRException):
    pass

class SetError(MSRException):
    pass

class MSR605(serial.Serial):
    ESC_CHR = '\x1B'
    FS_CHR = '\x1C'

    TRACK_SENTINELS = (('%', '?'), (';', '?'), (';', '?'))

    def __init__(self, dev, test=True):
        super(MSR605, self).__init__(dev, 9600, 8, serial.PARITY_NONE, timeout=10)
        self.reset()
        if test:
            self.communication_test()
            self.ram_test()
            self.sensor_test()
            self.reset()

    def _read_status(self):
        exceptions = {
            '\x31': ReadWriteError,
            '\x32': CommandFormatError,
            '\x34': InvalidCommand,
            '\x39': InvalidCardSwipeForWrite,
            '\x41': SetError,
        }
        self._expect(self.ESC_CHR)
        status = self.read(1)
        print status
        if status in exceptions:
            raise exceptions[status]()
        return status

    def _expect(self, data):
        read_data = self.read(len(data))
        if read_data != data:
            raise ReadError('Expected %s, got %s.' % (repr(data), repr(read_data)))

    def _read_until(self, end):
        data = ''
        while True:
            data += self.read(1)
            if data.endswith(end):
                return data

    def _send_command(self, command, *args):
        self.flushInput()
        self.flushOutput()
        self.write(self.ESC_CHR + command + ''.join(args))

    def all_leds_off(self):
        self._send_command('\x81')

    def all_leds_on(self):
        self._send_command('\x82')

    def led_green_on(self):
        self._send_command('\x83')

    def led_yellow_on(self):
        self._send_command('\x84')

    def led_red_on(self):
        self._send_command('\x85')

    def sensor_test(self):
        self._send_command('\x86')
        if self.read(2) != (self.ESC_CHR + '\x30'):
            raise MSRException('Sensor test failed.')

    def communication_test(self):
        self._send_command('\x65')
        if self.read(2) != (self.ESC_CHR + '\x79'):
            raise MSRException('Communication test failed.')

    def ram_test(self):
        self._send_command('\x87')
        if self.read(2) != (self.ESC_CHR + '\x30'):
            raise MSRException('RAM test failed.')

    def reset(self):
        self._send_command('\x61')

    def read_raw(self):
        def read_tracks():
            for tn in xrange(1, 4):
                self._expect(self.ESC_CHR + chr(tn))
                str_len = ord(self.read(1))
                yield self.read(str_len)
        self._send_command('\x6D')
        self._expect(self.ESC_CHR + '\x73')
        tracks = tuple(get_tracks())
        self._expect('\x3F' + self.FS_CHR)
        self._read_status()
        return tracks

    def get_device_model(self):
        self._send_command('\x74')
        self._expect(self.ESC_CHR)
        model = self.read(1)
        self._expect('S')
        return model

    def get_firmware_version(self):
        self._send_command('\x76')
        self._expect(self.ESC_CHR)
        return self.read(8)

    def set_hico(self):
        self._send_command('\x78')
        self._expect(self.ESC_CHR + '\x30')

    def set_lowco(self):
        self._send_command('\x79')
        self._expect(self.ESC_CHR + '\x30')

    def get_co_status(self):
        self._send_command('\x79')
        self._expect(self.ESC_CHR)
        return self.read(1)

    def set_leading_zero(self, t13, t2):
        self._send_command('\x7A', chr(t13), chr(t2))
        self._read_status()

    def check_leading_zero(self):
        self._send_command('\x6C')
        self._expect(self.ESC_CHR)
        t13 = ord(self.read(1))
        t2 = ord(self.read(1))
        return t13, t2

    def erase_card(self, t1=True, t2=True, t3=True):
        flags = (t1 and 1 or 0) | (t2 and 2 or 0) | (t3 and 4 or 0)
        self._send_command('\x63', chr(flags))
        self._read_status()

    def select_bpi(self, t1_density, t2_density, t3_density):
        self._send_command('\x62', t1_density and '\xD2' or '\x4B')
        self._read_status()
        self._send_command('\x62', t2_density and '\xA1' or '\xA0')
        self._read_status()
        self._send_command('\x62', t3_density and '\xC1' or '\xC0')
        self._read_status()

    def set_bpc(self, t1, t2, t3):
        self._send_command('\x6F', chr(t1), chr(t2), chr(t3))
        self._expect(self.ESC_CHR + '\x30' + chr(t1) + chr(t2) + chr(t3))

    def _get_raw_data(self, *tracks):
        assert len(tracks) == 3
        raw_data_block = self.ESC_CHR + '\x73'
        for tn, track in enumerate(tracks):
            raw_data_block += \
                self.ESC_CHR +\
                chr(tn + 1) +\
                chr(len(track)) +\
                track
        raw_data_block += '\x3F' + self.FS_CHR
        return raw_data_block     

    def write_raw(self, *tracks):
        raw_data_block = _get_raw_data(self)
        self._send_command('\x6E', raw_data_block)
        self._read_status()

    def _set_iso_mode(self):
        self.select_bpi(True, False, True)
        self.set_bpc(7, 5, 5)
        self.set_leading_zero(61, 22)

    def write_iso(self, *tracks):
        soft = False	
        print tracks
        assert len(tracks) == 3
        if soft:
            return self._write_iso_soft(*tracks)
        return self._write_iso_native(*tracks)

    def _clean_iso_track_data(self, tracks):
        print 'START'
        for sentinels, track in zip(self.TRACK_SENTINELS, tracks):
            print map(re.escape, sentinels)
            print tracks
            print re.sub(r'^%s|%s$' % tuple(map(re.escape, sentinels)), '', track)
        print 'END'
        return [
            re.sub(r'^%s|%s$' % tuple(map(re.escape, sentinels)), '', track)
            for sentinels, track in zip(self.TRACK_SENTINELS, tracks)
        ]

    def _write_iso_native(self, *tracks):
        print tracks
        tracks = self._clean_iso_track_data(tracks)
        data_block = self.ESC_CHR + '\x73'
        data_block += ''.join(
            self.ESC_CHR + chr(tn + 1) + track
            for tn, track in enumerate(tracks)
        )
        data_block += '\x3F' + self.FS_CHR
        self._send_command('\x77', self._get_raw_data(*tracks))
        self._read_status()

    def _write_iso_soft(self, *tracks):
        self._set_iso_mode()
        tracks = self._clean_iso_track_data(tracks)
        tracks = [
            (ss + track + es).encode('iso7811-2-track%d' % (tn + 1))
            for tn, ((ss, es), track) in enumerate(zip(self.TRACK_SENTINELS, tracks))
        ]
        return self.write_raw(*tracks)

    def read_iso(self, soft=False):
        if soft:
            return self._read_iso_soft()
        return self._read_iso_native()

    def _read_iso_native(self):
        self._send_command('\x72')
        self._expect(self.ESC_CHR + '\x73')
        self._expect(self.ESC_CHR + '\x01')
        track1 = self._read_until(self.ESC_CHR + '\x02')[:-2]
        track2 = self._read_until(self.ESC_CHR + '\x03')[:-2]
        track3 = self._read_until(self.FS_CHR)[:-1]
        self._read_status()
        return track1, track2, track3

    def _read_iso_soft(self):
        self._set_iso_mode()
        return [
            track.decode('iso7811-2-track%d' % (tn + 1))
            for tn, track in enumerate(self.read_raw())
        ]

    def read_track_1(self):
        """
        Read only the first track of data from the card. The return value of this
        function is a string of characters, where each character represents one
        pre-translated DEC six-bit value. For example:
            %B4147202126396546^DIAO/LAWRENCE ^1605101100001100000000849000000?

        Note: Nader wrote this.
        """
        self._send_command('\x72')
        self._expect(self.ESC_CHR + '\x73')
        self._expect(self.ESC_CHR + '\x01')
        track1 = self._read_until(self.ESC_CHR + '\x02')[:-2]
        return track1



class ISO7811_2(codecs.Codec):
    TRACK1_CHARS = ' !"#$%&\'()*+`,./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'
    TRACK23_CHARS = '0123456789:;<=>?'

    @classmethod
    def _reverse_bits(cls, value, nbits):
        return sum(
            1 << (nbits - 1 - i)
            for i in xrange(nbits)
            if (value >> i) & 1
        )

    @classmethod
    def _with_parity(cls, value, nbits):
        if sum(1 for i in xrange(nbits) if (value >> i) & 1) % 2 != 0:
            return value
        return value | (1 << (nbits - 1))

    @classmethod
    def _iso_encode_data(cls, data, mapping, nbits):
        def make_data():
            lrc = 0
            for v in map(mapping.index, data):
                lrc ^= v
                yield chr(cls._with_parity(v, nbits))
            yield chr(cls._with_parity(lrc, nbits))
        enc = ''.join(make_data())
        return enc, len(enc)

    @classmethod
    def _iso_decode_data(cls, data, mapping, nbits):
        dec = ''.join(
            mapping[cls._reverse_bits(ord(c) >> 1, nbits - 1)]
            for c in data
        )
        return dec, len(dec)

    @classmethod
    def encode_track1(cls, data):
        return cls._iso_encode_data(data, cls.TRACK1_CHARS, 7)

    @classmethod
    def encode_track23(cls, data):
        return cls._iso_encode_data(data, cls.TRACK23_CHARS, 5)

    @classmethod
    def decode_track1(cls, data):
        return cls._iso_decode_data(data, cls.TRACK1_CHARS, 7)

    @classmethod
    def decode_track23(cls, data):
        return cls._iso_decode_data(data, cls.TRACK23_CHARS, 5)

    @classmethod
    def codec_search(cls, name):
        return {
            'iso7811-2-track1': (cls.encode_track1, cls.decode_track1, None, None),
            'iso7811-2-track2': (cls.encode_track23, cls.decode_track23, None, None),
            'iso7811-2-track3': (cls.encode_track23, cls.decode_track23, None, None),
        }.get(name, None)

def parseTrack1(track1_str):
    if (len(track1_str) == 2):
        return (None, None, None, None, None)
    split = track1_str.split('^')

    cc = split[0][-16:]
    firstname, lastname = split[1].split('/')
    year_exp = split[2][0:2]
    month_exp = split[2][2:4]

    return cc, firstname, lastname, month_exp, year_exp

x = MSR605('COM6', test=False)

def callback(t1):
    track1 = x.read_track_1()
    cc, firstname, lastname, month_exp, year_exp = parseTrack1(track1)

    t1.delete("1.0", tk.END)
    #t2.delete("1.0", tk.END)
    #t3.delete("1.0", tk.END)
    #t4.delete("1.0", tk.END)
    t1.insert(tk.CURRENT, "cc: %s\nFirst Name: %s\nLast Name: %s\nExpiration Date: %s/%s" % (cc, firstname, lastname, month_exp, year_exp))
    #t2.insert(tk.CURRENT, "First Name: %s" % firstname)
    #t3.insert(tk.CURRENT, "Last Name: %s" % lastname)
    #t4.insert(tk.CURRENT, "%s/%s" % (month_exp, year_exp))

    x.reset()
    time.sleep(.1)

def parse_track_1_data(track1_str):
    """
    track1 is a string of characters pre-translated from the DEC six-bit encoding
    read in. For example:
        %B4147202126396546^DIAO/LAWRENCE ^1605101100001100000000849000000?
    This method should take in this string of characters and break it down into
    components and return a dictionary containing each component. Later on,
    we'll use the output of this function to display the data we read in from
    the card on a nice GUI.

    Note: Nader wrote this.
    """
    pass

codecs.register(ISO7811_2.codec_search)

import time

# This awesome little loop repeatedly reads in the first track and
# prints it out without erroring!
#
# XXX: Turn this into a GUI.

'''
str = '1010001010001100101010100101011010001101000001101001010110001010100101100110010011001010100010010101101000000100000010001001010111110001001110010101000011111101011110010011010100001111101100100110101001001110111100010101001000000010111110100010110101000000100000110110001010000100100010110001010000100000010000001000000100100010110001010000100000010000001000000100000010000001000000100000010000101010100101000110100001000000100000010000001000000100000010011111000001011000'
print len(str);
for i, b in enumerate(str):
    if i%7 == 0:
        sys.stdout.write("\n")
    sys.stdout.write(b + ", ")

'''

import Tkinter as tk


top = tk.Tk()
left = tk.Frame()
right = tk.Frame( height = 100)
right.grid(row=0, column=1)
right.grid_propagate(False)

left.pack(side = tk.LEFT)
right.pack(side = tk.LEFT)

t1 = tk.Text(master=right)

#t2 = tk.Text(master=right)
#t3 = tk.Text(master=right)
#t4 = tk.Text(master=right)
t1.pack()
#t2.pack()
#t3.pack()
#t4.pack()


tk.Button(left, text="Read CC", command= lambda: callback(t1)).pack()
tk.Button(left, text='Exit', command=top.destroy).pack()
top.mainloop()

'''
x = MSR605('COM6', test=False)
while True:
    track1 = x.read_track_1()
    cc, firstname, lastname, month_exp, year_exp = parseTrack1(track1)

    print ''
    print track1
    print "cc: %s" % cc
    print "First Name: %s" % firstname
    print "Last Name: %s" % lastname
    print "Expiration Date: %s/%s" % (month_exp, year_exp)

    x.reset()
    time.sleep(.1)
'''
#x.write_iso('%B4147202126396546^DIAO/LAWRENCE ^1605101100001100000000849000000?', ';4147202126396546=160510110000849?', '\x1b+?')