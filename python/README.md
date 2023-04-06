Python bindings for `ggtag`.

# Installation

`pip install ggtag`

# Usage
The `GGTag` class provides a simple interface for creating and rendering tags. The tag can be rendered as a bitmap or encoded as byte array. 

Here is an example how to create a tag and encode it to byte array:

```python
tag = ggtag.GGTag()
tag.text(204, 122, 3, "ggtag")
tag.icon(212, 81, 34, "wifi")
tag.rect(198, 74, 70, 70)
tag.rect(68, 74, 82, 82)
tag.qrcode(71, 77, 3, "https://ggtag.io/demo.html")
tag.text(43, 186, 5, "https://ggtag.io")
data = bytes(tag)
```

The byte array can be programmed into a ggtag device using either:
 * data over sound and the the [ggwave](https://pypi.org/project/ggwave) library 
 * serial USB connection and the [pyserial](https://pypi.org/project/pyserial) library.

See [demo.py](https://github.com/rgerganov/ggtag/blob/master/examples/demo.py) for a complete demo.
