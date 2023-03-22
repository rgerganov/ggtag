Here are some examples which demonstrate how to use the Python bindings for `ggtag`.

If you are on Linux, you may need to install some prerequisites for the `ggwave` library:

```
sudo apt install portaudio19-dev python3-pyaudio
```

Then install all dependencies (preferably into venv) with `pip`:

```
pip install -r requirements.txt
```

# `demo.py`
This scripts creates a demo tag using the `ggtag` API:
```python
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
```
To program the tag with sound use:
```
$ ./demo.py sound
```
To program the tag with USB serial use:
```
$ ./demo.py serial
```
To open the tag in a browser:
```
$ ./demo.py browser
```
To save the tag in PNG and ASCII:
```
$ ./demo.py save
```
