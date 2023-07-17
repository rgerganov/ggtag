let ggwave = null;
ggwave_factory().then(function(obj) {
    ggwave = obj;
});
let audioContext = null;
let ggwaveInstance = null;
let recorder = null;

// helper function
function convertTypedArray(src, type) {
    var buffer = new ArrayBuffer(src.byteLength);
    var baseView = new src.constructor(buffer).set(src);
    return new type(buffer);
}

function concatTypedArrays(a, b) { // a, b TypedArray of same type
    var c = new (a.constructor)(a.length + b.length);
    c.set(a, 0);
    c.set(b, a.length);
    return c;
}

function equalArrays(a, b) {
    if (a.length != b.length) {
        return false;
    }
    for (let i=0; i<a.length; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

function clearCanvas() {
    const canvas = document.getElementById("ggCanvas");
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
}

function render(recvBuffer) {
    const canvas = document.getElementById("ggCanvas");
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    let buffer = Module._malloc(recvBuffer.length);
    if (buffer == 0) {
        console.log("Failed to allocate memory");
        return;
    }
    Module.HEAPU8.set(recvBuffer, buffer);
    let ptr = Module.ccall('renderBuffer', 'number', ['number', 'number', 'number', 'number'], [buffer, recvBuffer.length*8, canvas.width, canvas.height]);
    if (ptr == 0) {
        Module._free(buffer);
        return;
    }
    Module._free(buffer);
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

function updateProgress(bytesReceived, totalBytes) {
    const progress = document.getElementById("progress");
    progress.style.width = (bytesReceived / totalBytes * 100) + "%";
}

function onStartListen() {
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
        parameters.operatingMode = ggwave.GGWAVE_OPERATING_MODE_RX | ggwave.GGWAVE_OPERATING_MODE_USE_DSS
        // this must be done only once
        ggwaveInstance = ggwave.init(parameters);
    }
    
    let constraints = {
        audio: {
            // not sure if these are necessary to have
            echoCancellation: false,
            autoGainControl: false,
            noiseSuppression: false
        }
    };
    let onSuccess = function(stream) {
        const mediaStream = audioContext.createMediaStreamSource(stream);
        var bufferSize = 1024;
        var numberOfInputChannels = 1;
        var numberOfOutputChannels = 1;
        if (audioContext.createScriptProcessor) {
            recorder = audioContext.createScriptProcessor(
                    bufferSize,
                    numberOfInputChannels,
                    numberOfOutputChannels);
        } else {
            recorder = audioContext.createJavaScriptNode(
                    bufferSize,
                    numberOfInputChannels,
                    numberOfOutputChannels);
        }
        let recvBuffer = new Uint8Array(0);
        let lastPacket = new Uint8Array(0);
        let expectedBytesCount = 0;
        recorder.onaudioprocess = function (e) {
            let source = e.inputBuffer;
            let tmp = ggwave.decode(ggwaveInstance, convertTypedArray(new Float32Array(source.getChannelData(0)), Int8Array));
            // ggwave returns Int8Array, but we need Uint8Array
            let res = new Uint8Array(tmp.buffer, tmp.byteOffset, tmp.length);

            if (res && res.length > 0) {
                console.log("pkt: " + res);
                if (res.length != 16) {
                    // should never happen
                    console.log("Unexpected buffer length: " + res.length);
                    return;
                }
                if (equalArrays(lastPacket, res)) {
                    // console.log("same packet received twice");
                    return;
                }
                lastPacket = res.slice();
                if (recvBuffer.length == 0) {
                    // first packet
                    expectedBytesCount = res[0]*256 + res[1];
                    console.log("expectedBytesCount: " + expectedBytesCount);
                    recvBuffer = res.slice(2);
                    clearCanvas();
                } else {
                    recvBuffer = concatTypedArrays(recvBuffer, res);
                }
                updateProgress(recvBuffer.length, expectedBytesCount);
                if (recvBuffer.length >= expectedBytesCount) {
                    render(recvBuffer);
                    recvBuffer = new Uint8Array(0);
                    expectedBytesCount = 0;
                }
            }
        }
        mediaStream.connect(recorder);
        recorder.connect(audioContext.destination);        
    }
    let onError = function(err) {
        console.log('The following error occured: ' + err);
    }
    navigator.mediaDevices.getUserMedia(constraints).then(onSuccess, onError);
    startListenButton.hidden = true;
    stopListenButton.hidden = false;
}

function onStopListen() {
    if (recorder) {
        recorder.disconnect(audioContext.destination);
        // todo: is this necessary?
        //mediaStream.disconnect(recorder);
        recorder = null;
    }    
    startListenButton.hidden = false;
    stopListenButton.hidden = true;
    updateProgress(0, 1);
}

// save the canvas as a PNG image
function onSaveTag() {
    const canvas = document.getElementById("ggCanvas");
    const link = document.createElement('a');
    link.download = 'ggtag.png';
    link.href = canvas.toDataURL()
    link.click();
}