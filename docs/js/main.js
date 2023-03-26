"use strict";

const ESC2CMD = {"\\t": "Text",
                 "\\r": "Rect",
                 "\\R": "FillRect",
                 "\\c": "Circle",
                 "\\C": "FillCircle",
                 "\\l": "Line",
                 "\\q": "QR code",
                 "\\I": "Image",
                 "\\a": "Icon",
                 "\\f": "RFID"}

const CMD2ESC = {"Text":       "\\t",
                 "Rect":       "\\r",
                 "FillRect":   "\\R",
                 "Circle":     "\\c",
                 "FillCircle": "\\C",
                 "Line":       "\\l",
                 "QR code":    "\\q",
                 "Image":      "\\I",
                 "Icon":       "\\a",
                 "RFID":       "\\f"}

var dragging = false;
// keeps the initial position when dragging starts
var startPos = { x:0, y:0 };
// keeps the intial position of the elements being dragged
var initialXYMap = {};

function splitCommands(input) {
    let cmds = [];
    let lastIdx = 0;
    for (let i = 1; i < input.length - 1; i++) {
        if (input[i] == "\\") {
            if (input[i + 1] != "\\") {
                cmds.push(input.substring(lastIdx, i));
                lastIdx = i;
            } else {
                i++;
            }
        }
    }
    cmds.push(input.substring(lastIdx));
    return cmds;
}

function onRuntimeInitialized()
{
    const params = new Proxy(new URLSearchParams(window.location.search), {
        get: (searchParams, prop) => searchParams.get(prop),
    });
    let value = params.s;
    if (value === "s") {
        changeSize(document.getElementById("smallRadio"), false);
        document.getElementById("smallRadio").checked = true;
    } else if (value === "l") {
        changeSize(document.getElementById("largeRadio"), false);
        document.getElementById("largeRadio").checked = true;
    }
    // take the input from the URL and split it into commands
    let input = params.i;
    if (input) {
        let cmds = splitCommands(input)
        let addCount = cmds.length - 1;
        let firstCmd = $('#cmdContainer').children()[0];
        for (let i = 0; i < addCount; i++) {
            let newCmd = $(firstCmd).clone()
            newCmd.find(".dropdown-item").click(onCmdChange);
            newCmd.find(".fa-plus").parent().click(onAdd);
            newCmd.find(".fa-trash").parent().click(onDelete);
            newCmd.find("input[type=text]").keypress(onKeypress);
            newCmd.find("input[type=text]").focusout(repaint);
            newCmd.insertAfter($(firstCmd));
        }
        for (let i = 0; i < cmds.length; i++) {
            let cmd = ESC2CMD[cmds[i].substring(0, 2)];
            let text = cmds[i].substring(2);
            text = text.replace(/\\\\/g, "\\");
            let row = $('#cmdContainer').children()[i];
            $(row).find("button").text(cmd);
            $(row).find("input[type=text]").val(text);
        }
    }
    repaint();
}

function getInput() {
    let inp = "";
    $('#cmdContainer').children().each(function () {
        let cmd = $(this).find("button").text();
        let text = $(this).find("input[type=text]").val();
        text = text.replace(/\\/g, "\\\\");
        inp += CMD2ESC[cmd] + text;
    });
    return inp;
}

async function repaint() {
    let inp = getInput();
    inp = await preprocessImages(inp);
    //console.log(inp);
    render(inp);
}

function randomInt(min, max) {
    // min and max included
    return Math.floor(Math.random() * (max - min + 1) + min)
}

function randomWord() {
    let words = ["foo", "bar", "spam", "eggs", "Hello", "ggtag"];
    return words[randomInt(0, words.length - 1)];
}

function randomIcon() {
    let codepoints = ["car-side", "bug", "ambulance", "crown", "gas-pump", "poo"];
    return codepoints[randomInt(0, codepoints.length - 1)];
}

function onCmdChange() {
    let newCmd = $(this).text();
    let curr = $(this).parent().parent().parent();
    let prevCmd = $(curr).find("button").text();
    let prevText = $(curr).find("input[type=text]").val();
    if (newCmd != prevCmd) {
        let xyCoord = "";
        let remaining = "";
        let prevParts = prevText.split(",");
        if (newCmd != "RFID") {
            let x = randomInt(0, 100);
            let y = randomInt(0, 100);
            if (prevParts.length > 1 && !isNaN(prevParts[0]) && !isNaN(prevParts[1])) {
                // keep the old coordinates if possible
                x = parseInt(prevParts[0]);
                y = parseInt(prevParts[1]);
            }
            xyCoord = x + "," + y + ",";
        }
        if (newCmd == "Text") {
            let fontNum = randomInt(1, 5);
            remaining = fontNum + "," + randomWord();
        } else if (newCmd == "Circle" || newCmd == "FillCircle") {
            if (prevCmd == "Circle" || prevCmd == "FillCircle") {
                // keep the old radius
                remaining = prevParts[2];
            } else {
                // random radius
                remaining = randomInt(10, 50);
            }
        } else if (newCmd == "Rect" || newCmd == "FillRect") {
            if (prevCmd == "Rect" || prevCmd == "FillRect") {
                // keep the old width and height
                remaining = prevParts[2] + "," + prevParts[3];
            } else {
                // random width and height
                remaining = randomInt(20, 60) + "," + randomInt(20, 60);
            }
        } else if (newCmd == "Line") {
            // random end point
            remaining = randomInt(0, 100) + "," + randomInt(0, 100);
        } else if (newCmd == "QR code") {
            remaining = randomInt(1, 4) + "," + randomWord();
        } else if (newCmd == "Image") {
            remaining = "0,0,https://xakcop.com/doomface.png";
        } else if (newCmd == "Icon") {
            remaining = randomInt(16,40) + "," + randomIcon();
        } else if (newCmd == "RFID") {
            remaining = "em,12,3456789A";
        }
        $(curr).find("button").text(newCmd);
        $(curr).find("input[type=text]").val(xyCoord+remaining);
        repaint();
    }
}

function onDelete() {
    let currRow = $(this).parent().parent().parent();
    if (currRow.parent().children().length > 1) {
        currRow.remove();
    }
    repaint();
}

function onAdd() {
    let currRow = $(this).parent().parent().parent();
    let currText = $(currRow).find("input[type=text]").val();
    // clone the current row and change the (X,Y) coordinates (if any)
    let parts = currText.split(",");
    if (parts.length > 1 && !isNaN(parts[0]) && !isNaN(parts[1])) {
        parts[0] = randomInt(0, 100);
        parts[1] = randomInt(0, 100);
    }
    let newRow = $(currRow).clone()
    newRow.find(".dropdown-item").click(onCmdChange);
    newRow.find(".fa-plus").parent().click(onAdd);
    newRow.find(".fa-trash").parent().click(onDelete);
    newRow.find("input[type=text]").val(parts.join(","));
    newRow.find("input[type=text]").keypress(onKeypress);
    newRow.find("input[type=text]").focusout(repaint);
    newRow.insertAfter($(currRow));
    repaint();
}

function onKeypress(event) {
    if (event.keyCode == 13) {
        repaint();
    }
}

async function onShare() {
    let inp = getInput();
    let size = "l";
    if (document.getElementById("smallRadio").checked) {
        size = "s";
    }
    let url = window.location.origin + window.location.pathname + "?i=" + encodeURIComponent(inp) + "&s=" + size;
    if (navigator.share) {
        try {
            let shareData = {
                title: 'ggtag',
                text: 'ggtag',
                url: url,
            };
            await navigator.share(shareData);
        } catch (e) {
            console.log('Share failed: ' + e)
        }
    } else if (navigator.clipboard) {
        navigator.clipboard.writeText(url);
    }
}

function changeSize(radio, doRepaint=true)
{
    let canvas = document.getElementById("ggCanvas");
    if (radio.value == "small") {
        canvas.width = 250;
        canvas.height = 122;
    } else if (radio.value == "large") {
        canvas.width = 360;
        canvas.height = 240;
    }
    if (doRepaint) {
        repaint();
    }
}

function render(input)
{
    const canvas = document.getElementById("ggCanvas");
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    let ptr = Module.ccall('render', 'number', ['string', 'number', 'number'], [input, canvas.width, canvas.height]);
    let errPtr = Module.ccall('getLastError', 'number', [], []);
    let jsStr = Module.UTF8ToString(errPtr);
    if (jsStr !== "OK") {
        $("#errMsg").text(jsStr);
        $("#errDiv").show();
    } else {
        $("#errDiv").hide();
    }
    if (ptr == 0) {
        return;
    }
    let width = Math.ceil(canvas.width / 8);
    let height = canvas.height;
    let totalBytes = width * height;
    let buf = new Uint8Array(Module.HEAP8.buffer, ptr, totalBytes);

    let imgData = ctx.createImageData(canvas.width, canvas.height);

    for (let row = 0; row < canvas.height; row++) {
        for (let col = 0; col < canvas.width; col++) {
            let pixel = buf[row*width + Math.floor(col/8)] & (0x80 >> (col % 8));
            let offset = (row * canvas.width + col) * 4;
            imgData.data[offset] = pixel ? 255 : 0;
            imgData.data[offset+1] = pixel ? 255 : 0;
            imgData.data[offset+2] = pixel ? 255 : 0;
            imgData.data[offset+3] = 255;
        }
    }

    ctx.putImageData(imgData, 0, 0);
    Module._free(ptr);
}

// Get the position of the mouse relative to the canvas
function getMousePos(canvasDom, event) {
    if (event.touches !== undefined && event.touches.length > 0) {
        event = event.touches[0];
    }
    var rect = canvasDom.getBoundingClientRect();
    return {
        x: Math.floor(event.clientX) - rect.left,
        y: Math.floor(event.clientY) - rect.top
    };
}

function onMouseDown(e) {
    dragging = true;
    let canvas = document.getElementById("ggCanvas");
    startPos = getMousePos(canvas, e);
    // save the initial (X,Y) coordinates of the selected commands
    $('#cmdContainer').find("input:checked").each(function(ind) {
        let currInput = $(this).parent().parent().parent().find("input[type=text]");
        let currText = $(currInput).val();
        let parts = currText.split(",");
        if (parts.length > 1 && !isNaN(parts[0]) && !isNaN(parts[1])) {
            // draggable command
            initialXYMap[ind] = { x: parseInt(parts[0]), y: parseInt(parts[1]) };
        } else {
            // non-draggable command (e.g. rfid)
            initialXYMap[ind] = null;
        }
    });
}

function onMouseUp(e) {
    dragging = false;
}

function onMouseMove(e) {
    if (dragging) {
        let canvas = document.getElementById("ggCanvas");
        let currPos = getMousePos(canvas, e);
        let dx = currPos.x - startPos.x;
        let dy = currPos.y - startPos.y;
        $('#cmdContainer').find("input:checked").each(function(ind) {
            if (initialXYMap[ind] == null) {
                return;
            }
            let currInput = $(this).parent().parent().parent().find("input[type=text]");
            let currText = $(currInput).val();
            let parts = currText.split(",");
            if (parts.length > 1) {
                parts[0] = initialXYMap[ind].x + dx;
                if (parts[0] > canvas.width) {
                    parts[0] = parts[0] % canvas.width;
                } else if (parts[0] < 0) {
                    parts[0] = canvas.width + parts[0];
                }
                parts[1] = initialXYMap[ind].y + dy;
                if (parts[1] > canvas.height) {
                    parts[1] = parts[1] % canvas.height;
                } else if (parts[1] < 0) {
                    parts[1] = canvas.height + parts[1];
                }
                $(currInput).val(parts.join(","));
            }
        });
        repaint();
    }
}

async function readSerialOutput(port)
{
    const textDecoder = new TextDecoderStream();
    const readableStreamClosed = port.readable.pipeTo(textDecoder.writable);
    const reader = textDecoder.readable.getReader();
    let result = false;
    while (true) {
        // wait 30sec max after each read()
        let timerId = setTimeout(() => reader.cancel(), 30000);
        const { value, done } = await reader.read();
        clearTimeout(timerId);
        if (done) {
          // timeout occurred and reader.cancel() has been called.
          break;
        }
        console.log(">>" + value);
        if (value.startsWith("Done.")) {
            result = true;
            reader.cancel();
            break;
        }
    }
    reader.releaseLock();
    await readableStreamClosed.catch(() => { /* Ignore the error */ });
    return result;
}

async function programSerial(input)
{
    console.log("Programming over serial port");
    let lengthPtr = Module._malloc(4);
    let ptr = Module.ccall('encode', 'number', ['string', 'number'], [input, lengthPtr]);
    if (ptr == 0) {
        Module._free(lengthPtr);
        return;
    }
    let length = Module.getValue(lengthPtr, 'i32');
    console.log("Encoded data length: " + length);
    Module._free(lengthPtr);
    let data = new Uint8Array(Module.HEAPU8.buffer, ptr, length);
    console.log("Encoded data: " + data);

    let port = {};
    if ("serial" in navigator) {
        // use the WebSerial API
        // filter by usb vendorId and productId
        port = await navigator.serial.requestPort({ filters: [{ usbVendorId: 0x2e8a, usbProductId: 0x000a }] });
    } else {
        // WebSerial over WebUSB (works on Android)
        port = await exports.serial.requestPort();
    }
    await port.open({ baudRate: 115200 });
    const closedPromise = readSerialOutput(port);

    const writer = port.writable.getWriter();
    await writer.write(data);
    writer.releaseLock();

    let result = await closedPromise;
    await port.close();
    console.log("All done, result: " + result);
    Module._free(ptr);
}

let ggwave = null;
ggwave_factory().then(function(obj) {
    ggwave = obj;
});
let audioContext = null;
let ggwaveInstance = null;

async function programSound(input)
{
    console.log("Programming over sound");
    let lengthPtr = Module._malloc(4);
    let ptr = Module.ccall('encode', 'number', ['string', 'number'], [input, lengthPtr]);
    if (ptr == 0) {
        Module._free(lengthPtr);
        return;
    }
    let length = Module.getValue(lengthPtr, 'i32');
    console.log("Encoded data length: " + length);
    Module._free(lengthPtr);
    if (length > 256) {
        Module._free(ptr);
        throw "The maximum data length when using sound is 256 bytes";
    }
    let data = new Uint8Array(Module.HEAPU8.buffer, ptr, length);
    console.log("Encoded data: " + data);

    if (audioContext == null) {
        let AudioContext = window.AudioContext // Default
           || window.webkitAudioContext // Safari and old versions of Chrome
           || false;
        if (AudioContext) {
            audioContext = new AudioContext({sampleRate: 48000});
        } else {
            Module._free(ptr);
            throw("Web Audio API is not supported by your browser");
        }
        let parameters = ggwave.getDefaultParameters();
        parameters.payloadLength = 16;
        parameters.sampleRateInp = audioContext.sampleRate;
        parameters.sampleRateOut = audioContext.sampleRate;
        parameters.operatingMode   = ggwave.GGWAVE_OPERATING_MODE_TX | ggwave.GGWAVE_OPERATING_MODE_USE_DSS
        // this must be done only once
        ggwaveInstance = ggwave.init(parameters);
    }

    let promiseResolve;
    const promise = new Promise(resolve => {
        promiseResolve = resolve;
    });

    let offset = 0;
    let tx = function() {
        if (offset < data.length) {
            // generate audio waveform
            let waveform = ggwave.encode(ggwaveInstance, data.slice(offset, offset+16), ggwave.ProtocolId.GGWAVE_PROTOCOL_AUDIBLE_FASTEST, 10)
            let buf = new Float32Array(waveform.buffer, waveform.byteOffset, waveform.length/Float32Array.BYTES_PER_ELEMENT);
            let buffer = audioContext.createBuffer(1, buf.length, audioContext.sampleRate);
            buffer.getChannelData(0).set(buf);
            let source = audioContext.createBufferSource();
            source.buffer = buffer;
            source.connect(audioContext.destination);
            source.addEventListener('ended', tx);
            source.start(0);
            offset += 16;
        } else {
            promiseResolve();
        }
    }
    tx();
    await promise;
    console.log("All done");
    Module._free(ptr);
}

function bitmapToBase64(bitmap)
{
    var base64 = "";
    for (var i = 0; i < bitmap.length; i++) {
        base64 += String.fromCharCode(bitmap[i]);
    }
    return btoa(base64);
}

const loadImage = (url) => new Promise((resolve, reject) => {
    const img = new Image();
    img.crossOrigin = "Anonymous";
    img.addEventListener('load', () => resolve(img));
    img.addEventListener('error', (err) => reject(err));
    img.src = url;
});

async function preprocessImages(input)
{
    const canvas = document.getElementById("ggCanvas");
    const ctx = canvas.getContext("2d", {willReadFrequently: true});
    // find all image escape sequences "\I<x>,<y>,<w>,<h><url>"
    let regex = /\\I(\d+),(\d+),(\d+),(\d+),([^\\]+)/g;
    let match = null;
    while ((match = regex.exec(input)) !== null) {
        try {
            var img = await loadImage(match[5]);
        } catch (e) {
            console.log("Failed to load image: " + match[5]);
            continue;
        }
        let x = match[1];
        let y = match[2];
        let w = match[3];
        let h = match[4];
        if (w == 0) {
            w = img.width;
        }
        if (h == 0) {
            h = img.height;
        }
        ctx.drawImage(img, x, y, w, h);
        let imgData = ctx.getImageData(x, y, w, h);
        let rgbaArr = imgData.data;
        let rgbaBuf = Module._malloc(rgbaArr.length*rgbaArr.BYTES_PER_ELEMENT);
        if (rgbaBuf == 0) {
            console.log("Failed to allocate memory for image: " + match[5]);
            continue;
        }
        Module.HEAPU8.set(rgbaArr, rgbaBuf);
        // get dithered bitmap from RGBA image
        let ditheredPtr = Module.ccall('dither', 'number', ['number', 'number', 'number'], [rgbaBuf, imgData.width, imgData.height]);
        if (ditheredPtr == 0) {
            console.log("Failed to dither image: " + match[5]);
            Module._free(rgbaBuf);
            continue;
        }
        Module._free(rgbaBuf);
        let ditheredLength = Math.ceil(canvas.width * canvas.height / 8);
        let ditheredBitmap = new Uint8Array(Module.HEAP8.buffer, ditheredPtr, ditheredLength);
        let base64 = bitmapToBase64(ditheredBitmap);
        Module._free(ditheredPtr);
        // replace with "\i<x>,<y>,<width>,<height>,<base64_encoded_bitmap>"
        input = input.replace(match[0], `\\i${x},${y},${w},${h},${base64}`);
    }
    //console.log(input);
    return input;
}