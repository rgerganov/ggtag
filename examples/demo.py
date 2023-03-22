#!/usr/bin/env python3
import png
import ggtag
import ggwave
import pyaudio
import webbrowser
import urllib.parse
import serial
import argparse

# This is a workaround for the fact that ggwave expects a string-like object
class BytesWrapper:
    def __init__(self, data):
        self.data = data

    def encode(self):
        return self.data

# Create a demo tag
def create_tag():
    tag = ggtag.GGTag()
    tag.text(204, 122, 3, "ggtag")
    tag.icon(212, 81, 34, "wifi")
    tag.rect(198, 74, 70, 70)
    tag.rect(68, 74, 82, 82)
    tag.qrcode(71, 77, 3, "https://ggtag.io/demo.html")
    tag.text(43, 186, 5, "https://ggtag.io")
    return tag

# Program the tag using the ggwave protocol
def program_sound(tag):
    data = bytes(tag)
    params = ggwave.getDefaultParameters()
    params['payloadLength'] = 16
    # ggwave.GGWAVE_OPERATING_MODE_TX | ggwave.GGWAVE_OPERATING_MODE_USE_DSS = 20
    params['operatingMode'] = 20

    instance = ggwave.init(params)
    p = pyaudio.PyAudio()
    stream = p.open(format=pyaudio.paFloat32, channels=1, rate=48000, output=True, frames_per_buffer=1024)

    offset = 0
    while offset < len(data):
        chunk = data[offset:offset+16]
        waveform = ggwave.encode(BytesWrapper(chunk), protocolId=2, volume=10, instance=instance)
        stream.write(waveform, len(waveform)//4)
        offset += 16

    stream.stop_stream()
    stream.close()
    p.terminate()

# Program the tag using the serial protocol
def program_serial(tag, port):
    data = bytes(tag)
    ser = serial.Serial(port, 115200, timeout=1)
    # read pending output from ggtag and print it to stdout
    while True:
        line = ser.readline()
        if not line:
            break
        print(line.decode('ascii').strip())
    # send the data
    ser.write(data)
    # wait for the 'Done.' message with some timeout
    timeouts = 0
    while True:
        line = ser.readline()
        if timeouts >= 20:
            raise Exception('Rendering is taking too long')
        if not line:
            timeouts += 1
            continue
        line = line.decode('ascii').strip()
        print(line)
        if line == 'Done.':
            break
    ser.close()

# Open the tag in the browser
def open_in_browser(tag):
    text_cmds = str(tag)
    url = "https://ggtag.io/?i={}".format(urllib.parse.quote(text_cmds, safe=''))
    print("Opening URL: {}".format(url))
    webbrowser.open(url)

# Save the tag to a PNG file
def save_to_png(tag, fname):
    bitmap = tag.render()
    w = png.Writer(len(bitmap[0]), len(bitmap), greyscale=True, bitdepth=1)
    f = open(fname, 'wb')
    w.write(f, bitmap)
    f.close()

# Save the tag to an ASCII file
def save_to_ascii(tag, fname):
    bitmap = tag.render()
    f = open(fname, 'w')
    for row in bitmap:
        f.write(''.join([' ' if c else '*' for c in row]) + '\n')
    f.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Demonstration of ggtag')
    subparsers = parser.add_subparsers(dest='cmd')
    subparsers.add_parser('sound', help='Program demo tag with sound')
    parser_serial = subparsers.add_parser('serial', help='Program demo tag with USB serial')
    parser_serial.add_argument('-p', '--port', type=str, help='USB serial port', default='/dev/ttyACM0')
    subparsers.add_parser('browser', help='Open demo tag in browser')
    parser_save = subparsers.add_parser('save', help='Save demo tag in PNG and ASCII')
    parser_save.add_argument('-f', '--fname', type=str, help='file name prefix', default='ggtag')
    args = parser.parse_args()

    tag = create_tag()
    if args.cmd == 'sound':
        program_sound(tag)
    elif args.cmd == 'serial':
        program_serial(tag, args.port)
    elif args.cmd == 'browser':
        open_in_browser(tag)
    elif args.cmd == 'save':
        save_to_png(tag, args.fname + '.png')
        save_to_ascii(tag, args.fname + '.txt')
    else:
        parser.print_help()
