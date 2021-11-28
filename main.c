#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rpg.h"

//Returns a pointer to an Item after initialising it with the given parameters.
Item* newItem(int id, int damage, int durability, int quantity, float protection, int heal){
    Item* item = malloc(sizeof(Item));
    item->id = id;
    item->damage = damage;
    item->durability = durability;
    item->quantity = quantity;
    item->protection = protection;
    item->heal = heal;
    return item;
}

//Returns a pointer to a Monster after initialising it with the given parameters.
Monster* newMonster(char* name, int id, int HP, int attack, int expDrop){
    if(id < 12 || id > 99){
        return NULL;
    }
    Monster* monster = malloc(sizeof(Monster));
    monster->name = name;
    monster->id = id;
    monster->HP = HP;
    monster->attack = attack;
    monster->expDrop = expDrop;
    return monster;
}

//Returns a pointer to a PNJ after initialising it with an empty inventory.
PNJ* newPNJ(){
    PNJ* pnj = malloc(sizeof(PNJ));
    for (int i = 0; i < 255; ++i) {
        pnj->inventory[i] = NULL;
    }
    pnj->inventoryNextSpace = 0;
    return pnj;
}

//Adds an already initialised Item to the Player's inventory.
void addToPlayerInventory(Player* player, Item* item){
    player->inventory[player->inventoryNextSpace] = item;
    player->inventoryNextSpace++;
}

//Removes the item at the given index of a player's inventory
void removeFromPlayerInventory(Player* player, int index){
    int j = 0;
    for (int i = 0; i < player->inventoryNextSpace; i++){
        if(i != index){
            player->inventory[j] = player->inventory[i];
            j++;
        } else {
            free(player->inventory[i]);
        }
    }
    player->inventoryNextSpace -= 1;
    for (int i = player->inventoryNextSpace; i < 10; ++i) {
        player->inventory[i] = NULL;
    }
}

void removeFromPNJInventory(PNJ * pnj, int index){
    int j = 0;
    for (int i = 0; i < pnj->inventoryNextSpace; i++){
        if(i != index){
            pnj->inventory[j] = pnj->inventory[i];
            j++;
        } else {
            free(pnj->inventory[i]);
        }
    }
    pnj->inventoryNextSpace -= 1;
    for (int i = pnj->inventoryNextSpace; i < 10; ++i) {
        pnj->inventory[i] = NULL;
    }
}

//Adds an already initialised Item to a PNJ's inventory.
void addToPNJInventory(PNJ* pnj, Item* item){
    pnj->inventory[pnj->inventoryNextSpace] = item;
    pnj->inventoryNextSpace++;
}

//adds a thing to respawn to the respawn linked list
thingToRespawn* addToRespawnList(thingToRespawn* head, int type, int x, int y, int map, int nbturns){
    thingToRespawn* newThing = malloc(sizeof(thingToRespawn));
    newThing->type = type;
    newThing->x = x;
    newThing->y = y;
    newThing->map = map;
    newThing->turnsUntilRespawn = nbturns;
    newThing->next = NULL;

    if(head != NULL){
        while (head->next != NULL){
            head = head->next;
        }
        head->next = newThing;
    }
    return newThing;
}

int updateTimerToRespawn(thingToRespawn* head){
    int count = 0;
    int stop = 0;
    if(head != NULL){
        do {
            head->turnsUntilRespawn -= 1;
            if(head->turnsUntilRespawn == 0){
                count++;
            }
            if(head->next != NULL){
                head = head->next;
            } else {
                stop = 1;
            }
        } while (stop != 1);
    }
    return count;
}

void respawnThings(thingToRespawn* head, Map* currentMap){
    int stop = 0;
    if(head != NULL){
        do {
            if(head->turnsUntilRespawn == 0){
                if(head->map == currentMap->level && currentMap->map[head->x][head->y] != 1){
                    putElementHere(currentMap, head->x, head->y, head->type);
                }else{
                    head->turnsUntilRespawn = 1;
                }
            }
            if(head->next != NULL){
                head = head->next;
            } else {
                stop = 1;
            }
        } while (stop != 1);
    }
}

int fight(Player* player, Monster* monster){
    int ongoing = 1;
    int weaponIndex;
    int countWeapon = weaponCheck(player);
    if(countWeapon != 0){
        weaponIndex = changeWeapon(player);
    }else{
        weaponIndex = -1;
    }
    while (ongoing == 1) {
        ongoing = playerTurn(player, monster, weaponIndex);
        if(player->inventory[weaponIndex]->durability == 0 && weaponIndex != -1){
            removeFromPlayerInventory(player, weaponIndex);
            int countWeapon = weaponCheck(player);
            if(countWeapon != 0){
                weaponIndex = changeWeapon(player);
            }else{
                weaponIndex = -1;
            }
        }
        if(ongoing == 1){
            ongoing = monsterTurn(monster, player);
        }
    }
    return processOutcome(ongoing);
}

int processOutcome(int outcome){
    switch (outcome) {
        case 0:
            printf("You were slain in battle! Game over ...\n");
            return 0;
        case 2:
            printf("Ran away successfully, that was close!\n");
        case 3:
            return 3;
        default:
            printf("Uh oh, something unexpected happened during the fight, this is an error, please report this to us!\n");
            return -1;
    }
}

int monsterTurn(Monster* monster, Player* player){
    int protectionIndex = 0;
    float protectionValue = 0;
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        if(player->inventory[i]->protection != -1){
            if(protectionValue < player->inventory[i]->protection){
                protectionIndex = i;
                protectionValue = player->inventory[i]->protection;
            }
        }
    }
    printf("The %s hits you for %d damage!\n", monster->name, (int)(monster->attack - protectionValue));
    player->HP -= (int)(monster->attack - protectionValue);
    if(player->HP <= 0){
        return 0;
    }else{
        return 1;
    }
}

int playerTurn(Player* player, Monster* monster, int weaponIndex){
    int outcome = 1; //Between 0 and 3 (0 = player is dead, 1 = the fight continues, 2 = the player fled, 3 = the monster is dead)
    int choice = 0;
    int error = 0;
    do {
        weaponIndex != -1? printf("Weapon: Damage: %d, Durability: %d\n", player->inventory[weaponIndex]->damage, player->inventory[weaponIndex]->durability) : printf("You have no weapons, run!");
        printf("Your hitpoints: %d, Fighting against: %s who has %d HP\n Select an action:\n - Type 1 to attack\n - Type 2 to use a potion\n - Type 3 to attempt to run away\n", player->HP, monster->name, monster->HP);
        char input[100];
        if ( !fgets( input, sizeof input, stdin ) )
        {
            choice = -1;
        }
        else
        {
            sscanf(input, "%d", &choice);
        }
        error = 0;
        switch (choice) {
            case 1:
                if(weaponIndex == -1){
                    printf("You cannot fight with your fists alone!\n");
                    error = 1;
                } else {
                    outcome = attack(player, monster, weaponIndex);
                    break;
                }
            case 2:
                error = findheal(player);
                break;
            case 3:
                (rand() % (3 + 1 - 1) + 1) == 1? outcome = 2 : printf("Failed to run away!\n");
                break;
            default:
                printf("Select another option!\n");
                error = 1;
        }
    } while (error == 1);
    return outcome;
}

int weaponCheck(Player* player){
    int count = 0;
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        if(player->inventory[i]->damage != -1){
            count++;
        }
    }
    return count;
}

int changeWeapon(Player* player){
    int result = -1;
    int error = 0;
    do {
        error = 0;
        printf("Please choose one of your weapons to equip by typing the corresponding number:\n");
        for (int i = 0; i < player->inventoryNextSpace; ++i) {
            if(player->inventory[i]->damage != -1){
                printf(" - Damage: %d, Durability: %d . Type %d\n", player->inventory[i]->damage, player->inventory[i]->durability, i);
            }
        }
        char input[100];
        if ( !fgets( input, sizeof input, stdin ) )
        {
            error = 1;
        }
        else
        {
            sscanf(input, "%d", &result);
        }
        if(result < 0 || result >= player->inventoryNextSpace){
            printf("Here1");
            error = 1;
        } else if(player->inventory[result]->damage == -1){
            printf("Here2");
            error = 1;
        }
    } while (error == 1);
    return result;
}

int attack(Player* player, Monster* monster, int index){
    printf("You hit the monster for %d points of damage!\n", player->inventory[index]->damage);
    monster->HP -= player->inventory[index]->damage;
    player->inventory[index]->durability -= 1;
    if(monster->HP <= 0){
        printf("The %s has been defeated! Gained %d exp!\n", monster->name, monster->expDrop);
        player->exp += monster->expDrop;
        if(player->exp >= player->expToNextLvl){
            levelUp(player);
        }
        return 3;
    } else {
        return 1;
    }
}

int findheal(Player* player) {
    int nbPotion1 = 0;
    int nbPotion2 = 0;
    int nbPotion3 = 0;
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        if (player->inventory[i]->heal == 30) {
            nbPotion1++;
        } else if (player->inventory[i]->heal == 80) {
            nbPotion2++;
        } else if (player->inventory[i]->heal == 200) {
            nbPotion3++;
        }
    }
    return selectheal(player, nbPotion1, nbPotion2, nbPotion3);
}

int selectheal(Player* player, int nbPotion1, int nbPotion2, int nbPotion3){
    int choice = 0;
    int error = 0;
    do {
        if (nbPotion1 != 0 || nbPotion2 != 0 || nbPotion3 != 0) {
            printf("Select a potion to use: \n");
            if (nbPotion1 > 0) {
                printf("Potion I - heals 30 HP (You have %d) - Type 1\n", nbPotion1);
            }
            if (nbPotion2 > 0) {
                printf("Potion II - heals 80 HP (You have %d) - Type 2\n", nbPotion2);
            }
            if (nbPotion3 > 0) {
                printf("Potion III - heals 200 HP (You have %d) - Type 3\n", nbPotion3);
            }
            char input[100];
            if (!fgets(input, sizeof input, stdin)) {
                error = 1;
            } else {
                sscanf(input, "%d", &choice);
            }
        } else {
            printf("You have no potions!\n");
            return 1;
        }
    } while (error);
    heal(player, choice);
    printf("Healed successfully!\n");
    return 0;
}

void heal(Player* player, int choice){
    switch (choice) {
        case 1:
            if (player->HP + 30 >= player->maxHP) {
                player->HP = player->maxHP;
            } else {
                player->HP += 30;
            }
            deletePotion(player, 30);
            break;
        case 2:
            if (player->HP + 80 >= player->maxHP) {
                player->HP = player->maxHP;
            } else {
                player->HP += 80;
            }
            deletePotion(player, 80);
            break;
        case 3:
            if (player->HP + 200 >= player->maxHP) {
                player->HP = player->maxHP;
            } else {
                player->HP += 200;
            }
            deletePotion(player, 200);
            break;
        default:
            printf("An error occured during the healing process...\n");
    }
}

void deletePotion(Player* player, int heal){
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        if (player->inventory[i]->heal == heal) {
            removeFromPlayerInventory(player, i);
        }
    }
}


void levelUp(Player* player){
    player->level += 1;
    if(player->level > 1 && player->level < 6){
        player->maxHP += (player->level - 1) * 10;
        player->HP = player->maxHP;
    } else if(player->level > 5 && player->level < 9){
        player->maxHP += 50;
        player->HP = player->maxHP;
    } else {
        player->maxHP += 75;
        player->HP = player->maxHP;
    }
    player->expToNextLvl += 10;
    player->exp = 0;

    printf("Level up! Reached level %d your max HP is now %d, you have been fully healed.\n", player->level, player->maxHP);
}

//To be called at the start of the game. Creates a Player with all the default values it needs.
Player* initPlayer(){
    Player* player = malloc(sizeof(Player));
    player->level = 1;
    player->exp = 0;
    player->expToNextLvl = 100;
    player->HP = 100;
    player->maxHP = 100;
    player->inventoryNextSpace = 0;
    //int position[2] = {0, 0};
    player->position[0] = 0;
    player->position[1] = 0;
    addToPlayerInventory(player, newItem(1, 1, 10, -1, -1, -1));
    addToPlayerInventory(player, newItem(2, -1, 10, -1, -1, -1));
    addToPlayerInventory(player, newItem(3, -1, 10, -1, -1, -1));
    addToPlayerInventory(player, newItem(7, -1, -1, 5, -1, -1));
    for (int i = 4; i < 10; ++i) {
        player->inventory[i] = NULL;
    }
    return player;
}

Map* initMap(int width, int height,int level)
{
    //creation of the map
    Map* aMap = malloc(sizeof(Map));

    aMap->width = width;
    aMap->height = height;
    aMap->level = level;

    int** map = (int**) malloc(sizeof(int*)*height);

    for(int i=0 ; i<height ; i++){
        *(map+i)= (int*)malloc(sizeof(int)*width);
        for(int j=0 ; j<width ; j++){
            map[i][j]=0;
        }
    }

    aMap->map = map;
    return (aMap);
}

void displayMap(Map* pMap)
{
    printf("\n");
    for(int i=0 ;i<pMap->height ; i++){
        for(int j=0 ; j<pMap->width ; j++){
            printf(" %d ",pMap->map[i][j]);
        }
        printf("\n");
    }
}
//Creates elements of the map
mapElement* newMapElement(int id)
{
    mapElement* element = malloc(sizeof(mapElement));

    element->id = id;

    return element;
}


//updating the map
int** updateMap(int** map,mapElement* element,int x, int y)
{
    map[x][y]=element->id;
    return(map);
}

//building a map
/*Map**/void buildMap(Map* pMap)
{
    int x;
    int y;

    //monsters positions
    for(int i=0;i<10;i++)
    {
        x = rand()%(pMap->width-1)+1;
        y = rand()%(pMap->height-1)+1;

        if(pMap->map[x][y]==0)
        {
            pMap->map[x][y]=12;
        }
    }

    //rocks positions
    for(int i=0;i<3;i++)
    {
        x = rand()%(pMap->width-1)+1;
        y = rand()%(pMap->height-1)+1;

        if(pMap->map[x][y]==0)
        {
            if(pMap->level==1)
            {
                pMap->map[x][y]=4;
            }
            else if(pMap->level==2)
            {
                pMap->map[x][y]=7;
            }
            else
            {
                pMap->map[x][y]=10;
            }
        }
    }

    //plants positions
    for(int i=0;i<3;i++)
    {
        x = rand()%(pMap->width-1)+1;
        y = rand()%(pMap->height-1)+1;

        if(pMap->map[x][y]==0)
        {
            if(pMap->level==1)
            {
                //updateMap(map, NULL, newMapElement(3),x,y);
                pMap->map[x][y]=3;
            }
            else if(pMap->level==2)
            {
                //updateMap(map, NULL, newMapElement(6),x,y);
                pMap->map[x][y]=6;
            }
            else
            {
                //updateMap(map, NULL, newMapElement(9),x,y);
                pMap->map[x][y]=9;
            }
        }
    }

    //wood positions
    for(int i=0;i<3;i++)
    {
        x = rand()%(pMap->width-1)+1;
        y = rand()%(pMap->height-1)+1;

        if(pMap->map[x][y]==0)
        {
            if(pMap->level==1)
            {
                //updateMap(map, NULL, newMapElement(5),x,y);
                pMap->map[x][y]=5;
            }
            else if(pMap->level==2)
            {
                //updateMap(map, NULL, newMapElement(8),x,y);
                pMap->map[x][y]=8;
            }
            else
            {
                //updateMap(map, NULL, newMapElement(11),x,y);
                pMap->map[x][y]=11;
            }
        }
    }
}

void checkMapElement(Map* pMap,Map* pMap1, Map* pMap2, Map* pMap3, Player* player, int x, int y, thingToRespawn* respawnList, Monster* allMonsters){
    int element = pMap->map[x][y]; //element stored in the player's future position
    int resultFight;

    printf("element %d",element);

    switch(element){
        case -3 : passPortal(player, pMap3);
            break;
        case -2 : passPortal(player, pMap2);
            break;
        case -1 : passPortal(player, pMap1);
            break;
        case 0 : //change player's position
            addToRespawnList(respawnList,pMap->map[x][y],x,y,pMap->level,10);
            player->position[0] = x;
            player->position[1] = y;
            printf("playyer x=%d y=%d",player->position[0],player->position[1]);
            putElementHere(pMap,x,y,1);
            break;
        case 2 : //interact with PNJ
            //instructions
            break;
        case 3 :
        case 4 :
        case 5 :
        case 6 :
        case 7 :
        case 8 :
        case 9 :
        case 10 :
        case 11 :
            //getResourse(player,element,pMap->map);
            getResourse(player, element,pMap,respawnList,x,y);
            break;
        case 99 :
            //engage fight with boss
            resultFight = fight(player, &allMonsters[9]);
            switch (resultFight) {
                case 3 : //monster has been slain
                //end of game
                case -1 :
                    printf("An error has occur. Sorry for the inconvenience");
                default:
                    break;
            }
            break;
        default:
            //engage fight with monster
            for(int i=0;i<10;i++){
                if(allMonsters[i].id==element){
                    resultFight = fight(player,&allMonsters[i]);
                    switch (resultFight) {
                        case 3 : //monster has been slain
                            addToRespawnList(respawnList,element,x,y,pMap->level,15);
                            player->position[0] = x;
                            player->position[1] = y;
                            putElementHere(pMap,x,y,1);
                        case -1 :
                            printf("An error has occur. Sorry for the inconvenience");
                        default:
                            break;
                    }
                }
            }
            break;
    }
}

void putPortalOnMap(Map* pmap1,Map* pmap2,Map* pmap3)
{
    Map* list[3] = {pmap1,pmap2,pmap3};

    for(int i=0 ; i<3 ; i++)
    {
        switch (list[i]->level)
        {
            case 1 : //putElementHere(list[i]->map, getRandomNum(list[i]->width), getRandomNum(list[i]->height),-2);
                list[i]->map[getRandomNum(list[i]->width)][getRandomNum(list[i]->height)]=-2;
                break;
            case 2 : //putElementHere(list[i]->map, getRandomNum(list[i]->width), getRandomNum(list[i]->height),-2);
                //putElementHere(list[i]->map, getRandomNum(list[i]->width), getRandomNum(list[i]->height),-3);
                list[i]->map[getRandomNum(list[i]->width)][getRandomNum(list[i]->height)]=-2;
                list[i]->map[getRandomNum(list[i]->width)][getRandomNum(list[i]->height)]=-3;
                break;
            case 3 : //putElementHere(list[i]->map, getRandomNum(list[i]->width), getRandomNum(list[i]->height),-3);
                list[i]->map[getRandomNum(list[i]->width)][getRandomNum(list[i]->height)]=-3;
                break;
        }
    }
}

int getRandomNum(int limit)
{
    return (rand()%limit+1);
}

void putElementHere(Map* pMap,int x, int y,int elementID)
{
    pMap->map[x][y] = elementID;
}

int movePlayer(Player* player,  Map* pMap, Map* pMap1, Map* pMap2, Map* pMap3,int movement,thingToRespawn* respawnList, Monster* allMonster)
{
    printf("%d",movement);
    switch (movement) {
        case 8 :
            if((player->position[0]>0) && (player->position[0]<=pMap->width) &&(player->position[1]>0) && (player->position[1]<=pMap->height)){
                checkMapElement(pMap,pMap1, pMap2, pMap3, player,player->position[0],player->position[1]+1,respawnList,allMonster);
                return(1);
            }else{
                return(-1);
            }
        case 4 :
            if((player->position[0]>0) && (player->position[0]<=pMap->width) &&(player->position[1]>0) && (player->position[1]<=pMap->height)){
                checkMapElement(pMap,pMap1, pMap2, pMap3,player,player->position[0]-1,player->position[1],respawnList,allMonster);
                return(1);
            }else{
                return(-1);
            }
        case 5 :
            printf("here case");
            if((player->position[0]>0) && (player->position[0]<=pMap->width) &&(player->position[1]>0) && (player->position[1]<=pMap->height)){

                checkMapElement(pMap,pMap1, pMap2, pMap3,player,player->position[0],player->position[1]-1,respawnList,allMonster);
                return(1);
            }else{
                return(-1);
            }
        case 6 :
            if((player->position[0]>0) && (player->position[0]<=pMap->width) &&(player->position[1]>0) && (player->position[1]<=pMap->height)){
                checkMapElement(pMap,pMap1, pMap2, pMap3,player,player->position[0]+1,player->position[1],respawnList, allMonster);
                return(1);
            }else{
                return(-1);
            }
        default:
            return(0);
    }
}

void passPortal(Player* player, Map* destination){
    switch(destination->level){
        case 1 :
            if(player->level >= 3){
                //go
                //player->position[0] = findPortal(player,-2,destination)[0];
                //player->position[1] = findPortal(player-2,destination)[1];
                findPortal(player,-2,destination);
            }
            else {
                printf("you are not allowed to acces this area");
            }
        case 2 :
            if((player->level >3) && (player->level <=7)){
                //go
                //player->position[0] = findPortal(-2,destination)[0];
                //player->position[1] = findPortal(-2,destination)[1];
                findPortal(player,-2,destination);
            }
            else {
                printf("you are not allowed to acces this area");
            }//if player's level >3 and <=7 GO else NO GO
        case 3 :
            if((player->level >7) && (player->level <=10)){
                //go
                //player->position[0] = findPortal(-3,destination)[0];
                //player->position[1] = findPortal(-3,destination)[1];
                findPortal(player,-3,destination);
            }
            else {
                printf("you are not allowed to acces this area");
            }//if player's level >7 and <=10 GO else NO GO
    }
}

void findPortal(Player* player, int id,Map* pMap){
    for(int i=0 ;i<pMap->height ; i++){
        for(int j=0 ; j<pMap->width ; j++){
            if(pMap->map[i][j]==id)
            {
                //portalPosition[0]=i;
                //portalPosition[1]=j;
                player->position[0]=i;
                player->position[1]=j;
            }
        }
    }
}


int checkInventory(Player* player){
    if (player->inventoryNextSpace >= 10) {
        return 1;
    }
    return 0;
}

//Add the resource to the player inventory and change the position of the player if it's possible
int getResourse(Player* player, int resource, Map* pMap, thingToRespawn* head, int x, int y) {
    //  Check if the inventory is full

    int res;
    res = useTool(player, resource, findTool(player, resource));
    if (res != -1) { // Look if the player have the tool or not
        dropResources(resource, player, pMap, head, x, y);
        return res;
    } else {
        printf("\nYou don't have the right tool for that.\n");
        return res;

    }
}


// Check if the resource exist in the player inventory to stack it
int isResourceExistInInventory(Player* player, int resource, int nbResource){
    for(int i = 0 ; i < player->inventoryNextSpace ; i++){
        if(player->inventory[i]->id == resource){
            if(player->inventory[i]->quantity < 20){
                while(player->inventory[i]->quantity < 20){
                    player->inventory[i]->quantity++;
                    nbResource--;

                }
            }
        }
    }
    return nbResource;
}

int dropPlant(Map* pMap, int resource){
    if( resource == 3 || resource == 6 || resource == 9){
        if(pMap->level == 1){
            return 7;
        }
        if(pMap->level == 2){
            return 18;
        }
        if(pMap->level == 3){
            return 29;
        }
    }
    return -1;
}

int dropRock(Map* pMap, int resource, int dropres){
    if( resource == 4 || resource == 7 || resource == 10){
        if(pMap->level == 1){
            return 6;
        }
        if(pMap->level == 2){
            return 17;
        }
        if(pMap->level == 3){
            return 28;
        }
    }
    return dropres;
}

int dropWood(Map* pMap, int resource, int dropres){
    if( resource == 5 || resource == 8 || resource == 11){
        if(pMap->level == 1){
            return 5;
        }
        if(pMap->level == 2){
            return 16;
        }
        if(pMap->level == 3){
            return 27;
        }
    }
    return dropres;
}


// Add random resource to the player inventory
void dropResources(int resource, Player* player, Map* pMap, thingToRespawn* head, int x, int y){
    int checkinventory = checkInventory(player);
    int randResources = rand()% 4 + 1;
    int dropresource = dropPlant(pMap, resource);
    dropresource = dropRock(pMap, resource, dropresource);
    dropresource = dropWood(pMap, resource, dropresource);
    int nbResource;
    nbResource = isResourceExistInInventory(player, dropresource, randResources); // check if the player already have the resource, if yes stack it
    if(nbResource != 0){
        if(checkinventory == 0){
            Item* item;
            item = newItem(dropresource, -1, -1, nbResource, -1, -1);// Create a new item with the ressource
            addToPlayerInventory(player, item); // add it to the player inventory
            thingToRespawn* lasThingToRespawn = NULL;
            lasThingToRespawn = addToRespawnList(head, resource, x, y, pMap->level,10);
        } else {
            printf("\nYour inventory is full \n");
        }
    }
}

void addResourseToInventory(int resource, Player* player, int nbResource){
    int res;
    res = isResourceExistInInventory(player, resource, nbResource); // check if the player already have the resource, if yes stack it
    if(res != 0){    // If the number of resource is equal to 0 we don't need to add more resource
        Item* item;
        item = newItem(resource, -1, -1, res, -1, -1);// Create a new item with the ressource
        addToPlayerInventory(player, item); // add it to the player inventory
    }

}
//  when the player use a tool the function decrease the durability of the tool
int useTool(Player* player, int resource, int toolPosition){
    if(toolPosition != -1) {
        switch (resource) {
            case 3 :
            case 4 :
            case 5 :
                player->inventory[toolPosition]->durability -= 1;
                if (player->inventory[toolPosition]->durability <= 0) {
                    player->inventory[toolPosition]->id = -1; // remove the tool from the inventory if the durability is equal to 0
                }
                return 1;
            case 6 :
            case 7 :
            case 8 :
                player->inventory[toolPosition]->durability -= 2;
                if (player->inventory[toolPosition]->durability <= 0) {
                    player->inventory[toolPosition]->id = -1;
                }
                return 1;
            case 9 :
            case 10 :
            case 11 :
                player->inventory[toolPosition]->durability -= 4;
                if (player->inventory[toolPosition]->durability <= 0) {
                    player->inventory[toolPosition]->id = -1;
                }
                return 1;
            default:
                return -1;
        }
    }
    return -1;
}

int findPlantToolZone3(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 24){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 24 && player->inventory[j]->id == 24){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findPlantToolZone2(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 13 || player->inventory[i]->id == 24){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 24 || player->inventory[i]->id == 13 && player->inventory[j]->id == 24 || player->inventory[j]->id == 13){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findPlantToolZone1(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 13 || player->inventory[i]->id == 24 || player->inventory[i]->id == 3){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 24 || player->inventory[i]->id == 13 || player->inventory[i]->id == 3 && player->inventory[j]->id == 24 || player->inventory[j]->id == 13 || player->inventory[j]->id == 3){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findRockToolZone3(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 23){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 23 && player->inventory[j]->id == 23){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findRockToolZone2(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 23 || player->inventory[i]->id == 12){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 23 || player->inventory[i]->id == 12 && player->inventory[j]->id == 23 || player->inventory[j]->id == 12){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findRockToolZone1(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 23 || player->inventory[i]->id == 12 || player->inventory[i]->id == 2){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 23 || player->inventory[i]->id == 12 || player->inventory[i]->id == 2 && player->inventory[j]->id == 23 || player->inventory[j]->id == 12 || player->inventory[j]->id == 2){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findWoodToolZone3(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 25){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 25 && player->inventory[j]->id == 25){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findWoodToolZone2(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 25 || player->inventory[i]->id == 14){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 25 || player->inventory[i]->id == 14 && player->inventory[j]->id == 25 || player->inventory[j]->id == 14){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}

int findWoodToolZone1(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i]->id == 25 || player->inventory[i]->id == 14 || player->inventory[i]->id == 4){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i]->id == 25 || player->inventory[i]->id == 14 || player->inventory[i]->id == 4 && player->inventory[j]->id == 25 || player->inventory[j]->id == 14 || player->inventory[j]->id == 4){
                    if(player->inventory[i]->durability > player->inventory[j]->durability){
                        toolPosition = j;
                    }
                }
            }
        }
    }
    return toolPosition;
}




int findPlantTool(Player* player, int resource){
    int toolPosition = -1;
    switch (resource) {
        case 3:
            toolPosition = findPlantToolZone1(player);
            break;
        case 6:
            toolPosition = findPlantToolZone2(player);
            break;
        case 9:
            toolPosition = findPlantToolZone3(player);
        default:
            break;
    }

    return toolPosition;
}

int findRockTool(Player* player, int resource){
    int toolPosition = -1;
    switch (resource) {
        case 4:
            toolPosition = findRockToolZone1(player);
            break;
        case 7:
            toolPosition = findRockToolZone2(player);
            break;
        case 10:
            toolPosition = findRockToolZone3(player);
        default:
            break;
    }

    return toolPosition;
}

int findWoodTool(Player* player, int resource){
    int toolPosition = -1;
    switch (resource) {
        case 5:
            toolPosition = findWoodToolZone1(player);
            break;
        case 8:
            toolPosition = findWoodToolZone2(player);
            break;
        case 11:
            toolPosition = findWoodToolZone3(player);
        default:
            break;
    }

    return toolPosition;
}


// Return the position of the used tool if it exist else return -1
int findTool(Player* player, int resource){
    int toolPosition = -1;
    // Check each item to find a tool in the player inventory
    switch (resource) {
        case 3 :
        case 6 :
        case 9 :
            toolPosition = findPlantTool(player, resource);
            break;
        case 4:
        case 7:
        case 10:
            toolPosition = findRockTool(player, resource);
            break;
        case 5 :
        case 8 :
        case 11 :
            toolPosition = findWoodTool(player, resource);
        default: break;
    }

    return toolPosition;
}

// Display the current PNJ inventory
void showPNJInventory(PNJ* pnj, Player* player, Item* allDurabilityItems){
    for(int i = 0 ; i < pnj->inventoryNextSpace ; i++){
        printf("(%d) Item : %d, Damage: %d, Durability: %d, Quantity: %d  \n", i, pnj->inventory[i]->id, pnj->inventory[i]->damage, pnj->inventory[i]->durability, pnj->inventory[i]->quantity);
    }
    interactWithPNJ(pnj, player, allDurabilityItems);
}

// Store an item in the pnj inventory
void storeInPNJInventory(PNJ* pnj, Player* player, Item* allDurabilityItems){
    int choosenItem;
    char input[2];
    fflush(stdin);
    if(player->inventoryNextSpace != 0) {
        printf("\nchoose an item to store :\n");
        for (int i = 0; i < player->inventoryNextSpace; i++) {
            printf(" (%d) Item : %d, Damage: %d, Durability: %d, Quantity: %d  \n", i, player->inventory[i]->id,
                   player->inventory[i]->damage, player->inventory[i]->durability, player->inventory[i]->quantity);
        }

        if (!fgets(input, sizeof input, stdin)) {
            printf("Please enter a correct value");
            storeInPNJInventory(pnj, player, allDurabilityItems);
        } else {
            sscanf(input, "%d", &choosenItem);
        }
        if (choosenItem >= 0 && choosenItem < player->inventoryNextSpace) {
            pnj->inventory[pnj->inventoryNextSpace] = player->inventory[choosenItem];
            addToPNJInventory(pnj, newItem(player->inventory[choosenItem]->id, player->inventory[choosenItem]->damage,
                                           player->inventory[choosenItem]->durability,
                                           player->inventory[choosenItem]->quantity,
                                           player->inventory[choosenItem]->protection,
                                           pnj->inventory[choosenItem]->heal));
            removeFromPlayerInventory(player, choosenItem);
            printf("\nTransfer success !\n");
        }
    } else {
        printf("\nYou don't have anything to store..\n");
    }
}



// Take a chosen item from the PNJ inventory
void takeInPNJInventory(PNJ* pnj, Player* player, Item* allDurabilityItems) {
    int choosenItem;
    int checkinventory;
    char input[2];
    fflush(stdin);
    for (int i = 0; i < pnj->inventoryNextSpace; i++) {
        printf("(%d) Item : %d, Damage: %d, Durability: %d, Quantity: %d  \n", i, pnj->inventory[i]->id,
               pnj->inventory[i]->damage, pnj->inventory[i]->durability, pnj->inventory[i]->quantity);
    }
    if ( !fgets( input, sizeof input, stdin ) )
    {
        printf("Please enter a correct value");
    }
    else
    {
        sscanf(input, "%d", &choosenItem);
    }
    if (choosenItem >= 0 && choosenItem < pnj->inventoryNextSpace) {
        if ((pnj->inventory[choosenItem]->id >= 5 && pnj->inventory[choosenItem]->id <= 7) || (pnj->inventory[choosenItem]->id >= 16 && pnj->inventory[choosenItem]->id <= 18) || (pnj->inventory[choosenItem]->id >= 27 && pnj->inventory[choosenItem]->id <= 30)) {
            addResourseToInventory(pnj->inventory[choosenItem]->id, player,
                                   pnj->inventory[choosenItem]->quantity); // Call the function to add a resource to the player inventory
        } else {
            checkinventory = checkInventory(player);
            if(checkinventory == 0){
                addToPlayerInventory(player, newItem(pnj->inventory[choosenItem]->id, pnj->inventory[choosenItem]->damage,
                                                     pnj->inventory[choosenItem]->durability,
                                                     pnj->inventory[choosenItem]->quantity,
                                                     pnj->inventory[choosenItem]->protection, pnj->inventory[choosenItem]->heal));
            } else {
                printf("your inventory is full");
            }
        }
        removeFromPNJInventory(pnj, choosenItem);
    }
    interactWithPNJ(pnj, player, allDurabilityItems);
}

void repairItem(Player* player, Item* allDurabilityItems){
    for(int i = 0 ; i < player->inventoryNextSpace ; i++){
        if(player->inventory[i]->durability != 1){
            for(int j = 0 ; j < 19 ; j++){
                if(allDurabilityItems[j].id == player->inventory[i]->id){
                    player->inventory[i]->durability = allDurabilityItems[j].durability;
                }
            }
        }
    }
    printf("\n Repair complete !\n");
}

//  Use to interact with PNJ
void usePNJ(PNJ* pnj, Player* player, int choice, Item* allDurabilityItems){
    switch (choice) {
        case 0:
            showPNJInventory(pnj, player, allDurabilityItems);
            interactWithPNJ(pnj, player, allDurabilityItems); // Display the PNJ menu again
            break;
        case 1:
            takeInPNJInventory(pnj, player, allDurabilityItems);
            interactWithPNJ(pnj, player, allDurabilityItems);
            break;
        case 2:
            storeInPNJInventory(pnj, player, allDurabilityItems);
            interactWithPNJ(pnj, player, allDurabilityItems); // Display the PNJ menu again
            break;
        case 3:
            repairItem(player, allDurabilityItems);
            interactWithPNJ(pnj, player, allDurabilityItems); // Display the PNJ menu again
            break;
        default:
            break;
    }

}

//  Display the menu to interact with the PNJ
void interactWithPNJ(PNJ* pnj, Player* player, Item* allDurabilityItems){
    printf("\n Display PNJ inventory (0)\n Take an item (1) \n Store an item (2) \n Repair all items (3) \n Cancel (4)\n");
    int choice;
    char input[2];
    fflush(stdin);
    if ( !fgets( input, sizeof input, stdin ) )
    {
        printf("Please enter a correct value");
        storeInPNJInventory(pnj, player, allDurabilityItems);
    }
    else
    {
        sscanf(input, "%d", &choice);
        usePNJ(pnj, player, choice, allDurabilityItems);
    }
}
int main() {
    Item allDurabilityItems[] = {};
            //Weapons

    Item allItems[] = {
            *newItem(1,1,10,-1, -1.0, -1),
            *newItem(8,2,10,-1, -1.0, -1),
            *newItem(9,3,8,-1, -1.0, -1),
            *newItem(10,4,5,-1, -1.0, -1),
            *newItem(19,5,10,-1, -1.0, -1),
            *newItem(20,7,8,-1, -1.0, -1),
            *newItem(21,10,5,-1, -1.0, -1),
            *newItem(30,10,10,-1, -1.0, -1),
            *newItem(31,15,8,-1, -1.0, -1),
            *newItem(32,20,5,-1, -1.0, -1),
            //Tools
            *newItem(2,-1,10,-1, -1.0, -1),
            *newItem(3,-1,10,-1, -1.0, -1),
            *newItem(4,-1,10,-1, -1.0, -1),
            *newItem(12,-1,10,-1, -1.0, -1),
            *newItem(13,-1,10,-1, -1.0, -1),
            *newItem(14,-1,10,-1, -1.0, -1),
            *newItem(23,-1,10,-1, -1.0, -1),
            *newItem(24,-1,10,-1, -1.0, -1),
            *newItem(25,-1,10,-1, -1.0, -1)
    };

    Player* player = initPlayer();

    thingToRespawn* thingsToRespawn = NULL;

    Monster allMonsters[10];
    //Zone 1
    allMonsters[0] = *newMonster("Slime", 12, 3, 5, 25);
    allMonsters[1] = *newMonster("Goblin", 13, 5, 3, 30);
    allMonsters[2] = *newMonster("Bat", 14, 2, 1, 10);
    //Zone 2
    allMonsters[3] = *newMonster("Orc", 15, 22, 7, 35);
    allMonsters[4] = *newMonster("Salamander", 16, 28, 12, 35);
    allMonsters[5] = *newMonster("Plant Monster", 17, 30, 8, 35);
    //Zone 3
    allMonsters[6] = *newMonster("Dragon", 18, 75, 30, 70);
    allMonsters[7] = *newMonster("Succubus", 19, 60, 33, 69);
    allMonsters[8] = *newMonster("Golem", 20, 100, 15, 65);
    //Boss
    allMonsters[9] = *newMonster("Briatte, Sith Lord, ArchDemon, Destroyer of Worlds, and more...", 99, 250, 55, 200);

    printf("\n");

    //Test to see if removing the second item from the inventory works
    removeFromPlayerInventory(player, 1);
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        printf("Item id: %d, damage: %d, durability: %d\n", player->inventory[i]->id, player->inventory[i]->damage, player->inventory[i]->durability);
    }

    //Test for monster creation
    Monster* monster = newMonster("Dragon", 12, 2, 15, 100);
    //printf("Nom du monstre : %s, id : %d, HP : %d, attack : %d, expDrop : %d\n", monster->name, monster->id, monster->HP, monster->attack, monster->expDrop);

    //addToPlayerInventory(player, newItem(45, -1, -1, -1, -1.0, 30));

    //Combat test
    //printf("Result of the fight: %d", fight(player, monster));
/*
    //Test for PNJ creation
    PNJ* pnj1 = newPNJ();
    addToPNJInventory(pnj1, newItem(1, 1, 10, -1, -1, -1));
    addToPNJInventory(pnj1, newItem(3, -1, -1, 5, -1, -1));
    addToPNJInventory(pnj1, newItem(4, -1, -1, 5, -1, -1));
    addToPNJInventory(pnj1, newItem(7, -1, -1, 17, -1, -1));
    addToPNJInventory(pnj1, newItem(7, -1, -1, 5, -1, -1));
    addToPNJInventory(pnj1, newItem(16, -1, -1, 5, -1, -1));

    interactWithPNJ(pnj1, player, allDurabilityItems);
    Map* map1 = initMap(10,15,1);
    getResourse(player, 3, map1, thingsToRespawn, 5, 5);
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        printf("Item id: %d, damage: %d, durability: %d, Quantity: %d\n", player->inventory[i]->id, player->inventory[i]->damage, player->inventory[i]->durability, player->inventory[i]->quantity);
    }
    //printf("\n---------------------------\n");

    //showPNJInventory(pnj1);

    /*printf("\n---------------------------\n");
    for (int i = 0; i < pnj1->inventoryNextSpace; ++i) {
        printf("PNJ1's inventory : id: %d, damage: %d, durability: %d\n", pnj1->inventory[i].id, pnj1->inventory[i].damage, pnj1->inventory[i].durability);
    }*/

    //interactWithPNJ(pnj1);

    //test map
    /*printf("\n\n");

    //test  = portail 3
    mapElement* firstElement= newMapElement(-3);
    printf("\nid : %d\n",firstElement->id);

    //update map with element on a position
    Map* map1 = initMap(10,15,1);
    Map* map2 = initMap(20,20,2);
    Map* map3 = initMap(25,25,3);



    buildMap(map1);
    printf("map1 faite\n");
    buildMap(map2);
    printf("map2 faite\n");
    buildMap(map3);
    printf("map3 faite\n");

    putPortalOnMap(map1,map2,map3);
    putPlayerOnMap(map1,player);

    printf("Test d'initialisation des maps\n\n");
    printf("Map niveau 1\n");
    displayMap(map1);
    printf("\n");

    printf("Map niveau 2\n");
    displayMap(map2);
    printf("\n");

    printf("Map niveau 3\n");
    displayMap(map3);
    printf("\n");



    //theMap = buildMap(theMap);

    //updateMap(theMap,NULL,firstElement,5,3);
    //printf("Map après update :\n");
    //displayMap(theMap);

    //checkMapElement(map3,player,5,3);

    //printf("\n\n\n");
    //putElementHere(map1,5,5,2000);
    //displayMap(map1);*/
    printf("Welcome to our game! You are represented on the map by a 1, use z, q, s, d to move around, we hope you'll have fun!");
    thingToRespawn* respawnList = NULL;
    int inProgress = 1;
    Map* currentMap;
    Map* map1 = initMap(10,15,1);
    Map* map2 = initMap(20,20,2);
    Map* map3 = initMap(25,25,3);
    buildMap(map1);
    buildMap(map2);
    buildMap(map3);
    currentMap = map1;
    putElementHere(currentMap,player->position[0], player->position[1],1);
    int result;
    displayMap(currentMap);
    do {
        //error = 0;
        fflush(stdin);
        printf("Next move : ");

        char input[100];
        if ( !fgets( input, sizeof input, stdin ) )
        {
            //error = 1;
            printf("error");
        }
        else
        {
            sscanf(input, "%d", &result);
        }
        movePlayer(player,currentMap,map1,map2,map3,result,respawnList,allMonsters);
        inProgress=0;
    } while (inProgress);
        int nbToRespawn = updateTimerToRespawn(thingsToRespawn);
        if(nbToRespawn > 0){
            respawnThings(thingsToRespawn, currentMap);
        }

    //printf("player position x = %d / y = %d",player->position[0],player->position[1]);
    putPortalOnMap(map1,map2,map3);
    //player->position[0] = 0;
    //player->position[1] = 0;
    //putElementHere(map1,5, 5,555);

    displayMap(map1);
}
