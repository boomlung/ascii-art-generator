#include <stdio.h>
#include <math.h>
#include "palette.h"
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
int r = 0, g = 0, b = 0;
int reader(int size)
{
    int i;
    unsigned char arr[size];
    for (i = 0; i < size; i++)
    {
        scanf("%c", &arr[i]);
    }
    return *(unsigned *)arr;
}
int sign(double input)
{
    double ascii_grey[26] =
        {0, 0.015625, 0.031250, 0.046875, 0.062500, 0.070313, 0.078125, 0.085938, 0.093750, 0.101563,
         0.109375, 0.117188, 0.125000, 0.132813, 0.140625, 0.148438, 0.156250, 0.164063, 0.171875, 0.179688,
         0.187500, 0.195313, 0.210938, 0.218750, 0.226563, 0.234375};
    int ascii_sign[26] =
        {32, 96, 39, 45, 58, 123, 47, 63, 61, 108,
         55, 114, 73, 76, 54, 110, 70, 97, 88, 82,
         103, 64, 72, 35, 78, 77};
    int i = 0;
    while (1)
    {
        if (input < ascii_grey[i])
        {
            if ((input - ascii_grey[i--]) <= (ascii_grey[i] - input))
                return ascii_sign[i--];
            else
                return ascii_sign[i];
        }
        else if (input == ascii_grey[i])
            return ascii_sign[i];
        i++;
    }
}
void dectohex(int input, int *r, int *g, int *b)
{
    int arr[6] = {0}, tmp, i = 5;
    while (1)
    {
        if (input == 0)
        {
            break;
        }
        tmp = input % 16;
        if (tmp == 0)
        {
            arr[i] = 0;
            input /= 16;
        }
        else if (input < 16)
        {
            arr[i] = tmp;
            break;
        }
        else
        {
            arr[i] = tmp;
            input -= tmp;
            input /= 16;
        }
        i--;
    }
    *r = arr[0] * 16 + arr[1];
    *g = arr[2] * 16 + arr[3];
    *b = arr[4] * 16 + arr[5];
}
int manhattan(int R, int G, int B)
{
    int smallest, position, ans = 500, tmp, i;
    for (i = 0; i < 256; i++)
    {
        dectohex(palette[i], &r, &g, &b);
        tmp = abs(r - R) + abs(g - G) + abs(b - B);
        if (tmp < ans)
        {
            ans = tmp;
            position = i;
        }
    }
    return position;
}
int main()
{
    reader(2);
    int size = reader(4);
    reader(4);
    int offset = reader(4);
    reader(4);
    int width = reader(4);
    int height = reader(4);
    reader(offset - 26);
    int i, j, padding = 0;
    if (width * 3 % 4 != 0)
    {
        padding = 4 - width * 3 % 4;
    }
    static int R[3000][3000], G[3000][3000], B[3000][3000], resized_R[3000][3000], resized_G[3000][3000], resized_B[3000][3000], resized_block_width[3000];
    static double grey[3000][3000], resized_grey[3000][3000], block_height[3000];
    double max_grey = 0.234375;
    for (i = height - 1; i >= 0; i--)
    {
        for (j = 0; j < width; j++)
        {
            B[i][j] = reader(1);
            G[i][j] = reader(1);
            R[i][j] = reader(1);
            grey[i][j] = (255 - (0.3 * R[i][j] + 0.59 * G[i][j] + 0.11 * B[i][j])) / 255 * max_grey;
        }
        reader(padding);
    }
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int term_width = w.ws_col;
    double tmp = (double)width / term_width;
    if (tmp < 1)
    {
        tmp = 1;
    }
    int x_count = 0, y_count = 0;
    int x = 0, y = 0;
    int approximate = round(tmp * (x + 1)) - round(tmp * x);
    int tmp_block_height;
    int row_number;
    for (i = 0; i < height; i++)
    {
        x = 0;
        for (j = 0; j < width; j++)
        {
            resized_R[y][x] += R[i][j];
            resized_G[y][x] += G[i][j];
            resized_B[y][x] += B[i][j];
            x_count++;
            if (x_count == approximate)
            {
                resized_block_width[x] = approximate;
                x_count = 0;
                x++;
                approximate = round(tmp * (x + 1)) - round(tmp * x);
            }
        }
        y_count++;
        tmp_block_height = resized_block_width[y] * 2;
        row_number = round((double)height / tmp_block_height);
        block_height[y] = (double)height / row_number;
        block_height[y] = round(block_height[y]);
        if (y_count == block_height[y])
        {
            y_count = 0;
            y++;
        }
        else if (i == height - 1)
        {
            block_height[y] = y_count;
        }
    }
    int colour;
    for (i = 0; i <= y; i++)
    {
        for (j = 0; j <= x; j++)
        {
            resized_R[i][j] = (double)resized_R[i][j] / (resized_block_width[j] * block_height[i]);
            resized_G[i][j] = (double)resized_G[i][j] / (resized_block_width[j] * block_height[i]);
            resized_B[i][j] = (double)resized_B[i][j] / (resized_block_width[j] * block_height[i]);
            resized_grey[i][j] = (255 - (0.3 * resized_R[i][j] + 0.59 * resized_G[i][j] + 0.11 * resized_B[i][j])) / 255 * max_grey;
            colour = manhattan(resized_R[i][j], resized_G[i][j], resized_B[i][j]);
            printf("\x1b[38;5;%dm", colour);
            printf("%c", sign(resized_grey[i][j]));
            printf("\x1b[0m");
        }
        printf("\n");
    }
    return 0;
}