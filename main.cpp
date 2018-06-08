#include <iostream>
#include <cmath>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dos.h>
#include <dir.h>
#include <conio.h>
#include <fstream>
#include <time.h>

using namespace std;

struct peca{// contém o tipo e o time de cada peça
    char tipo='m';// por padrão as peças são do tipo "morto" e tem time ==0, assim como estão em sua primeira jogada
    bool primeiro=true;
    int time=0;
};

struct tabuleiro{// srtuct que contem  64 "peças" e o time atacante
    peca pos[8][8];
    int atacante=1;
};

void menu();
void xadrez();
void start(tabuleiro &tab);
void imprimetabuleiro(tabuleiro tab);
bool peao(tabuleiro tab,int x0,int y0, int x, int y);
bool bispo(tabuleiro tab,int x0,int y0, int x, int y);
bool torre(tabuleiro tab, int x0, int y0, int x, int y);
bool dama(tabuleiro tab, int x0, int y0, int x, int y);
bool cavalo(tabuleiro tab, int x0, int y0, int x, int y);
bool movimento(tabuleiro &tab,int x0,int y0, int x, int y);
void jogada(tabuleiro &tab, int t1, int t2);
bool acabou(tabuleiro tab,int t1, int t2);
bool cheque(tabuleiro tab,int x,int y,int time);
bool rei(tabuleiro tab,int x0, int y0,int x, int y);
void salvar(tabuleiro tab,int t1, int t2);
void carregar(tabuleiro &tab,int &t1, int &t2);
void setcolor(int ForgC);
void relogio(tabuleiro tab,time_t delta,int &t1,int &t2);

int main()
{
    menu();
    return 0;
}

void menu(){// menu simples
    short int op;
    cout<<"Selecione uma opcao:"<<endl<<"1 - Xadrez"<<endl<<"0 - Sair"<<endl;
    cin>>op;
    switch (op){
    case 1:
        xadrez();
        break;
    case 0:
        break;
    }
    return;
}

void xadrez(){// função que executa o jogo de xdrez prorpiamente dito, é a função principal do jogo e chama as outras funções basicas do jogo
    tabuleiro tab;
    int op;
    int t1=3600,t2=3600;
    time_t str, delta;
    system("@cls||clear");
    cout<<"1- Carregar jogo"<<endl<<"2- Novo Jogo (apaga o antigo save)"<<endl;
    cin>>op;
    do{
        switch (op){
        case 1:
            carregar(tab,t1,t2);
            break;
        case 2:
            start(tab);
            break;
        default:
            cout<<"Opção invalida"<<endl;
        }
    }while((op!=1)&&(op!=2));
    do{
        system("@cls||clear");
        str=time(NULL);
        imprimetabuleiro(tab);
        jogada(tab,t1,t2);
        delta=time(NULL)-str;
        relogio(tab,delta,t1,t2);
        salvar(tab,t1,t2);
    }while(!acabou(tab,t1,t2));

    return;
}

void start(tabuleiro &tab){// inicia um novo tabuleiro
    for(int i=0;i<8;i+=(1+4*(i%6))){
        for(int j=0;j<8;j++){
            if(i==1){
                tab.pos[i][j].tipo='p';// inicia os peoes
                tab.pos[i][j].time=-1;
            }
            else if(i==6){
                tab.pos[i][j].tipo='p';
                tab.pos[i][j].time=1;
            }
            else{
                switch (j){
                    case 3:
                        tab.pos[i][j].tipo='R';//inicia os reis
                        tab.pos[i][j].time=-1+2*(i%6);
                        break;
                    case 4:
                        tab.pos[i][j].tipo='D';// inicia as damas
                        tab.pos[i][j].time=-1+2*(i%6);
                        break;
                    case 0:
                    case 7:
                        tab.pos[i][j].tipo='T';//inicia as torres
                        tab.pos[i][j].time=-1+2*(i%6);
                        break;
                    case 1:
                    case 6:
                        tab.pos[i][j].tipo='C';//inicia os cavalos
                        tab.pos[i][j].time=-1+2*(i%6);
                        break;
                    case 2:
                    case 5:
                        tab.pos[i][j].tipo='B';// inicia os bispos
                        tab.pos[i][j].time=-1+2*(i%6);
                        break;
                }
            }
        }
    }
}

void imprimetabuleiro(tabuleiro tab){//imprime o tabuleiro
    char base='A';
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(tab.pos[i][j].tipo!='m'){
                if((i+j)%2==0){
                    setcolor(6);
                }
                else
                    setcolor(7);
                cout<<"[";
                if(tab.pos[i][j].time==1){// muda a cor da peça baseado no time
                    setcolor(5);
                }
                else if(tab.pos[i][j].time==-1){
                    setcolor(3);
                }
                cout<<tab.pos[i][j].tipo;
                if((i+j)%2==0){
                    setcolor(6);
                }
                else
                    setcolor(7);
                cout<<"]";
            }
            else {
                if((i+j)%2==0){
                    setcolor(6);
                }
                else
                    setcolor(7);
                    cout<<"[ ]";
            }
        }
        setcolor(7);
        cout<<i+1<<endl; // imprime os numeros nas laterais
    }
    for(int h=0;h<8;h++){
        cout<<" "<<char(base+h)<<" ";// imprime as letras da base
    }
}

bool peao(tabuleiro tab,int x0,int y0, int x, int y){//avalia o movimento do peao, se for a primeira jogada pode andar dois, se tiver um alvo diretamente a sua diagonal pode andar na diagonal
    if(((y==y0)&&(tab.pos[x0][y0].time*(x-x0)==-1))&&(tab.pos[x][y].time==0)){
        return true;// movimento basico
    }
    else if(((y==y0)&&(tab.pos[x0][y0].time*(x0-x)==2)&&(tab.pos[x0][y0].primeiro==true))){
        return true;// movimento inicial
    }
    else if((-1*tab.pos[x0][y0].time==tab.pos[x][y].time)&&(abs(x-x0)+abs(y-y0)==2)&&(tab.pos[x0][y0].time*(x-x0)==-1)){
        return true;// comer peça na diagonal
    }

    return false;
}

bool bispo(tabuleiro tab,int x0,int y0, int x, int y){//checa se o não há impecilhos par o movimento do bispo
    if(abs(x-x0)!=abs(y-y0))
        return false;
    else {
        for(int i=1;i<abs(x-x0);i++){
            if(tab.pos[x0+(((x-x0)/abs(x-x0))*i)][y0+(((y-y0)/abs(y-y0))*i)].tipo!='m'){
                return false;// usa um conceito parecido com o de vetores para determimnar a direção em que o bispo de move e assim checa essa diagonal
            }
        }
    }
    return true;
}

bool torre(tabuleiro tab, int x0, int y0, int x, int y){// checa o caminho da torre
    if((y!=y0)&&(x!=x0))
        return false;
    else if(y==y0){
        for(int i=min(x0,x)+1;i<max(x0,x);i++){
            if(tab.pos[i][y0].tipo!='m'){
                return false;
            }
        }
    }
     else if(x==x0){
        for(int j=min(y0,y)+1;j<max(y0,y);j++){
            if(tab.pos[x0][j].tipo!='m'){
                return false;
            }
        }
    }

    return true;
}

bool dama(tabuleiro tab, int x0, int y0, int x, int y){//checa o caminho da dama
    if(bispo(tab,x0,y0,x,y)||torre(tab,x0,y0,x,y)){
        return true;// se a dama fizer movimento do tipo bispo ou torre o mesmo é considerado válido
    }
    else
    return false;
}

bool cavalo(tabuleiro tab, int x0, int y0, int x, int y){//checa o movimento do cavalo
    if((abs(x-x0)>0)&&(abs(y-y0)>0)&&(abs(x-x0)+abs(y-y0)==3))
        return true;// o cavalo se move tres espaços e tem que se mover pelo menos um espaço em ambas as direções para realizar um movimento em "L"
    else
        return false;
}

bool cheque(tabuleiro tab,int x,int y, int time){//checa se uma posição está em cheque
    if(time==-1){
        if((tab.pos[x+1][y+1].tipo=='p')&&(tab.pos[x+1][y+1].time==1)){//checa peões
            return true;
        }
        if((tab.pos[x+1][y-1].tipo=='p')&&(tab.pos[x+1][y-1].time==1)){//checa peões
            return true;
        }
    }
    if(time==1){
        if((tab.pos[x-1][y+1].tipo=='p')&&(tab.pos[x-1][y+1].time==-1)){//checa peões
            return true;
        }
        if((tab.pos[x-1][y-1].tipo=='p')&&(tab.pos[x-1][y-1].time==-1)){//checa peões
            return true;
        }
    }
    for(int i=x+1;i<8;i++){
        if(((tab.pos[i][y].tipo=='T')||(tab.pos[i][y].tipo=='D'))&&(time==-1*tab.pos[i][y].time)){
                return true;//checa a reta inferior a posição por torres ou damas
            }
        else if(tab.pos[i][y].time!=0){
                break;
        }
    }

    for(int i=x-1;i>=0;i--){
        if(((tab.pos[i][y].tipo=='T')||(tab.pos[i][y].tipo=='D'))&&(time==-1*tab.pos[i][y].time)){
                return true;//checa a reta inferior a posição por torres ou damas
            }
            else if(0!=tab.pos[i][y].tipo){
                break;
        }
    }

    for(int i=y+1;i<8;i++){
        if(((tab.pos[x][i].tipo=='T')||(tab.pos[x][i].tipo=='D'))&&(time==-1*tab.pos[x][i].time)){
                return true;//checa a reta direita a posição por torres ou damas
            }
            else if(0!=tab.pos[i][y].time){
                break;
        }
    }

    for(int i=y-1;i>=0;i--){
        if(((tab.pos[x][i].tipo=='T')||(tab.pos[x][i].tipo=='D'))&&(time==-1*tab.pos[x][i].time)){
                return true;//checa a reta esquerda a posição por torres ou damas
            }
            else if(0!=tab.pos[i][y].time){
                break;
        }
    }

    for(int i=1;i<8-x&&i<8-y&&i<=x&&i<=y;i++){
        if(((tab.pos[x+i][y+i].tipo=='B')||(tab.pos[x+i][y+i].tipo=='D'))&&(time==-1*tab.pos[x+i][y+i].time)){
            return true;//checa a diagonal inferior direita a posição por bispos ou damas
        }
        else if(0!=tab.pos[i][i].time){
                break;
        }
    }

   for(int i=1;i<=x&&i<=y;i++){
        if(((tab.pos[x-i][y-i].tipo=='B')||(tab.pos[x-i][y-i].tipo=='D'))&&(time==-1*tab.pos[x-i][y-i].time)){
            return true;//checa a diagonal superior esquerda a posição por bispos ou damas
        }
        else if(0!=tab.pos[x-i][y-i].time){
                break;
        }
    }

    for(int i=1;i<8-x&&i<=y;i++){
        if(((tab.pos[x+i][y-i].tipo=='B')||(tab.pos[x+i][y-i].tipo=='D'))&&(time==-1*tab.pos[x+i][y-i].time)){
            return true;//checa a diagonal inferior esquerda a posição por bispos ou damas
        }
        else if(0!=tab.pos[x+i][y-i].time){
                break;
        }
    }

    for(int i=1;i<8-y&&i<=x;i++){
        if(((tab.pos[x-i][y+i].tipo=='B')||(tab.pos[x-i][y+i].tipo=='D'))&&(time==-1*tab.pos[x-i][y+i].time)){
            return true;//checa a diagonal superior direita a posição por bispos ou damas
        }
        else if(0!=tab.pos[x-i][y+i].time){
                break;
        }
    }

    for(int i=x-2;i<=x+2;i++){
        for(int j=y-2;j<=y+2;j++){
            if((i>=0)&&(j>=0)&&(i<8)&&(j<8)){
                if((abs(x-i)+abs(y-j)==3)&&(abs(x-i)>0)&&(abs(y-j)>0)){
                    if((tab.pos[i][j].time==-1*tab.pos[x][y].time)&&(tab.pos[i][j].tipo=='C')){
                        return true;//checa a posição de possiveis cavalos que ponham em cheque a posição
                    }
                }
            }
        }
    }

    return false;
}

bool rei(tabuleiro tab,int x0, int y0,int x, int y){// checa o movimento do rei
    if(cheque(tab,x,y,tab.pos[x0][y0].time)){// não pode se mover para posições em cheque
        return false;
    }
    if((abs(x-x0)>1)||(abs(y-y0)>1)){//pode se mover em qualquer direção, desde que no maximo uma casa por dirção
        return false;
    }
    return true;
}

bool movimento(tabuleiro &tab,int x0,int y0, int x, int y){//checa se o movimento é válido para o tipo de peça, com o auxilio de funções específicas
    if(tab.pos[x0][y0].time!=tab.atacante)// recusa movimentos de time diferente do atacante
        return false;
    if(x0>7||y0>7||x>7||y>7||x0<0||y0<0||x<0||y<0||(x0==x&&y0==y))// recusa movimentos nulos ou fora do tabuleiro
        return false;
    if(tab.pos[x0][y0].time==tab.pos[x][y].time)// recusa movimentos que chegam em posições ocupada pelo mesmo time da peça atacante
        return false;
    if(tab.pos[x0][y0].tipo!='R'){// checa se o rei está em cheque e "obriga" a move-lo se o mesmo estiver em cheque
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                if((tab.pos[i][j].tipo=='R')&&(tab.pos[i][j].time==tab.atacante)&&(cheque(tab,i,j,tab.pos[i][j].time))){
                        cout<<"Rei em cheque"<<endl;
                    return false;
                }
            }
        }
    }
    switch (tab.pos[x0][y0].tipo){// chama  as funções criadas acima para avaliar o movimento, baeado no tipo de peça
    case 'm':
        return false;// retorna falso para movimentos de peças "mortas"
    case 'p':
        if(peao(tab,x0,y0,x,y)){
            return true;
        }
        break;
    case 'B':
        if(bispo(tab,x0,y0,x,y)){
            return true;
        }
        break;
    case 'T':
       if(torre(tab,x0,y0,x,y)){
            return true;
        }
        break;
    case 'C':
        if(cavalo(tab,x0,y0,x,y)){
            return true;
        }
        break;
    case 'D':
        if(dama(tab,x0,y0,x,y)){
            return true;
        }
        break;
    case 'R':
       if(rei(tab,x0,y0,x,y)){
            return true;
        }
        break;
}
    return false;
}

bool acabou(tabuleiro tab,int t1, int t2){//condição de término de jogo
    int mate=0;
    if(t2<=0){
        cout<<endl<<"Vitoria por tempo do time roxo !";
        return true;
    }
    if(t1<=0){
        cout<<endl<<"Vitoria por tempo do time azul !";
        return true;
    }
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(tab.pos[i][j].tipo=='R'){//checa o tabuleiro e procura o rei para saber se o mesmo esta em cheque
                if(cheque(tab,i,j,tab.pos[i][j].time)){
                    for(int k=i-1;k<=i+1;k++){// se o rei está em cheque então checamos as posições ao seu redor, se elas forem não existentes, estiverem ocupadas ou em cheque adicionamos um ao contador
                        for(int l=j-1;l<=j+1;l++){
                            if(cheque(tab,k,l,tab.pos[i][j].time)||tab.pos[k][l].tipo!='m'||k<0||l<0||k>8||l>8){
                                mate++;// se o rei está em cheque então checamos as posições ao seu redor, se elas forem não existentes, estiverem ocupadas ou em cheque adicionamos um ao contador
                            }
                        }
                    }
                }
                if(mate==9){// se o contador==9 então o rei não tem movimentos para escapar, então o jogo acabou
                    system("@cls||clear");
                    imprimetabuleiro(tab);
                    if(tab.pos[i][j].time==1){
                        cout<<endl<<"Vitoria do time azul !";
                    }
                    if(tab.pos[i][j].time==-1){
                        cout<<endl<<"Vitoria do time roxo !";
                    }
                    return true;
                }
                mate=0;
            }
        }
    }

    return false;
}

void jogada(tabuleiro &tab, int t1, int t2){// faz o cin da jogada e executa o movimento, se o mesmo for válido
    int x0,y0,x,y;
    char y0c,yc;
    do{//cin da jogada
    if(tab.atacante==1){
        cout<<endl<<"Vez do time Roxo:"<< endl<< "Tempo restante: "<<t1/60<<" : "<<t1%60;
    }
    if(tab.atacante==-1){
        cout<<endl<<"Vez do time Azul:"<< endl<< "Tempo restante: "<<t2/60<<" : "<<t2%60;
    }
    cout<<endl<<"Escolha a peca a ser movida (letra e entao numero):"<<endl;
    cin>>y0c>>x0;
    y0c=tolower(y0c);
    y0=y0c-97;x0-=1;
    cout<<endl<<"Time: "<<tab.pos[x0][y0].time<<"  Tipo:  "<<tab.pos[x0][y0].tipo<<endl;
    cout<<"Escolha o local de destino (letra e entao numero):"<<endl;
    cin>>yc>>x;
    yc=tolower(yc);
    y=yc-97;x--;
    if(!movimento(tab,x0,y0,x,y)){
        system("@cls||clear");
        cout<<"Jogada invalida, tente novamente."<<endl;
        imprimetabuleiro(tab);
    }
    }while(!movimento(tab,x0,y0,x,y));
    tab.pos[x0][y0].primeiro=false;//execução do movimento
    tab.pos[x][y]=tab.pos[x0][y0];
    tab.pos[x0][y0].tipo='m';
    tab.pos[x0][y0].time=0;
    tab.atacante*=-1;
}

void salvar(tabuleiro tab, int t1, int t2){// salva o jogo
     ofstream out;
     out.open("Xadrez_save.txt");
     out<<t1<<endl;
     out<<t2<<endl;
     out<<tab.atacante<<endl;
     for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                out<<tab.pos[i][j].time<<endl;
                out<<tab.pos[i][j].tipo<<endl;
                out<<tab.pos[i][j].primeiro<<endl;
            }
        }
    out.close();
}

void carregar(tabuleiro &tab,int &t1, int &t2){// carrega o jogo salvo
    ifstream in;
     in.open("Xadrez_save.txt");
     in>>t1;
     in>>t2;
     in>>tab.atacante;
     for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                in>>tab.pos[i][j].time;
                in>>tab.pos[i][j].tipo;
                in>>tab.pos[i][j].primeiro;
            }
        }

    return;
}

void relogio(tabuleiro tab,time_t delta,int &t1,int &t2){// faz a contagem de tempo sobrando no relogio de cada jogador
    if(tab.atacante==1){
        t2-=delta;
    }
    if(tab.atacante==-1){
        t1-=delta;
    }
}

void setcolor(int ForgC)//função de colorir as peças
{
 WORD wColor;

  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

                       //We use csbi for the wAttributes word.
 if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
 {
                 //Mask out all but the background attribute, and add in the forgournd color
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
 }
 return;
}
