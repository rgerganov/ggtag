import base64
import cggtag
import urllib.parse
import webbrowser

class GGTag(object):
    """GGTag is a Python wrapper for the ggtag C library. It provides a simple
    interface for creating and rendering tags. The tag can be rendered as a
    bitmap or as a byte array. The byte array can be programmed into a ggtag
    device using either ggwave or pyserial.
    """
    def __init__(self, width=360, height=240):
        self._width = width
        self._height = height
        self._text_cmd = ""

    def _check_xy(self, x, y):
        """Check that the given x and y coordinates are in range."""
        if x < 0 or x >= self._width:
            raise ValueError("x must be in range [0, {})".format(self._width))
        if y < 0 or y >= self._height:
            raise ValueError("y must be in range [0, {})".format(self._height))

    def text(self, x, y, font_size, text):
        """Add text at position (x, y) using the given font size."""
        self._check_xy(x, y)
        self._text_cmd += "\\t{},{},{},{}".format(x, y, font_size, text)

    def circle(self, x, y, radius):
        """Draw circle at position (x, y) with the given radius."""
        self._check_xy(x, y)
        self._text_cmd += "\\c{},{},{}".format(x, y, radius)

    def rect(self, x, y, width, height):
        """Draw rectangle at position (x, y) with the given width and height."""
        self._check_xy(x, y)
        self._text_cmd += "\\r{},{},{},{}".format(x, y, width, height)

    def line(self, x1, y1, x2, y2):
        """Draw line from (x1, y1) to (x2, y2)."""
        self._check_xy(x1, y1)
        self._text_cmd += "\\l{},{},{},{}".format(x1, y1, x2, y2)

    def qrcode(self, x, y, size, text):
        """Place QR code at position (x, y) with the given size and text."""
        self._check_xy(x, y)
        self._text_cmd += "\\q{},{},{},{}".format(x, y, size, text)

    def image(self, x, y, width, height, rgba_image):
        """Draw image at position (x, y) with the given width and height.
        The image is given as a byte array of RGBA values in row-major order.
        """
        self._check_xy(x, y)
        bitmap = cggtag.dither(rgba_image, width, height)
        image = base64.b64encode(bitmap).decode("utf-8")
        self._text_cmd += "\\i{},{},{},{},{}".format(x, y, width, height, image)

    def image_url(self, x, y, width, height, url):
        """Place the image from the given URL at position (x, y) with the given
        width and height."""
        self._check_xy(x, y)
        self._text_cmd += "\\I{},{},{},{},{}".format(x, y, width, height, url)

    def icon(self, x, y, height, icon_name):
        """Place the icon with the given name at position (x, y) with the given
        height. Refer to https://fontawesome.com/v5/cheatsheet/free/solid for
        a list of available icons."""
        self._check_xy(x, y)
        self._text_cmd += "\\a{},{},{},{}".format(x, y, height, icon_name)

    def rfid(self, id1, id2, is_em4102=True):
        """Program the tag to function as an RFID tag with the given IDs."""
        if is_em4102:
            self._text_cmd += "\\fem,{},{}".format(id1, id2)
        else:
            self._text_cmd += "\\fhid,{},{}".format(id1, id2)

    def render(self):
        """Render the tag and return the bitmap as a 2D array of 0s and 1s."""
        bitmap = cggtag.render(self._text_cmd, self._width, self._height)
        bw = self._width // 8
        result = [[0 for _ in range(self._width)] for _ in range(self._height)]
        for row in range(self._height):
            for col in range(self._width):
                byte = bitmap[row * bw + (col // 8)]
                bit = 0x80 >> (col % 8)
                result[row][col] = 1 if byte & bit else 0
        return result

    def browse(self, host='https://ggtag.io'):
        """Open the tag in a web browser."""
        url = "{}/?i={}".format(host, urllib.parse.quote(self._text_cmd, safe=''))
        webbrowser.open(url)

    def __bytes__(self):
        """Return the tag as a byte array. The result can be programmed into a
        ggtag device using either ggwave or pyserial."""
        return cggtag.encode(self._text_cmd)

    def __str__(self):
        """Return the tag as a string."""
        return self._text_cmd
