#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

int puyo_size[2] = {12, 6};//y, x
int tetris_size[2] = {20, 10}; 
//合わせるか
//ぷよぷよとテトリスわける？　なーるほど
//原作 テトリス 20x10 ぷよぷよ 12x6
//あわせるかな
int map[2][20][10];//[player][y][x]
int map_sub[2][8][19];//[player][y][x]

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

int before_key[8];

char logo[999] ={"    * ### ##   ##  ###  ##  ##    ## ##   ### ##   ##  ###  ##  ##    ## ##   #### ##  ### ###  #### ##  ### ##     ####    ## ## *\n*  ##  ##  ##   ##  ##  ##   ##   ##   ##  ##  ##   ##  ##  ##   ##   ##  # ## ##   ##  ##  # ## ##   ##  ##     ##    ##   ## *\n*  ##  ##  ##   ##  ##  ##   ##   ##   ##  ##  ##   ##  ##  ##   ##   ##    ##      ##        ##      ##  ##     ##    #### *\n*  ##  ##  ##   ##   ## ##   ##   ##   ##  ##  ##   ##   ## ##   ##   ##    ##      ## ##     ##      ## ##      ##     ##### *\n*  ## ##   ##   ##    ##     ##   ##   ## ##   ##   ##    ##     ##   ##    ##      ##        ##      ## ##      ##        ### *\n*  ##      ##   ##    ##     ##   ##   ##      ##   ##    ##     ##   ##    ##      ##  ##    ##      ##  ##     ##    ##   ## *\n* ####      ## ##     ##      ## ##   ####      ## ##     ##      ## ##    ####    ### ###   ####    #### ##    ####    ## ## *\n"};
BYTE keyboardState[256];

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

        // for(int y = 0; y < 20; y++){
        //     for(int x = 0; x < 16; x++){
        //         printf("map[%d][%d][%d]:%d\n",num,y,x,map[num][y][x]);
        //     }
        //     printf("\n");
        // }

        // for(int y = 0; y < 8; y++){
        //     for(int x = 0; x < 16; x++){
        //         printf("map_sub[%d][%d][%d]:%d\n",num,y,x,map_sub[num][y][x]);
        //     }
        //     printf("\n");
        // }
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

void Rotate_Block(int player, int modes){
    /*
    ブロックの回転
    */
    block_direction[player] = (block_direction[player] + 1) % 4;
    //int temp[4][4]; 
    //if(!mode){
    //    for(int c = 0; c < 2; c++){
    //        for(int y = 0; y < 4; y++){
    //            for(int x = 0; x < 4; x++){
    //                if(!c){
    //                    temp[y][x] = currentBlock[player][y][x];
    //                }
    //                else{
    //                    currentBlock[player][y][x] = temp[3 - x][y];
    //                }
    //            }
    //        }
    //    }
    //}else{
    //    //puyopuyo pass;
    //}
}
//落ちるぷよがあるかどうか毎回判定してた気がする
//そこ僕と違う処理方法だからそっちに合わせます。できるかはできるようするんです。
void PlaceBlock(int player,int mode, int GameMode,int put_x,int put_y){
    /*
    ブロックをマップに配置
    */
   //配置方法要検討
}

int Len(int array[]){
    int i;
    for(i = 0; array[i]; i++){
        if(!array[i]){
            break;
        }
    }
    return i;
}

int InArray(int element, int array[]){
    for(int i = 0; array[i]; i++){
        if(array[i] == element){
            return 1;
        }
    }
    return 0;
}

int CanDeleteBlock(int player){
    if(!mode[player]){
        for(int y = tetris_size[0] - 2; y >= 0; y--){
            for(int x = 1; x < tetris_size[1] - 1; x++){
                if(!map[player][y][x]){
                    break;
                }
                if(x == tetris_size[1] - 2){
                    return 1;
                }
            }
        }
    }
    else{
        int searched_block[180] = {0};
        for(int y = 0; y < puyo_size[0] - 1; y++){
            for(int x = 1; x < puyo_size[1] - 1; x++){
                if(map[player][y][x] && !InArray(y * 100 + x, searched_block)){
                    searched_block[Len(searched_block)] = y * 100 + x;
                    int adjacent_block[3] = {0};
                    adjacent_block[0] = y * 100 + x;
                    for(int i = 0; i < 4 && adjacent_block[i]; i++){
                        for(int j = 0; j < 4; j++){//
                            if(
                                adjacent_block[i] / 100 + sin(j * 90) > 0 
                                && adjacent_block[i] / 100 + sin(j * 90) < puyo_size[0] - 1 
                                && adjacent_block[i] % 100 + cos(j * 90) > 0 
                                && adjacent_block[i] % 100 + cos(j * 90) < puyo_size[1] - 1 
                                && map[player][adjacent_block[i] / 100][adjacent_block[i] % 100] == map[player][int(adjacent_block[i] / 100 + sin(j * 90))][int(adjacent_block[i] % 100 + cos(j * 90))]
                                )
                            {
                                if(Len(adjacent_block) == 3){
                                    return 1;
                                }
                                adjacent_block[Len(adjacent_block)] = adjacent_block[i] + static_cast<int>(sin(j * 90) * 100 + cos(j * 90));
                                searched_block[Len(searched_block)] = adjacent_block[i] + static_cast<int>(sin(j * 90) * 100 + cos(j * 90));
                            }
                        }
                    }//変数作っときましょう
                }//マップの大きさ、後々変更しやすいよう変数にしとく？
            }//意外とこれ調整がんばらんとバランス崩れる。今ぷよぷよのサイズ的にはいいけど、テトリスは狭いかも
        }
    }
    return 0;
}

int CanBlockFall(int player){
    if(!mode[player]){
        for(int y = tetris_size[0] - 2; y >= 0; y--){
            for(int x = 1; x < tetris_size[1] - 1; x++){
                if(map[player][y][x]){
                    break;
                }
                if(x == tetris_size[1] - 2){
                    return 1;
                }
            }
        }//僕も風呂行ってくるわ
    }//huro mainの画面できたけどsubの画面に試行錯誤中。れっつごー
    else{
        for(int x = 1; x < 9; x++){
            for(int y = 14; y > 0; y--){
                if(!map[player][y][x]){
                    for(y--; y >= 0; y--){
                        if(map[player][y][x]){
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void FallBlock(int player){//多分できた
    /*
    ブロックを落下させる
    */
    if(!mode[player])
    {
        for(int y = 14; y >= 0; y--){
            for(int x = 1; x < 9; y++){
                if(map[player][y][x]){
                    break;
                }
                if(x == 12){
                    for(int i = y; i >= 0; y--){
                        for(int j = 1; j < 9; j++){
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
        for(int x = 1; x < 9; x++){
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
            for(int x = 1; x < 9; x++){
                map[1 - player][y - power][x] = map[1 - player][y][x]; 
            }
        }
        for(int i = 0; i < power; i++){
            srand((unsigned int)time(NULL));
            int x = rand() % 8 + 1;
            for(int j = 1; j < 9; j++){
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
        int start = rand() % 8 + 1;
        for (int y = 0; atack_power[player] > 0 && y < 15; y++){
            for(int x = 0; atack_power[player] > 0 && x < 9; x++){
                if(!map[1 - player][y][(x + start) % 8 + 1]){        
                    map[1 - player][y][(x + start) % 8 + 1] = 1;
                    atack_power[player]--;
                }//がんばってーいきましょう．時間あったらなるべく進めとく
            }
        }//共闘とおいきましょう
    }
}

void DeleteBlock(int player){
    /*
    ブロックをマップから削除
    */
   if(!mode[player]){
    for(int y = 0; y < 15; y++){
        for(int x = 1; x < 9; x++){
            if(!map[player][y][x]){
                break;
            }
            if(x == 8){
                for(int i = 1; i < 9; i++){
                    map[player][y][i] = 0;
                }
            }
        }
    }
   }
   else{
        int searched_block[180] = {0};
        for(int y = 0; y < 15; y++){
            for(int x = 1; x < 9; x++){
                if(map[player][y][x] > 1 && !InArray(y * 100 + x, searched_block)){
                    searched_block[Len(searched_block)] = y * 100 + x;
                    int adjacent_block[180] = {0};
                    adjacent_block[0] = y * 100 + x;
                    for(int i = 0; adjacent_block[i]; i++){
                        for(int j = 0; j < 4; j++){
                            if(
                                !InArray(searched_block[i] + static_cast<int>(sin(j * 90) * 100 + cos(j * 90)), adjacent_block)
                                && adjacent_block[i] / 100 + sin(j * 90) > 0 
                                && adjacent_block[i] / 100 + sin(j * 90) < 15 
                                && adjacent_block[i] % 100 + cos(j * 90) > 0 
                                && adjacent_block[i] % 100 + cos(j * 90) < 9 
                                && map[player][adjacent_block[i] / 100][adjacent_block[i] % 100] == map[player][int(adjacent_block[i] / 100 + sin(j * 90))][int(adjacent_block[i] % 100 + cos(j * 90))]
                                )
                            {
                                adjacent_block[Len(adjacent_block)] = adjacent_block[i] + static_cast<int>(sin(j * 90) * 100 + cos(j * 90));
                                searched_block[Len(searched_block)] = adjacent_block[i] + static_cast<int>(sin(j * 90) * 100 + cos(j * 90));
                            }
                        }
                    }
                    if(Len(adjacent_block) > 3){
                        for(int i = 0; adjacent_block[i]; i++){
                            map[player][adjacent_block[i] / 100][adjacent_block[i] % 100] = 0;
                            for(int j = 0; j < 4; i++){
                                if(map[player][int(adjacent_block[i] / 100 + sin(j * 90))][int(adjacent_block[i] % 100 + cos(j * 90))] == 1)
                                {
                                    map[player][int(adjacent_block[i] / 100 + sin(j * 90))][int(adjacent_block[i] % 100 + cos(j * 90))]--;
                                }
                            }
                        }
                    }
                }
            }
        }
   }
}

int CanPlaceBlock(int player,int modes, int put_x, int put_y){
    /*
    ブロックをマップに配置できるか
    */
    if(!modes){
        for(int y = 0; y<4; y++){
            for(int x = 0; x<4; x++){
                int ys = put_y + y;
                int xs = put_x + x;
                int bls = currentBlock[player][y][x];
                if((xs <= 13 && xs >= 0)&&(ys < 0 || ys >= 16 || xs < 9 || xs >= 14 || (bls >= 1 && bls <= 14)) && ((map[player][ys][xs] >= 1 && map[player][ys][xs] <= 14)))
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
    return 1;
}

void CheckArrange(){
    /*
    ブロックの配置を確認

    */
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
            for(int x = 0; x < map_YX[map_change][1]; x++){//x
                for(int st = 0; st < modes; st++){//player
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
        // printf("\n\n");
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
    printf("%s",logo);
    printf("hello world\n");
    while (1)
    {
        //キーボード入力サンプル
        if (GetAsyncKeyState('A') & 0x8000) {
            if(!before_key[0]){
                printf("a push\n");
            }
            before_key[0] = 1;
        }else{
            before_key[0] = 0;
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            if(!before_key[1]){
                printf("d push\n");
            }
            before_key[1] = 1;
        }else{
            before_key[1] = 0;
        }
        if (GetAsyncKeyState('W') & 0x8000) {
            if(!before_key[2]){
                printf("w push\n");
            }
            before_key[2] = 1;
        }else{
            before_key[2] = 0;
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            if(!before_key[3]){
                printf("s push\n");
            }
            before_key[3] = 1;
        }else{
            before_key[3] = 0;
        }
        MapShow(1);
    }
    
}