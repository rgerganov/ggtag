"use strict";

function onRuntimeInitialized()
{
    const params = new Proxy(new URLSearchParams(window.location.search), {
        get: (searchParams, prop) => searchParams.get(prop),
    });
    let value = params.s;
    if (value === "s") {
        changeSize(document.getElementById("smallRadio"));
        document.getElementById("smallRadio").checked = true;
    } else if (value === "l") {
        changeSize(document.getElementById("largeRadio"));
        document.getElementById("largeRadio").checked = true;
    }
    let input = params.i;
    if (input) {
        document.getElementById("inp").value = decodeURI(input);
    }
    document.getElementById("preview").click();
}

async function onShare() {
    let inp = document.getElementById("inp").value;
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
        // TODO: show tooltip indicating that the link was copied to clipboard
        navigator.clipboard.writeText(url);
    }
}

function changeSize(radio)
{
    let canvas = document.getElementById("ggCanvas");
    if (radio.value == "small") {
        canvas.width = 250;
        canvas.height = 122;
    } else if (radio.value == "large") {
        canvas.width = 360;
        canvas.height = 240;
    }
    document.getElementById("preview").click();
}

function render(input)
{
    const canvas = document.getElementById("ggCanvas");
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    let ptr = Module.ccall('render', 'number', ['string', 'number', 'number'], [input, canvas.width, canvas.height]);
    if (ptr == 0) {
        throw 'render() failed';
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
        throw 'encode() failed';
    }
    let length = Module.getValue(lengthPtr, 'i32');
    console.log("Encoded data length: " + length);
    Module._free(lengthPtr);
    let data = new Uint8Array(Module.HEAPU8.buffer, ptr, length);
    console.log("Encoded data: " + data);

    let port = {};
    if ("serial" in navigator) {
        // use the WebSerial API
        port = await navigator.serial.requestPort();
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
        throw 'encode() failed';
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

function imageDataToBitmap(imgData)
{
    var bitmap = new Uint8Array(Math.ceil(imgData.width * imgData.height / 8));
    for (var i = 0; i < imgData.data.length; i += 4) {
        var bit = 0;
        if (imgData.data[i] < 128) {
            bit = 1;
        }
        var byte = Math.floor(i / 4 / 8);
        var bitInByte = 7 - ((i / 4) % 8);
        bitmap[byte] |= bit << bitInByte;
    }
    return bitmap;
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
    // find all image escape sequences "\I<x>,<y>,<url>"
    let regex = /\\I(\d+),(\d+),([^\\]+)/g;
    let match = null;
    while ((match = regex.exec(input)) !== null) {
        try {
            var img = await loadImage(match[3]);
        } catch (e) {
            console.log("Failed to load image: " + match[3]);
            continue;
        }
        let x = match[1];
        let y = match[2];
        ctx.drawImage(img, x, y);
        let imgData = ctx.getImageData(x, y, img.width, img.height);
        let bitmap = imageDataToBitmap(imgData);
        let base64 = bitmapToBase64(bitmap);
        // replace with "\i<x>,<y>,<width>,<height>,<base64_encoded_bitmap>"
        input = input.replace(match[0], `\\i${x},${y},${img.width},${img.height},${base64}`);
    }
    console.log(input);
    return input;
}