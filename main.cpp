#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

int fps = 20;

int puyo_size[2] = {12, 6};//y, x
int tetris_size[2] = {20, 10}; 
//合わせるか
//ぷよぷよとテトリスわける？　なーるほど
//原作 テトリス 20x10 ぷよぷよ 12x6
//あわせるかな
int map[2][20][16];//[player][y][x]
int map_sub[2][8][19];//[player][y][x]

int puyopuyo_block[2][2][2];//[1st 2nd] [2] 1 2選択 [2] x y 
int puyopuyo_block_color[2][2][2];//[1st 2nd] now next選択 [2]  1 2　
int puyopuyo_block_hold[2][2];//[1st 2nd] 1 2選択 
int mode[2];//0:テトリス 1:ぷよぷよ
int current_blopuyo[2];//[player][block]
int current_position[2][2];
int next_blopuyo[2];//ぷよぷよの場合は十の位の数字と一の位の数字で二つのぷよの色を表す
int hold_blopuyo[2];
int block_direction[2];//回転回数
int wait_time[2] = {0};
int atack_power[2] = {0};

int fall_timer[2] = {800,800};
int control_timer[2] = {0};

int score[2] = {0};//スコア 

int currentBlock[2][4][4];//[player][y][x]

int next_tetris_block_number[2] = {0,0};

int before_key[12];

int players = 1;

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
    int temp[4][4]; 
    if(!modes){
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
        block_direction[player] = (block_direction[player] + 1) % 4;
        if(map[player][current_position[player][0] + int(sin(block_direction[player] * 90))][current_position[player][1] + int(cos(block_direction[player] * 90))]){
            current_position[player][0] -= int(sin(block_direction[player] * 90));
            current_position[player][1] -= int(cos(block_direction[player] * 90));
        }
        if (map[player][current_position[player][0]][current_position[player][1]]){
            current_position[player][0] += int(sin(block_direction[player] * 90));
            current_position[player][1] += int(cos(block_direction[player] * 90));
            block_direction[player] = (block_direction[player] + 3) % 4;
        }
        //puyopuyo pass;
    }
}

void HoldBlock(int player){
    if(!mode[player]){

    }
    else{
        current_blopuyo[player] += hold_blopuyo[player];
        hold_blopuyo[player] = current_blopuyo[player] - hold_blopuyo[player];
        current_blopuyo[player] -= hold_blopuyo[player];
    }
}
//落ちるぷよがあるかどうか毎回判定してた気がする
//そこ僕と違う処理方法だからそっちに合わせます。できるかはできるようするんです。
void PlaceBlock(int player,int put_x,int put_y){
    /*
    ブロックをマップに配置
    */
    //配置方法要検討
    if (!mode[player]){
        for(int y = 0; y < 4; y++){
            for(int x = 0; x < 4; x++){
                if(currentBlock[player][y][x]){
                    map[player][put_y + y][put_x + x] = currentBlock[player][y][x];
                }
            }
        }
    }
    else{
        map[player][put_y][put_x] = current_blopuyo[player] / 10;
        map[player][put_y + int(sin(block_direction[player] * 90))][put_x + int(sin(block_direction[player] * 90))] = current_blopuyo[player] % 10;
        current_blopuyo[player] = 0;
    }
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
        for(int x = 1; x < puyo_size[1] - 1; x++){
            for(int y = puyo_size[0] - 2; y > 0; y--){
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
        for(int y = tetris_size[0] - 2; y >= 0; y--){
            for(int x = 1; x < tetris_size[1] - 1; y++){
                if(map[player][y][x]){
                    break;
                }
                if(x == tetris_size[1] - 2){
                    for(int i = y; i >= 0; y--){
                        for(int j = 1; j < tetris_size[1] - 1; j++){
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
        for(int x = 0; x < puyo_size[1] - 1; x++){
            for(int y = puyo_size[0] - 2; y > 0; y--){
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
        for(int y = power; y < tetris_size[0] - 1; y++){
            for(int x = 1; x < tetris_size[1] - 1; x++){
                map[1 - player][y - power][x] = map[1 - player][y][x]; 
            }
        }
        for(int i = 0; i < power; i++){
            srand((unsigned int)time(NULL));
            int x = rand() % (tetris_size[1] - 2) + 1;
            for(int j = 1; j < tetris_size[1] - 1; j++){
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
        int start = rand() % (puyo_size[1] - 2) + 1;
        for (int y = 0; atack_power[player] > 0 && y < puyo_size[0] - 1; y++){
            for(int x = 0; atack_power[player] > 0 && x < puyo_size[1] - 1; x++){
                if(!map[1 - player][y][(x + start) % (puyo_size[1] - 2) + 1]){        
                    map[1 - player][y][(x + start) % (puyo_size[1] - 2) + 1] = 1;
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
    for(int y = 0; y < tetris_size[0] - 1; y++){
        for(int x = 1; x < tetris_size[1] - 1; x++){
            if(!map[player][y][x]){
                break;
            }
            if(x == tetris_size[1] - 2){
                for(int i = 1; i < tetris_size[1] - 1; i++){
                    map[player][y][i] = 0;
                }
            }
        }
    }
   }
   else{
        int searched_block[180] = {0};
        for(int y = 0; y < puyo_size[0] - 1; y++){
            for(int x = 1; x < puyo_size[1] - 1; x++){
                if(map[player][y][x] > 1 && !InArray(y * 100 + x, searched_block)){
                    searched_block[Len(searched_block)] = y * 100 + x;
                    int adjacent_block[180] = {0};
                    adjacent_block[0] = y * 100 + x;
                    for(int i = 0; adjacent_block[i]; i++){
                        for(int j = 0; j < 4; j++){
                            if(
                                !InArray(searched_block[i] + static_cast<int>(sin(j * 90) * 100 + cos(j * 90)), adjacent_block)
                                && adjacent_block[i] / 100 + sin(j * 90) > 0 
                                && adjacent_block[i] / 100 + sin(j * 90) < puyo_size[0] - 1 
                                && adjacent_block[i] % 100 + cos(j * 90) > 0 
                                && adjacent_block[i] % 100 + cos(j * 90) < puyo_size[1] - 1 
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
                   //回転するときの判定はバグの可能性大。引用元がバグあり。
                   //if文の魔人さん　出番です
            }//まあ一旦作ってみるわ

        }//if(CanPlaceBlock(player,mode,put_x,put_y)){PlaceBlock(player,mode,GameMode,put_x,put_y);}　こんな感じで使えるはず
   }else{
        //puyopuyo pass;
        if(map[player][put_y][put_x] || map[player][put_y + int(sin(block_direction[player] * 90))][put_x + int(cos(block_direction[player] * 90))]){
            return 0;//そろそろ落ちる
        }//了解
   }
    return 1;
}

void CheckArrange(){
    /*
    ブロックの配置を確認

    */
}

void MapShow(int player){
    /*
    マップの表示
    player 1・・・シングル 
    player 2・・・対戦
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
    system("cls");
    int map_YX[2][2] = {{20,16},{8,16}};//main sub画面切り替え sub mainの順
    for(int map_change = 1; map_change >= 0; map_change--){//main sub画面切り替え
        puts("");
        for(int y = 0; y < map_YX[map_change][0]; y++){//y
            for(int st = 0; st < player; st++){//player
                printf("  ");
                for(int x = 0; x < map_YX[map_change][1]; x++){//x
                    int block_number;
                    if(map_change == 1)
                    {
                        block_number = map_sub[st][y][x];
                    }else{
                        block_number = map[st][y][x];
                    }
                    // printf("%d ",block_number);
                    if (block_number == 1)
                    {
                        printf("\x1b[0m□ \x1b[0m");
                    }
                    else if (block_number == 2)
                    {
                        if(!mode[st]){
                            printf("\x1b[31m■ \x1b[0m");
                        }
                        else{
                            printf("\x1b[31m● \x1b[0m");
                        }
                    }
                    else if (block_number == 3)
                    {
                        if(!mode[st]){
                            printf("\x1b[32m■ \x1b[0m");
                        }
                        else{
                            printf("\x1b[32m● \x1b[0m");
                        }
                    }
                    else if (block_number == 4)
                    {
                        if(!mode[st]){
                            printf("\x1b[33m■ \x1b[0m");
                        }
                        else{
                            printf("\x1b[33m● \x1b[0m");
                        }
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
    system("cls");
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

void GemoModeSelect(){
    /*
    ゲームモード選択
    */
    printf("1:シングルプレイ\n2:対戦プレイ\n数字で入力してください\n");
    scanf("%d",&players);
    if (players == 1){
        printf("1:シングルモード\nゲームモードを選択してください\n 1:テトリス 2:ぷよぷよ\n");
        int modes;
        scanf("%d",&modes);
        mode[0] = modes;
    }else if(players == 2){
        printf("1:対戦モード\nゲームモードを選択してください\n 1:テトリス 2:ぷよぷよ\n");
        int modes;
        for (int i = 0; i < 2; i++)
        {
            printf("player%d:",i + 1);
            scanf("%d",&modes);
            mode[i] = modes;
        }
    }
}


int main(){
    MakeMap();
    printf("hello world\n");
    printf("%s",logo);
    GemoModeSelect();
    while (1)
    {

        //control
        if(control_timer[0] <= 0){
            if(GetAsyncKeyState('A') & 0x8000 && !before_key[0]){
                before_key[0] = 1;
                if(CanPlaceBlock(0, mode[0], current_position[0][1] - 1, current_position[0][0])){
                    current_position[0][1]--;
                    control_timer[0] = 100;
                }
            else if(GetAsyncKeyState('D') & 0x8000 && !before_key[1]){
                before_key[1] = 1;
                if(CanPlaceBlock(0, mode[0], current_position[0][1] + 1, current_position[0][0])){
                    current_position[0][1]++;
                    control_timer[0] = 100;
                }
            }
            else if(GetAsyncKeyState('W') & 0x8000 && !before_key[2]){
                before_key[2] = 1;
                Rotate_Block(0, mode[0]);
                control_timer[0] = 100;
            }
            else if(GetAsyncKeyState('S') & 0x8000 && !before_key[3]){
                before_key[3] = 1;
                if(CanPlaceBlock(0, mode[0], current_position[0][1] + 1, current_position[0][0])){
                    current_position[0][0]++;
                    control_timer[0] = 100;
                }
                else{
                    PlaceBlock(0, current_position[0][1], current_position[0][0]);
                }
            }
            else if(GetAsyncKeyState('Q') & 0x8000 && !before_key[4]){
                before_key[4] = 1;
                HoldBlock(0);
                control_timer[0] = 100;
            }


            if(!(GetAsyncKeyState('A') & 0x8000) && before_key[0]){
                before_key[0] = 0;
            }

            if(!(GetAsyncKeyState('D') & 0x8000) && before_key[1]){
                before_key[1] = 0;
            }

            if(!(GetAsyncKeyState('W') & 0x8000) && before_key[2]){
                before_key[2] = 0;
            }

            if(!(GetAsyncKeyState('S') & 0x8000) && before_key[3]){
                before_key[3] = 0;
            }

            if(!(GetAsyncKeyState('Q') & 0x8000) && before_key[4]){
                before_key[4] = 0;
            }
        
        }
        if(players > 1 && control_timer[1] <= 0){
            if(GetAsyncKeyState('J') & 0x8000 && !before_key[6]){
                before_key[6] = 1;
                if(CanPlaceBlock(1, mode[1], current_position[1][1] - 1, current_position[1][0])){
                    current_position[1][1]--;
                    control_timer[1] = 100;
                }
            }
            else if(GetAsyncKeyState('L') & 0x8000 && !before_key[7]){
                before_key[7] = 1;
                if(CanPlaceBlock(1, mode[1], current_position[1][1] + 1, current_position[1][0])){
                    current_position[1][1]++;
                    control_timer[1] = 100;
                }
            }
            else if(GetAsyncKeyState('I') & 0x8000 && !before_key[8]){
                before_key[8] = 1;
                Rotate_Block(1, mode[1]);
                control_timer[1] = 100;
            }
            else if(GetAsyncKeyState('K') & 0x8000 && !before_key[9]){
                before_key[9] = 1;
                if(CanPlaceBlock(1, mode[1], current_position[1][1] + 1, current_position[1][0])){
                    current_position[1][0]++;
                    control_timer[1] = 100;
                }
                else{
                    PlaceBlock(1, current_position[1][1], current_position[1][0]);
                }
            }
            else if(GetAsyncKeyState('U') & 0x8000 && !before_key[10]){
                before_key[10] = 1;
                HoldBlock(1);
                control_timer[1] = 100;
            }

            if(!(GetAsyncKeyState('J') & 0x8000) && before_key[6]){
                before_key[6] = 0;
            }

            if(!(GetAsyncKeyState('L') & 0x8000) && before_key[7]){
                before_key[7] = 0;
            }

            if(!(GetAsyncKeyState('I') & 0x8000) && before_key[8]){
                before_key[8] = 0;
            }

            if(!(GetAsyncKeyState('K') & 0x8000) && before_key[9]){
                before_key[9] = 0;
            }

            if(!(GetAsyncKeyState('U') & 0x8000) && before_key[10]){
                before_key[10] = 0;
            }

        }

        //delete and fall block
        for(int player = 0; player < players; player++){
            if(control_timer[player]){
                control_timer[player] -= 1000 / fps;
            }
            if(!control_timer[player]){
                if(!mode[player]){
                    if(CanDeleteBlock(player)){
                        DeleteBlock(player);
                        control_timer[player] = 500;
                    }
                    else if(CanBlockFall(player)){
                        FallBlock(player);
                        control_timer[player] = 300;
                    }
                }
                else{
                    if(CanBlockFall(player)){
                        FallBlock(player);
                        control_timer[player] = 250;
                    }
                    else if(CanDeleteBlock(player)){
                        DeleteBlock(player);
                        control_timer[player] = 250;
                    }
                }
            }
            if(!CanBlockFall(player) && !CanDeleteBlock(player)){
                if(fall_timer[player]){
                    fall_timer[player] -= 1000 / fps;
                }
                else{
                    current_position[player][0]++;
                    if(!CanPlaceBlock(player,mode[player],current_position[player][0], current_position[player][1])){
                        PlaceBlock(player, current_position[player][0] - 1, current_position[player][1]);
                        current_position[player][0] = 0;
                        current_position[player][1] = (mode[player] ? puyo_size[1] : tetris_size[1]) / 2 - 1;
                        MakeBlock(player);
                    }
                    fall_timer[player] = 800;
                }
            }
        }
        MapShow(2);
        Sleep(static_cast<DWORD>(1000 / fps));
        }
    
    }
}