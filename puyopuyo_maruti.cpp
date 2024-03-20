
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>

/*
作成期間 1週間 kazuma1112とsomeyashの共同制作作品。

Ubuntu環境のみ動作確認(windows未確認)

gcc -o puyopuyo_maruti puyopuyo_maruti.cpp

./puyopuyo_maruti

操作方法
*/


int tetris_drawings[25][45];    // main表示
int tetris_drawings_sub[8][50]; // next表示 hold表示

int block[3][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}};

int block1_x, block1_y, block1_color = 0, block1_color_next = 0, block1_hold = 0;
int block2_x, block2_y, block2_color = 0, block2_color_next = 0, block2_hold = 0;
int block3_x, block3_y, block3_color = 0, block3_color_next = 0, block3_hold = 0;
int block4_x, block4_y, block4_color = 0, block4_color_next = 0, block4_hold = 0;

int A_combo = 0;
int B_combo = 0;

int jama1;
int jama2;
int jama1past = 0;
int jama2past = 0;

int end = 0;

int zahyou;

// block_color
// 1 白 壁
// 2
// 3
// 4

int mode1 = 1;
int mode2 = 1;
// 0...control
// 1...fall or delete

int sleep1 = 0;
int sleep2 = 0;

void show();
void zero_down();

int adjustment(int player)
{
    if (player == 1)
    {
        return 0;
    }
    return 31;
}

void make_map()
{
    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 45; j++)
        {
            if (i == 24 && (j < 14 || j > 30))
            {
                tetris_drawings[i][j] = 1;
            }
            else
            {
                tetris_drawings[i][j] = 0;
            }
        }
        tetris_drawings[i][0] = 1;
        tetris_drawings[i][13] = 1;
        tetris_drawings[i][31] = 1;
        tetris_drawings[i][44] = 1;
    }
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            if (((i == 0 || i == 7) && (j < 19 || j > 30)) || j == 0 || j == 18 || j == 31 || j == 49)
            {
                tetris_drawings_sub[i][j] = 1;
            }
            else
            {
                tetris_drawings_sub[i][j] = 0;
            }
        }
        tetris_drawings_sub[i][9] = 1;
        tetris_drawings_sub[i][40] = 1;
    }
}

void block_make(int player)
{
    srand((unsigned int)time(NULL));
    if (block1_color_next == 0)
    {
        block1_color = rand() % 4 + 2;
        usleep(7);
        block2_color = rand() % 4 + 2;
        usleep(7);
        block1_color_next = rand() % 4 + 2;
        usleep(7);
        block2_color_next = rand() % 4 + 2;
        usleep(7);
        block3_color = rand() % 4 + 2;
        usleep(7);
        block4_color = rand() % 4 + 2;
        usleep(7);
        block3_color_next = rand() % 4 + 2;
        usleep(7);
        block4_color_next = rand() % 4 + 2;
    }
    else
    {
        if (player == 1)
        {
            block1_color = block1_color_next;
            block2_color = block2_color_next;
            block1_color_next = rand() % 4 + 2;
            usleep(7);
            block2_color_next = rand() % 4 + 2;
        }
        else
        {
            block3_color = block3_color_next;
            block4_color = block4_color_next;
            block3_color_next = rand() % 4 + 2;
            usleep(7);
            block4_color_next = rand() % 4 + 2;
        }
    }
}

void block_kaitenn(int player)
{
    printf("1");
    int y1, x1, y2, x2;
    printf("2");
    if (player == 1)
    {
        y1 = block1_y;
        x1 = block1_x;
        y2 = block2_y;
        x2 = block2_x;
        printf("3");
    }
    else
    {
        y1 = block3_y;
        x1 = block3_x;
        y2 = block4_y;
        x2 = block4_x;
        printf("4");
    }
    
    if (y1 == y2 && x1 + 1 == x2 && tetris_drawings[y1 + 1][x1] > 0 && !tetris_drawings[y1 - 1][x1])
    {
        y1--;
        y2--;
        int i = x2 - x1;
        int j = y1 - y2;
        x2 = x1 + j;
        y2 = y1 + i;
        printf("5");
    }
    else if (y1 == y2 && x1 - 1 == x2 && tetris_drawings[y1 - 1][x1] && !tetris_drawings[y1 + 1][x1])
    {
        y1++;
        y2++;
        int i = x2 - x1;
        int j = y1 - y2;
        x2 = x1 + j;
        y2 = y1 + i;
        printf("6");
    }
    else if (y1 - 1 == y2 && x1 == x2 && tetris_drawings[y1][x1 + 1] && !tetris_drawings[y1][x1 - 1])
    {
        x1--;
        x2--;
        int i = x2 - x1;
        int j = y1 - y2;
        x2 = x1 + j;
        y2 = y1 + i;
        printf("7");
    }
    else if (y1 + 1 == y2 && x1 == x2 && tetris_drawings[y1][x1 - 1] && !tetris_drawings[y1][x1 + 1])
    {
        x1++;
        x2++;
        int i = x2 - x1;
        int j = y1 - y2;
        x2 = x1 + j;
        y2 = y1 + i;
        printf("8");
    }
    else if ((y1 == y2 && x1 + 1 == x2 && !tetris_drawings[y1 + 1][x1]) || (y1 == y2 && x1 - 1 == x2 && !tetris_drawings[y1 - 1][x1]) || (y1 - 1 == y2 && x1 == x2 && !tetris_drawings[y1][x1 + 1]) || (y1 + 1 == y2 && x1 == x2 && !tetris_drawings[y1][x1 - 1]))
    {
        int i = x2 - x1;
        int j = y1 - y2;
        x2 = x1 + j;
        y2 = y1 + i;
        printf("9");
        //block2_x = block1_y - block2_y;
        //block2_y = block2_x - block1_x;
    }
    if (player == 1)
    {
        block1_y = y1;
        block1_x = x1;
        block2_y = y2;
        block2_x = x2;
        printf("10");
    }
    else
    {
        block3_y = y1;
        block3_x = x1;
        block4_y = y2;
        block4_x = x2;
    }
    printf("11");
    printf("x1=%dy1=%dx2=%dy2%d", x1, y1, x2, y2);
    // if (block2_y == 31)
    //{
    //     block1_y--;
    //     block2_y--;
    // }
    // else if (block2_x == 0)
    //{
    //     block1_x++;
    //     block2_x++;
    // }
    // else if (block2_x == 31)
    //{
    //     block1_x--;
    //     block2_x--;
    // }
}

void add_blocks(int y1, int x1, int c1, int y2, int x2, int c2)
{
    tetris_drawings[y1][x1] = c1;
    tetris_drawings[y2][x2] = c2;
}

void add_next(int player)
{
    if (player == 1)
    {
        tetris_drawings_sub[3][4] = block1_color_next;
        tetris_drawings_sub[3][5] = block2_color_next;
    }
    else
    {
        tetris_drawings_sub[3][35] = block3_color_next;
        tetris_drawings_sub[3][36] = block4_color_next;
    }
}

void block_hold(int player)
{
    if (player == 1)
    {
        if (block1_hold == 0)
        {
            block1_hold = block1_color;
            block2_hold = block2_color;
            block1_color = block1_color_next;
            block2_color = block2_color_next;
            block1_color_next = rand() % 4 + 2;
            block2_color_next = rand() % 4 + 2;
        }
        else
        {
            int c = block1_color;
            block1_color = block1_hold;
            block1_hold = c;
            c = block2_color;
            block2_color = block2_hold;
            block2_hold = c;
        }
        tetris_drawings_sub[3][13] = block1_hold;
        tetris_drawings_sub[3][14] = block2_hold;
    }
    else
    {
        if (block3_hold == 0)
        {
            block3_hold = block3_color;
            block4_hold = block4_color;
            block3_color = block3_color_next;
            block4_color = block4_color_next;
            block3_color_next = rand() % 4 + 2;
            block4_color_next = rand() % 4 + 2;
        }
        else
        {
            int c = block3_color;
            block3_color = block3_hold;
            block3_hold = c;
            c = block4_color;
            block4_color = block4_hold;
            block4_hold = c;
        }
        tetris_drawings_sub[3][44] = block3_hold;
        tetris_drawings_sub[3][45] = block4_hold;
    }
}

int hairetulen(int hairetu[])
{
    int i = 0;
    while (hairetu[i])
    {
        i++;
    }
    return i;
}

int in(int hairetu[], int num)
{
    int i = 0;
    while (hairetu[i])
    {
        if (hairetu[i] == num)
        {
            return 1;
        }
        i++;
    }
    return 0;
}

int delete_search(int player)
{
    int de[30]; //deleted
    int ad[30]; //adjacet
    for (int i = 0; i < 30; i++)
    {
        de[i] = 0;
        ad[i] = 0;
    }
    int num = 0; //number of adjacent block
    for (int i = 0; i < 24; i++)
    {
        for (int j = 1 + adjustment(player); j < 13 + adjustment(player); j++)
        {
            if (tetris_drawings[i][j] && tetris_drawings[i][j] < 6)
            {
                if (!in(de, i * 100 + j))
                {
                    ad[0] = i * 100 + j;
                }
                while (num != hairetulen(ad))
                {
                    // printf("\n1");
                    int m = 0;
                    num = hairetulen(ad);
                    while (ad[m])
                    {
                        if (i != 0 && tetris_drawings[(ad[m] - ad[m] % 100) / 100][ad[m] % 100] == tetris_drawings[(ad[m] - ad[m] % 100) / 100 - 1][ad[m] % 100] && !in(de, ((ad[m] - ad[m] % 100) / 100 - 1) * 100 + ad[m] % 100) && !in(ad, ((ad[m] - ad[m] % 100) / 100 - 1) * 100 + ad[m] % 100))
                        {
                            ad[hairetulen(ad)] = ad[m] - 100;
                        }
                        if (tetris_drawings[(ad[m] - ad[m] % 100) / 100][ad[m] % 100] == tetris_drawings[(ad[m] - ad[m] % 100) / 100 + 1][ad[m] % 100] && !in(de, ((ad[m] - ad[m] % 100) / 100 + 1) * 100 + ad[m] % 100) && !in(ad, ((ad[m] - ad[m] % 100) / 100 + 1) * 100 + ad[m] % 100))
                        {
                            ad[hairetulen(ad)] = ad[m] + 100;
                        }
                        if (tetris_drawings[(ad[m] - ad[m] % 100) / 100][ad[m] % 100] == tetris_drawings[(ad[m] - ad[m] % 100) / 100][ad[m] % 100 - 1] && !in(de, (ad[m] - ad[m] % 100) / 100 * 100 + (ad[m] % 100 - 1)) && !in(ad, (ad[m] - ad[m] % 100) / 100 * 100 + (ad[m] % 100 - 1)))
                        {
                            ad[hairetulen(ad)] = ad[m] - 1;
                        }
                        if (tetris_drawings[(ad[m] - ad[m] % 100) / 100][ad[m] % 100] == tetris_drawings[(ad[m] - ad[m] % 100) / 100][ad[m] % 100 + 1] && !in(de, (ad[m] - ad[m] % 100) / 100 * 100 + (ad[m] % 100 + 1)) && !in(ad, (ad[m] - ad[m] % 100) / 100 * 100 + (ad[m] % 100 + 1)))
                        {
                            ad[hairetulen(ad)] = ad[m] + 1;
                        }
                        m++;
                    }
                }
                if (hairetulen(ad) >= 4)
                {
                    return 1;
                }
                while (ad[0])
                {
                    ad[hairetulen(ad) - 1] = 0;
                }
                num = hairetulen(ad);
            }
        }
    }
    return 0;
}

void delete_block(int player)
{
    while (delete_search(player))
    {
        int deleted_blocks[30];  // 削除すべきブロックをここに入れる
        int adjacent_blocks[30]; // 同じ色のブロックと隣接しているブロックをここに入れる
        for (int i = 0; i < 30; i++)
        {
            deleted_blocks[i] = 0;
            adjacent_blocks[i] = 0;
        }
        int num_blocks = 0;
        for (int i = 0; i < 24; i++)
        {
            for (int j = 1 + adjustment(player); j < 13 + adjustment(player); j++)
            {
                if (tetris_drawings[i][j] && tetris_drawings[i][j] < 6)
                {
                    if (!in(deleted_blocks, i * 100 + j))
                    {
                        adjacent_blocks[0] = i * 100 + j;
                    }
                    while (num_blocks != hairetulen(adjacent_blocks))
                    {

                        int m = 0;
                        num_blocks = hairetulen(adjacent_blocks);
                        while (adjacent_blocks[m])
                        {
                            if (i != 0 && tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100][adjacent_blocks[m] % 100] == tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 - 1][adjacent_blocks[m] % 100] && !in(deleted_blocks, ((adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 - 1) * 100 + adjacent_blocks[m] % 100) && !in(adjacent_blocks, ((adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 - 1) * 100 + adjacent_blocks[m] % 100))
                            {
                                adjacent_blocks[hairetulen(adjacent_blocks)] = adjacent_blocks[m] - 100;
                            }
                            if (tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100][adjacent_blocks[m] % 100] == tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 + 1][adjacent_blocks[m] % 100] && !in(deleted_blocks, ((adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 + 1) * 100 + adjacent_blocks[m] % 100) && !in(adjacent_blocks, ((adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 + 1) * 100 + adjacent_blocks[m] % 100))
                            {
                                adjacent_blocks[hairetulen(adjacent_blocks)] = adjacent_blocks[m] + 100;
                            }
                            if (tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100][adjacent_blocks[m] % 100] == tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100][adjacent_blocks[m] % 100 - 1] && !in(deleted_blocks, (adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 * 100 + (adjacent_blocks[m] % 100 - 1)) && !in(adjacent_blocks, (adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 * 100 + (adjacent_blocks[m] % 100 - 1)))
                            {
                                adjacent_blocks[hairetulen(adjacent_blocks)] = adjacent_blocks[m] - 1;
                            }
                            if (tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100][adjacent_blocks[m] % 100] == tetris_drawings[(adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100][adjacent_blocks[m] % 100 + 1] && !in(deleted_blocks, (adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 * 100 + (adjacent_blocks[m] % 100 + 1)) && !in(adjacent_blocks, (adjacent_blocks[m] - adjacent_blocks[m] % 100) / 100 * 100 + (adjacent_blocks[m] % 100 + 1)))
                            {
                                adjacent_blocks[hairetulen(adjacent_blocks)] = adjacent_blocks[m] + 1;
                            }
                            m++;
                        }
                    }
                    if (hairetulen(adjacent_blocks) >= 4)
                    {
                        for (int k = 0; k < hairetulen(adjacent_blocks); k++)
                        {
                            tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100][adjacent_blocks[k] % 100] = 0;
                            if (tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100 - 1][adjacent_blocks[k] % 100] == 6)
                            {
                                tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100 - 1][adjacent_blocks[k] % 100] = 0;
                            }
                            if (tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100 + 1][adjacent_blocks[k] % 100] == 6)
                            {
                                tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100 + 1][adjacent_blocks[k] % 100] = 0;
                            }
                            if (tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100][adjacent_blocks[k] % 100 - 1] == 6)
                            {
                                tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100][adjacent_blocks[k] % 100 - 1] = 0;
                            }
                            if (tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100][adjacent_blocks[k] % 100 + 1] == 6)
                            {
                                tetris_drawings[(adjacent_blocks[k] - adjacent_blocks[k] % 100) / 100][adjacent_blocks[k] % 100 + 1] = 0;
                            }
                            deleted_blocks[hairetulen(deleted_blocks)] = adjacent_blocks[k];
                        }
                        //show();
                        if (player == 1)
                        {
                            if (!jama1)
                            {
                                jama1 = 10000;
                            }
                            sleep1 = 80000;
                            jama1 += (hairetulen(adjacent_blocks) * hairetulen(adjacent_blocks) * (A_combo + 1) * (A_combo + 1) / 8);
                        }
                        else
                        {
                            if (!jama2)
                            {
                                jama2 = 10000;
                            }
                            sleep2 = 80000;
                            jama2 += (hairetulen(adjacent_blocks) * hairetulen(adjacent_blocks) * (B_combo + 1) * (B_combo + 1) / 8);
                        }
                    }
                    while (adjacent_blocks[0])
                    {
                        adjacent_blocks[hairetulen(adjacent_blocks) - 1] = 0;
                    }
                    num_blocks = hairetulen(adjacent_blocks);
                    // printf("reset");
                }
            }
        }
    }
}

int search(int x, int y, int board[][45], int player)
{
    int i;
    for (i = y; i >= 0; i--)
    {
        if (board[i][x] > 0)
        {
            return (board[i][x]);
        }
    }
    return 0;
}

int void_search(int player)
{
    int past;
    if (player == 1)
    {
        past = jama1past;
    }
    else 
    {
        past = jama2past;
    }
    for (int x = 0; x < 12; x ++)
    {
        for (int y = 1; y < 24; y ++)
        {
            if (!tetris_drawings[y][(x + past) % 12 + 1 + adjustment(player)])
            {
                if (player == 1)
                {
                    jama1past = (past + x + 1) % 12;
                }
                else
                {
                    jama2past = (past + x + 1) % 12;
                }
                return y * 100 + (x + past) % 12 + 1 + adjustment(player);
            }
        }
    }
    return 0;
}

int all_search(int player)
{
    for (int i = 23; i > 0; i--)
    {
        for (int j = 1 + adjustment(player); j < 13 + adjustment(player); j++)
        {
            if (tetris_drawings[i][j] == 0 && search(j, i, tetris_drawings, player))
            {
                return 1;
            }
        }
    }
    return 0;
}

void zero_down(int player)
{
    int check = 0;
        for (int x = 1 + adjustment(player); x < 13 + adjustment(player); x++)
        {
            for (int y = 23; y > 1; y--)
            {
                if (tetris_drawings[y][x] == 0 && search(x, y, tetris_drawings, player))
                {
                    for (int yt = y; yt - 1 >= 0; yt--)
                    {
                        tetris_drawings[yt][x] = tetris_drawings[yt - 1][x];
                    }
                    tetris_drawings[0][x] = 0;
                    if (search(x, y, tetris_drawings, player) == 6)
                    {
                        if (player == 1)
                        {
                            sleep1 = 40000;
                        }
                        else
                        {
                            sleep2 = 40000;
                        }
                    }
                    else
                    {
                        if (player == 1)
                        {
                            sleep1 = 120000;
                        }
                        else
                        {
                            sleep2 = 120000;
                        }
                    }
                    break;
                }
            }
        }
}

// 落ちてくるブロックの生成と座標の指定
// 生成したブロックを落としながら操作も受け付ける
// 床やほかのブロックに接触したときに↑を終わる
// ブロック落下とブロック消去

void show()
{
    system("clear");
    // printf("\n\n\n");
    int h_max;
    int w_max;
    int han;
    for (int t = 0; t < 2; t++)
    {
        if (t == 0)
        {
            h_max = 8;
            w_max = 50;
        }
        else
        {
            h_max = 25;
            w_max = 45;
        }
        for (int h = 0; h < h_max; h++)
        {
            for (int w = 0; w < w_max; w++)
            {
                if (t == 0)
                {
                    han = tetris_drawings_sub[h][w];
                }
                else
                {
                    han = tetris_drawings[h][w];
                }
                if ((mode1 == 0 && t == 1 && ((h == block1_y && w == block1_x) || (h == block2_y && w == block2_x))) || (mode2 == 0 && t == 1 && ((h == block3_y && w == block3_x) || (h == block4_y && w == block4_x))))
                {
                    int color;
                    if (h == block1_y && w == block1_x)
                    {
                        color = block1_color;
                    }
                    else if (h == block2_y && w == block2_x)
                    {
                        color = block2_color;
                    }
                    else if (h == block3_y && w == block3_x)
                    {
                        color = block3_color;
                    }
                    else
                    {
                        color = block4_color;
                    }
                    switch (color)
                    {
                        case 2:
                            printf("\x1b[38;5;009m◎ \x1b[0m");
                            break;
                        case 3:
                            printf("\x1b[38;5;087m◎ \x1b[0m");
                            break;
                        case 4:
                            printf("\x1b[38;5;010m◎ \x1b[0m");
                            break;
                        case 5:
                            printf("\x1b[38;5;011m◎ \x1b[0m");
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    switch (han)
                    {
                    case 0:
                        printf("　");
                        break;
                    case 1:
                        printf("\x1b[0m□ \x1b[0m");
                        break;
                    case 2:
                        printf("\x1b[38;5;009m◎ \x1b[0m");
                        break;
                    case 3:
                        printf("\x1b[38;5;087m◎ \x1b[0m");
                        break;
                    case 4:
                        printf("\x1b[38;5;010m◎ \x1b[0m");
                        break;
                    case 5:
                        printf("\x1b[38;5;011m◎ \x1b[0m");
                        break;
                    case 6:
                        printf("\x1b[38;5;248m◎ \x1b[0m");
                        break;
                    default:
                        break;
                    }
                }
            }
            puts("");
        }
        //printf("\n%d\n", jama1);
    }
    if (A_combo)
    {
        printf("　　　　<<%3d COMBO", A_combo);
        if (A_combo > 9)
        {
            printf("!");
        }
        else
        {
            printf(" ");
        }
        printf(">>\n");
    }
    else
    {
        printf("　　　　            \n");
    }
    if (B_combo)
    {
        printf("　　　　<<%3d COMBO", B_combo);
        if (B_combo > 9)
        {
            printf("!");
        }
        else
        {
            printf(" ");
        }
        printf(">>\n");
    }
    //printf("mode1 = %d mode2 = %d jama1 = %d jama2 = %d sleep1 = %d sleep2 = %d", mode1, mode2, jama1, jama2, sleep1, sleep2);
}

int end_judge(int banmen[][45])
{
    for (int i = 1; i < 13; i++)
    {
        if (banmen[0][i])
        {
            return 0;
        }
    }
    for (int i = 32; i < 44; i++)
    {
        if (banmen[0][i])
        {
            return 0;
        }
    }
    return 1;
}

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

int main(void)
{
    int seconds = 0;
    struct termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    make_map();
    system("xset r off");
    do
    { // 上まで積み上げ切られていなければゲームを続ける
        if (mode1 == 1 && sleep1 <= 0 && !all_search(1) && !delete_search(1))
        {
            //zahyou = void_search(1);
            //if (jama2 && (jama2 < 1000 || !mode2) && zahyou)
            //{
            //    if (jama2 > 1000 && !mode2)
            //    {
            //        jama2 -= 1000;
            //    }
            //    tetris_drawings[(zahyou - zahyou % 100) / 100][zahyou % 100] = 6;
            //    jama2 --;
            //    sleep1 = 100000;
            //}
            //else
            //{
                block1_x = 4;
                block1_y = 0;
                block2_x = 5;
                block2_y = 0;
                mode1 = 0;
                block_make(1);
                add_next(1);
                sleep1 = 75000;
            //}
        }
        else if (mode2 == 1 && sleep2 <= 0 && !all_search(2) && !delete_search(2))
        {
            //zahyou = void_search(2);
            //if (jama1 && (jama1 < 1000 || !mode1) && zahyou)
            //{
            //    if (jama1 > 1000 && !mode1)
            //    {
            //        jama1 -= 1000;
            //    }
            //    tetris_drawings[(zahyou - zahyou % 100) / 100][zahyou % 100] = 6;
            //    jama1 --;
            //    sleep2 = 100000;
            //}
            //else
            //{
                block3_x = 35;
                block3_y = 0;
                block4_x = 36;
                block4_y = 0;
                mode2 = 0;
                block_make(2);
                add_next(2);
                sleep2 = 75000;
            //}
        }
        unsigned int microseconds = 150000;
        seconds += 5000;
        char c;
        if (kbhit())
        {
            // ↑キーボード設定
            //  char c;
            //  if (read(STDIN_FILENO, &c, 1) > 0)
            //  このifで入力されたかの判定、入力されたものはcに入っている。
            if (read(STDIN_FILENO, &c, 1) > 0)
            {
                if (mode1 == 0)
                {
                    if (c == 'a' && !(tetris_drawings[block1_y][block1_x - 1]) && !(tetris_drawings[block2_y][block2_x - 1]))
                    {
                        block1_x--;
                        block2_x--;
                    }
                    else if (c == 'd' && !(tetris_drawings[block1_y][block1_x + 1]) && !(tetris_drawings[block2_y][block2_x + 1]))
                    {
                        block1_x++;
                        block2_x++;
                    }
                    else if (c == 'w')
                    {
                        block_kaitenn(1);
                    }
                    else if (c == 'q')
                    {
                        block_hold(1);
                    }
                    else if (c == 's' && !tetris_drawings[block1_y + 1][block1_x] && !tetris_drawings[block2_y + 1][block2_x])
                    {
                        block1_y++;
                        block2_y++;
                    }
                }
                if (mode2 == 0)
                {
                    if (c == 'j' && !(tetris_drawings[block3_y][block3_x - 1]) && !(tetris_drawings[block4_y][block4_x - 1]))
                    {
                        block3_x--;
                        block4_x--;
                    }
                    else if (c == 'l' && !(tetris_drawings[block3_y][block3_x + 1]) && !(tetris_drawings[block4_y][block4_x + 1]))
                    {
                        block3_x++;
                        block4_x++;
                    }
                    else if (c == 'i')
                    {
                        block_kaitenn(2);
                    }
                    else if (c == 'u')
                    {
                        block_hold(2);
                    }
                    else if (c == 'k' && !tetris_drawings[block3_y + 1][block3_x] && !tetris_drawings[block4_y + 1][block4_x])
                    {
                        block3_y++;
                        block4_y++;
                    }
                }
                else if (c == 'b')
                {
                    end = 1;
                }
                c = '\0';
            }
        }
        if (end == 0)
        {
            show();
            usleep(10000);
            sleep1 -= 8000;
            sleep2 -= 8000;
            if (sleep1 <= 0 && mode1 == 0)
            {
                block1_y++;
                block2_y++;
                sleep1 = 150000;
            }
            if (sleep2 <= 0 && mode2 == 0)
            {
                block3_y++;
                block4_y++;
                sleep2 = 150000;
            }
            
        }
        if (end == 0)
        {
            if (mode1 == 0 && (tetris_drawings[block1_y][block1_x] || tetris_drawings[block2_y][block2_x]))
            {
                mode1 = 1;
                block1_y--;
                block2_y--;
                add_blocks(block1_y, block1_x, block1_color, block2_y, block2_x, block2_color);
                zero_down(1);
            }
            if (mode2 == 0 && (tetris_drawings[block3_y][block3_x] || tetris_drawings[block4_y][block4_x]))
            {
                mode2 = 1;
                block3_y--;
                block4_y--;
                add_blocks(block3_y, block3_x, block3_color, block4_y, block4_x, block4_color);
                zero_down(1);
            }
            if (sleep1 <= 0 && mode1 == 1)
            {
                if (all_search(1))
                {
                    zero_down(1);
                }
                else if (delete_search(1))
                {
                    delete_block( 1);
                }
                else
                {
                    A_combo = 0;
                    if (jama2)
                    {
                        mode1 = 3;
                    }
                }
            }
            if (sleep2 <= 0 && mode2 == 1)
            {
                if (all_search(2))
                {
                    zero_down(2);
                }
                else if (delete_search(2))
                {
                    delete_block(2);
                }
                else
                {
                    B_combo = 0;
                    if (jama1)
                    {
                        mode2 = 3;
                    }
                }
            }
            if (mode1 == 3 && ((jama2 && jama2 < 10000) || (jama2 > 10000 && !mode2) || mode2 == 3) && sleep1 <= 0)
            {
                zahyou = void_search(1);
                if (zahyou)
                {
                    if (jama2 > 10000 && mode2 != 1)
                    {
                        jama2 -= 10000;
                    }
                    tetris_drawings[(zahyou - zahyou % 100) / 100][zahyou % 100] = 6;
                    jama2 --;
                    if (!jama2)
                    {
                        mode1 = 1;
                    }
                    else
                    {
                        sleep1 = 90000;
                    }
                }
            }
            if (mode2 == 3 && ((jama1 && jama1 < 10000) || (jama1 > 10000 && !mode1) || mode1 == 3) && sleep2 <= 0)
            {
                zahyou = void_search(2);
                if (zahyou)
                {
                    if (jama1 > 10000 && mode1 != 1)
                    {
                        jama1 -= 10000;
                    }
                    tetris_drawings[(zahyou - zahyou % 100) / 100][zahyou % 100] = 6;
                    jama1 --;
                    if (!jama1)
                    {
                        mode2 = 1;
                    }
                    else
                    {
                        sleep2 = 90000;
                    }
                }
            }
        }
        /*while(落下もしくは消去しなければならないブロックがあるならば){
            while(落下させなければならないブロックがあるならば){
                落下させる
            };
            while(消去しなければならないブロックがあるならば){
                消去する
            }
          }
          */
    } while (end_judge(tetris_drawings) && end == 0);
    for (int i = 0; i < 45; i ++)
    {
    //    printf("%d = %d\n", i, tetris_drawings[0][i]);
    }
    printf("%d", end_judge(tetris_drawings));
    if (end == 0)
    {
        printf("GAME OVER\n");
    }
    system("xset r on");
    return 0;
}