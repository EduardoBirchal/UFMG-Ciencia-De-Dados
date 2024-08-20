#include <stdio.h>
#include <string.h>
#define MAX_POKEMON 6
#define MAX_NAME 100
#define FALSE 0
#define TRUE 1

struct pokemon_struct{
    char name[MAX_NAME];
    float attack;
    int defense;
    float hp;
    int type;
};

typedef struct pokemon_struct pokemon;

int stringToType(char *string){
    if(strcmp(string, "gelo") == 0)
        return 0;
    if(strcmp(string, "pedra") == 0)
        return 1;
    if(strcmp(string, "elétrico") == 0)
        return 2;
    if(strcmp(string, "água") == 0)
        return 3;
    else return 4;
}

int totalDamage(float attack, int defense){
    int totalDamage = attack - defense;
    if(totalDamage <= 0) 
        totalDamage = 1;
    return totalDamage;
}

int isWeak(int type1, int type2){
    if(type1 - 1 == type2) 
        return TRUE;
    if(type1 == 0 && type2 == 4) 
        return TRUE;
    else return FALSE;
}

int isStrong(int type1, int type2){
    if(type1 + 1 == type2) 
        return TRUE;
    if(type1 == 4 && type2 == 0) 
        return TRUE;
    else return FALSE;
}

float totalAttack(float attack, int type1, int type2){
    float totalAttack;
    if(isWeak(type1, type2))
        totalAttack = attack * 0.8;
    else if(isStrong(type1, type2))
        totalAttack = attack * 1.2;
    else
        totalAttack = attack;
    return totalAttack;
}

void printPoke(pokemon p){
    printf("\n");
    printf("Nome: %s\n", p.name);
    printf("Ataque: %f\n", p.attack);
    printf("Defesa: %d\n", p.defense);
    printf("Vida: %f\n", p.hp);
    printf("Tipo: %d\n", p.type);
    printf("\n");
}

int main(void){
    FILE* file;
    pokemon player1[MAX_POKEMON], player2[MAX_POKEMON];
    int total1, total2, current1 = 0, current2 = 0;
    int playerOnePlay = TRUE;
    char temp[MAX_NAME];

    file = fopen("pokemon.txt", "r");

    fscanf(file, " %d %d", &total1, &total2);
    
    for(int i = 0; i < total1; i++){
        fscanf(file, " %s %f %d %f %s", player1[i].name, &player1[i].attack, &player1[i].defense, &player1[i].hp, temp);
        player1[i].type = stringToType(temp);
    }

    for(int i = 0; i < total2; i++){   
        fscanf(file, " %s %f %d %f %s", player2[i].name, &player2[i].attack, &player2[i].defense, &player2[i].hp, temp);
        player2[i].type = stringToType(temp);
    }

    while((total1 - current1 > 0) && (total2 - current2 > 0)){
        if(playerOnePlay == TRUE){
            printf("%s ataca %s por %d de dano (%.0f - %d)\n", player1[current1].name, player2[current2].name, totalDamage(totalAttack(player1[current1].attack, player1[current1].type, player2[current2].type), player2[current2].defense), totalAttack(player1[current1].attack, player1[current1].type, player2[current2].type), player2[current2].defense);
            player2[current2].hp -= totalDamage(totalAttack(player1[current1].attack, player1[current1].type, player2[current2].type), player2[current2].defense);
            if(player2[current2].hp <= 0){
                current2++;
                printf("%s venceu %s\n", player1[current1].name, player2[current2 - 1].name);
            }
            playerOnePlay = FALSE;
        }
        else{
            printf("%s ataca %s por %d de dano (%.0f - %d)\n", player2[current2].name, player1[current1].name, totalDamage(totalAttack(player2[current2].attack, player2[current2].type, player1[current1].type), player1[current1].defense), totalAttack(player2[current2].attack, player2[current2].type, player1[current1].type), player1[current1].defense);

            player1[current1].hp -= totalDamage(totalAttack(player2[current2].attack, player2[current2].type, player1[current1].type), player1[current1].defense);
            if(player1[current1].hp <= 0){
                current1++;
                printf("%s venceu %s\n", player2[current2].name, player1[current1 - 1].name);
            }
            playerOnePlay = TRUE;
        }
    }

    if(total1 - current1 == 0)
        printf("Jogador 2 venceu \n");
    else
        printf("Jogador 1 venceu \n");

    printf("Pokemon sobreviventes: \n");
    for(int i = current1; i < total1; i++) 
        printf("%s\n", player1[i].name);
    for(int i = current2; i < total2; i++) 
        printf("%s\n", player2[i].name);

    printf("Pokemon derrotados: \n");
    for(int i = 0; i < current1; i++)
        printf("%s ", player1[i].name);
    for(int i = 0; i < current2; i++) 
        printf("%s ", player2[i].name);
        
    printf("\n");

    return 0;
}