"use strict";
function DrawChar(ctx, font, ch, x, y)
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

function DrawString(ctx, font, str, x, y)
{
    for (let i = 0; i < str.length; i++) {
        DrawChar(ctx, font, str[i], x, y);
        x += font.width;
    }
}
