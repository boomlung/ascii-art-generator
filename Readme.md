# **Ascii art generator**

## Description

The ascii_art.c program can turn a bmp file into an ascii art and it will be shown in terminal.

_Noted that the output image is responsive to the terminal width (the image width will be the same as the terminal width)_

## Environment

**This program requires a linux environment** (i.e. Ubuntu).

The command to execute this program:

```bash
gcc -o ascii_art ascii_art.c -lm    #complie the program
./ascii_art < cuhk.bmp              #redirect the bitmap file to the executable
```

The above output will be:
![Example output](example.png)
Original image:
![Example image](cuhk.bmp)
