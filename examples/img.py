#!/usr/bin/env python3
import png
import ggtag
import urllib.parse
import urllib.request
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Creates tag with PNG image')
    parser.add_argument('img', type=str, help='URL or file name of the image', nargs='?', default='https://ggtag.io/mario.png')
    args = parser.parse_args()
    if args.img.startswith('http://') or args.img.startswith('https://'):
        img_url = args.img
        print('Downloading image from {}'.format(img_url))
        r = png.Reader(file=urllib.request.urlopen(img_url))
    else:
        img_file = args.img
        print('Reading image from {}'.format(img_file))
        r = png.Reader(filename=img_file)
    p = r.asRGBA()
    width, height, rows, metadata = p
    rgba = bytes()
    for row in rows:
        rgba += bytes(row)
    tag = ggtag.GGTag()
    tag.image(10, 10, width, height, rgba)
    tag.browse()
    print("Saving tag to ggtag.png")
    bitmap = tag.render()
    w = png.Writer(len(bitmap[0]), len(bitmap), greyscale=True, bitdepth=1)
    f = open('ggtag.png', 'wb')
    w.write(f, bitmap)
    f.close()
