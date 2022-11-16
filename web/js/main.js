"use strict";

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
        // wait 20sec max after each read()
        let timerId = setTimeout(() => reader.cancel(), 20000);
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
    let data = new Uint8Array(Module.HEAPU8.buffer, ptr, length);
    console.log("Encoded data: " + data);

    if (audioContext == null) {
        audioContext = new AudioContext({sampleRate: 48000});
    }
    if (ggwave == null) {
        ggwave = await ggwave_factory();
        let parameters = ggwave.getDefaultParameters();
        parameters.payloadLength = 16;
        parameters.sampleRateInp = audioContext.sampleRate;
        parameters.sampleRateOut = audioContext.sampleRate;
        parameters.operatingMode   = ggwave.GGWAVE_OPERATING_MODE_TX | ggwave.GGWAVE_OPERATING_MODE_USE_DSS
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
            var buffer = audioContext.createBuffer(1, buf.length, audioContext.sampleRate);
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