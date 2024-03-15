#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int map[2][16][14];//[player][y][x]
int map_sub[2][8][19];//[player][x][y]

int puyopuyo_block[2][2][2];//[1st 2nd] [2] 1 2選択 [2] x y 
int puyopuyo_block_color[2][2][2];//[1st 2nd] now next選択 [2]  1 2　カラー選択？
int puyopuyo_block_hold[2][2];//[1st 2nd] 1 2選択 
int mode[2];//0:テトリス 1:ぷよぷよ
int current_blopuyo[2];//[player][block]
int next_blopuyo[2];//ぷよぷよの場合は十の位の数字と一の位の数字で二つのぷよの色を表す
int hold_blopuyo[2];
int block_direction[2];//回転回数
int wait_time[2] = {0};
int atack_power[2] = {0};

int score[2];//スコア 

int currentBlock[2][4][4];//[player][y][x]

int next_tetris_block_number[2] = {0,0};

struct Block
{
    int I[4][4] = {
        {2, 2, 2, 2},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

    int o[4][4] = {
        {0, 3, 3, 0},
        {0, 3, 3, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    };

    int T[4][4] = {
        {0, 4, 0, 0},
        {4, 4, 4, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

    int J[4][4] = {
        {5, 0, 0, 0},
        {5, 5, 5, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

    int L[4][4] = {
        {0, 0, 6, 0},
        {6, 6, 6, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

    int S[4][4] = {
        {0, 7, 7, 0},
        {7, 7, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

    int Z[4][4] = {
        {8, 8, 0, 0},
        {0, 8, 8, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};
};

void MakeMap()
{
    /*
    テトリスのマップを作成 
    0:空白 1:壁
    [0][y][x] [1][y][x]
    1player   2player 
    */
    for(int num = 0; num<2; num++){
        for (int y =0; y<16; y++){
            for (int x = 0; x<14; x++){
                if(y == 15){//
                    map[num][y][x] = 1;
                }else{
                    map[num][y][x] = 0;
                    
                }
            }
            map[num][y][0] = 1;
            map[num][y][6] = 1;
        }
        //サブマップの作成
        for (int x = 0; x<8; x++){
            for(int y = 0; y<19; y++){
                if(x == 0 || x == 7 || y == 0 || y == 18){
                    map_sub[num][x][y] = 1;
                }else{
                    map_sub[num][x][y] = 0;
                }
            }
            map_sub[num][x][9] = 1;
        }
    }
}

void SwitchBlock(int player)
{
    /*
    ブロックの切り替え
    */
    Block blocks;
    if(mode[player] == 0){
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                switch (current_blopuyo[player])
                {
                case 0:
                    currentBlock[player][y][x] = blocks.I[y][x];
                    break;
                case 1:
                    currentBlock[player][y][x] = blocks.o[y][x];
                    break;
                case 2:
                    currentBlock[player][y][x] = blocks.T[y][x];
                    break;
                case 3:
                    currentBlock[player][y][x] = blocks.J[y][x];
                    break;
                case 4:
                    currentBlock[player][y][x] = blocks.L[y][x];
                    break;
                case 5:
                    currentBlock[player][y][x] = blocks.S[y][x];
                    break;
                case 6:
                    currentBlock[player][y][x] = blocks.Z[y][x];
                    break;
                    }
                }
            }
        }
}

void MakeBlock(int player)
{   
    /*
    ブロックの作成

    */
    srand((unsigned int)time(NULL));
    if(mode[player] == 0){
        next_blopuyo[player] = rand() % 7;
    }
    else{
        next_blopuyo[player] = (rand() % 4 + 2) * 10;
        next_blopuyo[player] += (rand() % 4 + 2);
    }
}

void Rotate_Block(int player, int mode){
    /*
    ブロックの回転
    */
    block_direction[player] = (block_direction[player] + 1) % 4;
    int temp[4][4]; 
    if(!mode){
        for(int c = 0; c < 2; c++){
            for(int y = 0; y < 4; y++){
                for(int x = 0; x < 4; x++){
                    if(!c){
                        temp[y][x] = currentBlock[player][y][x];
                    }
                    else{
                        currentBlock[player][y][x] = temp[3 - x][y];
                    }
                }
            }
        }
    }else{
        //puyopuyo pass;
    }
}
//落ちるぷよがあるかどうか毎回判定してた気がする
//そこ僕と違う処理方法だからそっちに合わせます。できるかはできるようするんです。
void PlaceBlock(int player,int mode, int GameMode,int put_x,int put_y){
    /*
    ブロックをマップに配置
    */
   //配置方法要検討
}

void FallBlock(int player){//多分できた
    /*
    ブロックを落下させる
    */
    if(!mode[player])
    {
        for(int y = 14; y >= 0; y--){
            for(int x = 1; x < 13; y++){
                if(map[player][y][x]){
                    break;
                }
                if(x == 12){
                    for(int i = y; i >= 0; y--){
                        for(int j = 1; j < 13; j++){
                            if(y){
                                map[player][i][j] = map[player][i -1][j];
                            }
                            else{
                                map[player][i][j] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        for(int x = 1; x < 13; x++){
            for(int y = 14; y > 0; y--){
                if(!map[player][y][x]){
                    for(y--; y >= 0; y--){
                        if(map[player][y][x]){
                            for(; y >= 0; y--){
                                if(map[player][y][x]){
                                    map[player][y + 1][x] = map[player][y][x];//大幅字下げ！！
                                }
                            }
                            map[player][y][x] = 0;
                        }
                    }
                }
            }
        }
    }
}
//
void HinderBlock(int player,int power){//引数playerにはお邪魔ブロックを送られる側ではなく，送る側の番号を入れる
    /*
    お邪魔ブロックを相手のマップに配置
    */
    if(!mode[1 - player]){
        for(int y = power; y < 15; y++){
            for(int x = 1; x < 13; x++){
                map[1 - player][y - power][x] = map[1 - player][y][x]; 
            }
        }
        for(int i = 0; i < power; i++){
            srand((unsigned int)time(NULL));
            int x = rand() % 12 + 1;
            for(int j = 1; j < 13; j++){
                if(j != x){
                    map[1 - player][i][j] = 1;
                }
                else{
                    map[1 - player][i][j] = 0;
                }
            }
        }
        atack_power[player] = 0;
    }
    else{
        srand((unsigned int)time(NULL));
        int start = rand() % 12 + 1;
        for (int y = 0; power > 0 && y < 15; y++){
            for(int x = )
        }
    }
}

void DeleteBlock(int player, int mode, int put_x, int put_y){
    /*
    ブロックをマップから削除
    */
}

int CanPlaceBlock(int player,int mode, int put_x, int put_y){
    /*
    ブロックをマップに配置できるか
    */
    if(!mode){
        for(int y = 0; y<4; y++){
            for(int x = 0; x<4; x++){
                int ys = put_y + y;
                int xs = put_x + x;
                int bls = currentBlock[player][y][x];
                if(xs <= 13 && xs >= 0 &&
                   ys < 0 || ys >= 16 || xs < 9 || xs >= 14 ||
                   (bls >= 1 && bls <= 14)
                   && (map[player][ys][xs] >= 1 && map[player][ys][xs] <= 14))
                   
                   {
                        return 0; 
                   }
                   //回転するときの判定はバグの可能性大。引用元がバグあり。だれがif文の魔人だ
                   //if文の魔人さん　出番です
            }
        }
   }else{
        //puyopuyo pass;
   }
}

void CheckArrange(){
    /*
    ブロックの配置を確認

    */
}

void MapShow(int mode){
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
    int map_YX[2][2] = {{16,14},{8,19}};//main sub画面切り替え sub mainの順
    for(int map_change = 0; map_change <2; map_change++){//main sub画面切り替え
        for(int y = 0; y < map_YX[map_change][0]; y++){//y
            for(int x = 0; x < map_YX[map_change][1]; x++){//x
                for(int st = 0; st < mode; st++){//player
                    int block_number = map[st][y][x];
                    if(block_number >= 10){
                        block_number -= 10;
                    }
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

void GameOver(){
    /*
    ゲームオーバー
    */
   system("clear");
    if(score[0] > score[1]){
        printf("player1 gameover\n");
        printf("Win player is 1\n");
    }
    else if(score[0] < score[1]){
        printf("player2 gameover\n");
        printf("Win player is 2\n");
    }
    else{
        printf("draw gameover\n");
    }
}

int main(){
    MakeMap();
    printf("hello world\n");
    return 0;
}