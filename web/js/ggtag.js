"use strict";
function drawChar(ctx, font, ch, x, y)
{
    let offset = (ch.charCodeAt() - ' '.charCodeAt()) * font.height * Math.ceil(font.width / 8);
    for (let row = 0; row < font.height; row++) {
        for (let col = 0; col < font.width; col++) {
            if (font.data[offset] & (0x80 >> (col % 8))) {
                ctx.fillRect(x+col, y+row, 1, 1);
            }
            if (col % 8 == 7) {
                offset++;
            }
        }
        if (font.width % 8 != 0) {
            offset++;
        }
    }
}

function drawString(ctx, font, str, x, y)
{
    for (let i = 0; i < str.length; i++) {
        drawChar(ctx, font, str[i], x, y);
        x += font.width;
    }
}

const FONT_CMD = 0x00;
const FONT_1 = 0x01;
const FONT_2 = 0x02;
const FONT_3 = 0x03;
const SETX_CMD = 0x01;
const SETY_CMD = 0x02;
const TEXT_CMD = 0x03;

const fontMap = [null, Font16, Font20, Font24];

function parseCommand(inp)
{
    if (inp[0] !== '\\') {
        throw "Expected command";
    }
    switch (inp[1]) {
        case 'f':
            return [FONT_CMD, inp.slice(2)];
        case 'x':
            return [SETX_CMD, inp.slice(2)];
        case 'y':
            return [SETY_CMD, inp.slice(2)];
        case 't':
            return [TEXT_CMD, inp.slice(2)];
        default:
            throw "Unknown command";
    }
}

function parseFontArg(arg)
{
    if (arg.length == 0) {
        throw "Expected font number";
    }
    const font = arg[0];
    switch (font) {
        case '1':
            return [FONT_1, arg.slice(1)];
        case '2':
            return [FONT_2, arg.slice(1)];
        case '3':
            return [FONT_3, arg.slice(1)];
        default:
            throw "Unknown font";
    }
}

function parseTextArg(arg)
{
    if (arg.length == 0) {
        throw "Expected some text";
    }
    let text = "";
    let i = 0;
    while (i < arg.length) {
        if (arg[i] == '\\') {
            if (i+1 < arg.length && arg[i+1] == '\\') {
                text += '\\';
                i += 2;
            } else {
                break;
            }
        } else {
            text += arg[i];
            i++;
        }
    }
    return [text, arg.slice(i)];
}

function parseSetXArg(arg)
{
    let i = 0;
    while (i < arg.length && arg[i] >= '0' && arg[i] <= '9') {
        i++;
    }
    if (i == 0) {
        throw "Expected X coordinate";
    }
    let x = parseInt(arg.slice(0, i));
    if (x < 0 || x > 250) {
        throw "X coordinate out of range";
    }
    return [x, arg.slice(i)];
}

function parseSetYArg(arg)
{
    let i = 0;
    while (i < arg.length && arg[i] >= '0' && arg[i] <= '9') {
        i++;
    }
    if (i == 0) {
        throw "Expected Y coordinate";
    }
    let y = parseInt(arg.slice(0, i));
    if (y < 0 || y > 250) {
        throw "Y coordinate out of range";
    }
    return [y, arg.slice(i)];
}

function parse(input)
{
    let cmds = [];
    function parseInput(inp)
    {
        if (inp.length == 0) {
            return;
        }
        const [cmd, tail] = parseCommand(inp);
        switch (cmd) {
            case FONT_CMD:
                let [font, tailFont] = parseFontArg(tail)
                cmds.push([FONT_CMD, font]);
                parseInput(tailFont);
                break;
            case SETX_CMD:
                let [x, tailX] = parseSetXArg(tail)
                cmds.push([SETX_CMD, x]);
                parseInput(tailX);
                break;
            case SETY_CMD:
                let [y, tailY] = parseSetYArg(tail)
                cmds.push([SETY_CMD, y]);
                parseInput(tailY);
                break;
            case TEXT_CMD:
                let [text, tailText] = parseTextArg(tail);
                cmds.push([TEXT_CMD, text]);
                parseInput(tailText);
                break;
        }
    }
    parseInput(input);
    return cmds;
}

function render(cmds)
{
    const canvas = document.getElementById("ggCanvas");
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "#000000";
    let x = 0;
    let y = 0;
    let font = Font16;
    for (let i = 0; i < cmds.length; i++) {
        let currCmd = cmds[i];
        switch (currCmd[0]) {
            case FONT_CMD:
                font = fontMap[currCmd[1]];
                break;
            case SETX_CMD:
                x = currCmd[1];
                break;
            case SETY_CMD:
                y = currCmd[1];
                break;
            case TEXT_CMD:
                drawString(ctx, font, currCmd[1], x, y);
                break;
        }
    }
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

async function programSerial(inp)
{
    console.log("Programming over serial port");
    if ("serial" in navigator) {
        const port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        const closedPromise = readSerialOutput(port);

        const writer = port.writable.getWriter();
        let buf = new Uint8Array(inp.length + 1);
        buf[0] = inp.length;
        for (let i = 1; i < buf.length; i++) {
            buf[i] = inp.charCodeAt(i-1);
        }
        await writer.write(buf);
        writer.releaseLock();

        let result = await closedPromise;
        await port.close();
        console.log("All done, result: " + result);
    }
}

async function programSound(cmds)
{
    console.log("TODO: programming via sound");
}