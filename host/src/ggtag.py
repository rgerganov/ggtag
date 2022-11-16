#!/usr/bin/env python3
import argparse
import serial

def compile(input):
    """ Compile the given input to ggtag commands """
    ret = input.encode('ascii')
    l = len(ret)
    if l > 255:
        raise Exception('input too long')
    ret = l.to_bytes(1, byteorder='little') + ret
    return ret

def read_output(ser):
    """ Read pending output from ggtag and print it to stdout"""
    while True:
        line = ser.readline()
        if not line:
            break
        print(line.decode('ascii').strip())

def wait_done(ser):
    """ Wait for the 'Done.' message with some timeout """
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

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Program ggtag over serial connection',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--port', default='/dev/ttyACM0', help='serial port')
    parser.add_argument('input', type=str, help='input string')
    args = parser.parse_args()
    data = compile(args.input)
    ser = serial.Serial(args.port, 115200, timeout=1)
    read_output(ser)
    ser.write(data)
    wait_done(ser)
