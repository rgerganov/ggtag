import cggtag

class GGTag(object):
    def __init__(self, width=360, height=240):
        self._width = width
        self._height = height
        self._text_cmd = ""

    def text(self, x, y, font_size, text):
        self._text_cmd += "\\t{},{},{},{}".format(x, y, font_size, text)

    def circle(self, x, y, radius):
        self._text_cmd += "\\c{},{},{}".format(x, y, radius)

    def rect(self, x, y, width, height):
        self._text_cmd += "\\r{},{},{},{}".format(x, y, width, height)

    def line(self, x1, y1, x2, y2):
        self._text_cmd += "\\l{},{},{},{}".format(x1, y1, x2, y2)

    def qrcode(self, x, y, size, text):
        self._text_cmd += "\\q{},{},{},{}".format(x, y, size, text)

    def image(self, x, y, width, height, image):
        self._text_cmd += "\\i{},{},{},{},{}".format(x, y, width, height, image)

    def icon(self, x, y, height, icon_name):
        self._text_cmd += "\\a{},{},{},{}".format(x, y, height, icon_name)

    def rfid(self, id1, id2, is_em4102=True):
        if is_em4102:
            self._text_cmd += "\\fem,{},{}".format(id1, id2)
        else:
            self._text_cmd += "\\fhid,{},{}".format(id1, id2)

    def render(self):
        bitmap = cggtag.render(self._text_cmd, self._width, self._height)
        bw = self._width // 8
        result = [[0 for _ in range(self._width)] for _ in range(self._height)]
        for row in range(self._height):
            for col in range(self._width):
                byte = bitmap[row * bw + (col // 8)]
                bit = 0x80 >> (col % 8)
                result[row][col] = 1 if byte & bit else 0
        return result

    def __bytes__(self):
        return cggtag.encode(self._text_cmd)

    def __str__(self):
        return self._text_cmd
