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
This script creates a demo tag using the `ggtag` API:
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
# `badge.py`
Creates a badge-like tag for a GitHub user. Some examples are [octocat](https://ggtag.io/?i=%5Cr10%2C25%2C110%2C110%5CI15%2C30%2C100%2C100%2Chttps%3A%2F%2Favatars.githubusercontent.com%2Fu%2F583231%3Fv%3D4%5Ct140%2C50%2C5%2CThe%20Octocat%5Ct140%2C80%2C2%2Cgithub.com%2Foctocat%5Ca13%2C156%2C16%2Cmap-marker-alt%5Ct33%2C158%2C2%2CSan%20Francisco%5Ca13%2C183%2C16%2Cbuilding%5Ct33%2C185%2C2%2C%40github%5Ca180%2C154%2C16%2Clink%5Ct202%2C158%2C2%2Chttps%3A%2F%2Fgithub.blog%5Ca180%2C185%2C16%2Cenvelope%5Ct202%2C185%2C2%2Coctocat%40github.com), [antirez](https://ggtag.io/?i=%5Cr10%2C25%2C110%2C110%5CI15%2C30%2C100%2C100%2Chttps%3A%2F%2Favatars.githubusercontent.com%2Fu%2F65632%3Fv%3D4%5Ct140%2C40%2C5%2CSalvatore%5Ct140%2C70%2C5%2CSanfilippo%5Ct140%2C110%2C2%2Cgithub.com%2Fantirez%5Ca13%2C156%2C16%2Cmap-marker-alt%5Ct33%2C158%2C2%2CCatania%2CSicily%2CItaly%5Ca13%2C183%2C16%2Cbuilding%5Ct33%2C185%2C2%2CRedis%20Labs%5Ca180%2C154%2C16%2Clink%5Ct202%2C158%2C2%2Chttp%3A%2F%2Finvece.org%5Ca180%2C185%2C16%2Cenvelope%5Ct202%2C185%2C2%2Cantirez%40gmail.com), [ggerganov](https://ggtag.io/?i=%5Cr10%2C25%2C110%2C110%5CI15%2C30%2C100%2C100%2Chttps%3A%2F%2Favatars.githubusercontent.com%2Fu%2F1991296%3Fv%3D4%5Ct140%2C40%2C5%2CGeorgi%5Ct140%2C70%2C5%2CGerganov%5Ct140%2C110%2C2%2Cgithub.com%2Fggerganov%5Ca13%2C156%2C16%2Cmap-marker-alt%5Ct33%2C158%2C2%2CSofia%2C%20Bulgaria%5Ca13%2C183%2C16%2Cbuilding%5Ct33%2C185%2C2%2C%40viewray-inc%20%5Ca180%2C154%2C16%2Clink%5Ct202%2C158%2C2%2Chttps%3A%2F%2Fggerganov.com%5Ca180%2C185%2C16%2Cenvelope%5Ct202%2C185%2C2%2Cggerganov%40gmail.com), [rgerganov](https://ggtag.io/?i=%5Cr10%2C25%2C110%2C110%5CI15%2C30%2C100%2C100%2Chttps%3A%2F%2Favatars.githubusercontent.com%2Fu%2F271616%3Fv%3D4%5Ct140%2C40%2C5%2CRadoslav%5Ct140%2C70%2C5%2CGerganov%5Ct140%2C110%2C2%2Cgithub.com%2Frgerganov%5Ca13%2C156%2C16%2Cmap-marker-alt%5Ct33%2C158%2C2%2CSofia%2C%20Bulgaria%5Ca13%2C183%2C16%2Cbuilding%5Ct33%2C185%2C2%2C%40vmware%5Ca180%2C154%2C16%2Clink%5Ct202%2C158%2C2%2Chttps%3A%2F%2Fxakcop.com%5Ca180%2C185%2C16%2Cenvelope%5Ct202%2C185%2C2%2Crgerganov%40gmail.com).

You need to create a [personal access token](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token) on GitHub and export it as `GITHUB_TOKEN` environment variable.
```
$ export GITHUB_TOKEN=<your-github-token>
$ ./badge.py <github-username>
# You can replace the profile picture with QR code, so you can program the tag with sound:
$ ./badge.py --qrcode <github-username>
```
# `img.py`
Creates a tag with PNG image specified either as local file or URL:
```
# Create a tag with PNG image specified as local file
$ ./img.py mario.png
# Create a tag with PNG image specified as URL
$ ./img.py 'https://ggtag.io/mario.png'
```
