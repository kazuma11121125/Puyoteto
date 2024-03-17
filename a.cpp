#include <stdio.h>

int map[2][20][16];
int map_sub[2][8][16];

int mode = 1;

void MakeMap()
{
    /*
    テトリスのマップを作成 
    0:空白 1:壁
    [0][y][x] [1][y][x]
    1player   2player 
    */
    for(int num = 0; num<2; num++){
        for (int y =0; y<20; y++){
            for (int x = 0; x<16; x++){
                if(x == 0||x == 15 ||y == 19){//
                    map[num][y][x] = 1;
                }else{
                    map[num][y][x] = 0;
                    
                }
            }
        }
        //サブマップの作成
        for (int y =0; y<8; y++){
            for (int x = 0; x<16; x++){
                if(y == 0 || y == 7 || x == 0|| x == 7 ||x == 8 || x == 15){
                    map_sub[num][y][x] = 1;
                }else{
                    map_sub[num][y][x] = 0;
            }
            }
        }
    }
    // for(int num = 0; num<2; num++){
    //     for(int y = 0; y < 8; y++){
    //         for(int x = 0; x < 16; x++){
    //             printf("%d ",map_sub[num][y][x]);
    //         }
    //         printf("\n");
    //     }
    //     for(int y = 0; y < 20; y++){
    //         for(int x = 0; x < 16; x++){
    //             printf("%d ",map[num][y][x]);
    //         }
    //         printf("\n");
    //     }
    // }
}

void MapShow(int modes){
    /*
    マップの表示
    mode 1・・・シングル 
    mode 2・・・対戦
    */

    /*
    31：文字の色を赤に変更
    32：文字の色を緑に変更
    33：文字の色を黄に変更
    34：文字の色を青に変更
    35：文字の色をマゼンタに変更
    36：文字の色をシアンに変更
    37：文字の色をグレーに変更

    壁:白1
    next:グレー 8
    */
    int map_YX[2][2] = {{20,16},{8,16}};//main sub画面切り替え sub mainの順
    for(int map_change = 1; map_change >= 0; map_change--){//main sub画面切り替え
        puts("");
        for(int y = 0; y < map_YX[map_change][0]; y++){//y
            for(int st = 0; st < modes; st++){//player
                printf("  ");
                for(int x = 0; x < map_YX[map_change][1]; x++){//x
                    int block_number;
                    if(map_change == 1)
                    {
                        block_number = map_sub[st][y][x];
                    }else{
                        block_number = map[st][y][x];
                    }
                    if(block_number >= 10){
                        block_number -= 10;
                    }
                    // printf("%d ",block_number);
                    if (block_number == 1)
                    {
                        printf("\x1b[0m□ \x1b[0m");
                    }
                    else if (block_number == 2)
                    {
                        printf("\x1b[31m■ \x1b[0m");
                    }
                    else if (block_number == 3)
                    {
                        printf("\x1b[32m■ \x1b[0m");
                    }
                    else if (block_number == 4)
                    {
                        printf("\x1b[33m■ \x1b[0m");
                    }
                    else if (block_number == 5)
                    {
                        printf("\x1b[34m■ \x1b[0m");
                    }
                    else if (block_number == 6)
                    {
                        printf("\x1b[35m■ \x1b[0m");
                    }
                    else if (block_number == 7)
                    {
                        printf("\x1b[36m■ \x1b[0m");
                    }
                    else if (block_number == 8)
                    {
                        printf("\x1b[37m■ \x1b[0m");
                    }
                    else
                    {
                        printf("  ");
                    }
                }
            }
            puts("");
        }
        printf("\n\n");
    }
}

int main()
{
    MakeMap();
    MapShow(2);
    return 0;
}