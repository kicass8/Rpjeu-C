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
    Item* defaultInventory = malloc(sizeof(Item) * 255);
    pnj->inventory = defaultInventory;
    pnj->inventoryNextSpace = 0;
    return pnj;
}

//Adds an already initialised Item to the Player's inventory.
void addToPlayerInventory(Player* player, Item* item){
    player->inventory[player->inventoryNextSpace] = *item;
    player->inventoryNextSpace++;
}

//Adds an already initialised Item to a PNJ's inventory.
void addToPNJInventory(PNJ* pnj, Item* item){
    pnj->inventory[pnj->inventoryNextSpace] = *item;
    pnj->inventoryNextSpace++;
}

//adds a thing to respawn to the respawn linked list
thingToRespawn* addToRespawnList(thingToRespawn* head, int type, int x, int y, int map, int nbturns){
    thingToRespawn* temp = head;
    while (head->next != NULL){
        head = head->next;
    }
    thingToRespawn* newThing = malloc(sizeof(thingToRespawn));
    newThing->type = type;
    newThing->x = x;
    newThing->y = y;
    newThing->map = map;
    newThing->turnsUntilRespawn = nbturns;

    head->next = newThing;
    head = temp;
    return head;
}

//To be called at the start of the game. Creates a Player with all the default values it needs.
Player* initPlayer(){
    Player* player = malloc(sizeof(Player));
    Item* defaultInventory = malloc(sizeof(Item) * 10);
    player->level = 1;
    player->exp = 0;
    player->expToNextLvl = 100;
    player->HP = 100;
    player->maxHP = 100;
    player->inventory = defaultInventory;
    player->inventoryNextSpace = 0;
    int position[2] = {0, 0};
    player->position = position;
    addToPlayerInventory(player, newItem(1, 1, 10, -1, -1, -1));
    addToPlayerInventory(player, newItem(2, -1, 10, -1, -1, -1));
    addToPlayerInventory(player, newItem(3, -1, 10, -1, -1, -1));
    addToPlayerInventory(player, newItem(4, -1, 10, -1, -1, -1));
    return player;
}

int** initMap(int width, int height)
{
    //creation of the map
    int** map = (int**) malloc(sizeof(int*)*height);
    for(int i=0 ; i<height ; i++){
        *(map+i)= (int*)malloc(sizeof(int)*width);
        for(int j=0 ; j<width ; j++){
            map[i][j]=0;
        }
    }
    return(map);
}

void displayMap(int** map,int width, int height)
{
    printf("\n");
    for(int i=0 ;i<height ; i++){
        for(int j=0 ; j<width ; j++){
            printf(" %d ",map[i][j]);
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
int** updateMap(int** map, Player* player, mapElement* element,int x, int y)
{
    if(player==NULL)
    {
        //idElement on the map
        map[x][y]=element->id;
    }
    else if(element==NULL)
    {
        map[x][y]=1;
    }

    return(map);
}

//building a map
int** buildMap(int** map, int level)
{
    int x,y;

    //monsters positions
    for(int i=0 ; i<10 ; i++)
    {
        x = rand()%9+1;
        y = rand()%9+1;

        if(map[x][y]==0)
        {
            map[x][y]=12;
        }
    }

    //rocks positions
    for(int i=0 ; i<3 ; i++)
    {
        x = rand()%9+1;
        y = rand()%9+1;
        if(map[x][y]==0)
        {
            if(level==1)
            {
                map[x][y]=4;
            }
            else if(level==2)
            {
                map[x][y]=7;
            }
            else
            {
                map[x][y]=10;
            }
        }
    }

    //plants positions
    for(int i=0 ; i<3 ; i++)
    {
        x = rand()%9+1;
        y = rand()%9+1;
        if(map[x][y]==0)
        {
            if(level==1)
            {
                //updateMap(map, NULL, newMapElement(3),x,y);
                map[x][y]=3;
            }
            else if(level==2)
            {
                //updateMap(map, NULL, newMapElement(6),x,y);
                map[x][y]=6;
            }
            else
            {
                //updateMap(map, NULL, newMapElement(9),x,y);
                map[x][y]=9;
            }
        }
    }

    //wood positions
    for(int i=0 ; i<3 ; i++)
    {
        x = rand()%9+1;
        y = rand()%9+1;

        if(map[x][y]==0)
        {
            if(level==1)
            {
                //updateMap(map, NULL, newMapElement(5),x,y);
                map[x][y]=5;
            }
            else if(level==2)
            {
                //updateMap(map, NULL, newMapElement(8),x,y);
                map[x][y]=8;
            }
            else
            {
                //updateMap(map, NULL, newMapElement(11),x,y);
                map[x][y]=11;
            }
        }
    }
    return(map);
}

//Add the resource to the player inventory and change the position of the player if it's possible
int getResourse(Player* player,int resource, int** map) {
    //  Check if the inventory is full
    if (player->inventoryNextSpace >= 10) {
        printf("Inventory full ! The ressource will not be collected");
    }
    int res;

    res = useTool(player, resource, findTool(player, resource));
    if (res != -1) { // Look if the player have the tool or not
        addResourseToInventory(resource, player);
        return res;
    } else {
        printf("You don't have the right tool for that.");
        return res;
        
    }
}

// Add random resource to the player inventory
void addResourseToInventory(int resource, Player* player){
    int randResources = rand()% 4 + 1;
    Item* item = malloc(sizeof(Item));
    item = newItem(resource, -1, -1, randResources, -1, -1);// Create a new item with the ressource
    addToPlayerInventory(player, item); // add it to the player inventory

}

//  when the player use a tool the function decrease the durability of the tool
int useTool(Player* player, int resource, int toolPosition){
    if(toolPosition != -1) {
        switch (resource) {
            case 3 :
            case 4 :
            case 5 :
                player->inventory[toolPosition].durability *= 0.10;
                if (player->inventory[toolPosition].durability <= 0) {
                      player->inventory[toolPosition].id = -1; // remove the tool from the inventory if the durability is equal to 0
                }
                return 1;
            case 6 :
            case 7 :
            case 8 :
                player->inventory[toolPosition].durability *= 0.20;
                if (player->inventory[toolPosition].durability <= 0) {
                      player->inventory[toolPosition].id = -1;
                }
                return 1;
            case 9 :
            case 10 :
            case 11 :
                player->inventory[toolPosition].durability *= 0.40;
                if (player->inventory[toolPosition].durability <= 0) {
                      player->inventory[toolPosition].id = -1;
                }
                return 1;
            default:
                return -1;
        }
    }
}

// PAS ENCORE AJOUTER LES POTOTYPE PARCE QUE PAS SUR
// Voir comment faire pour chercher la plus petite durablitité PAS SUR
int findLowerDurabilityTool(Player* player, int resource){

}

int findPlantToolZone3(Player* player){
    int toolPosition = -1;
    for(int i = 0; i < player->inventoryNextSpace; i++){
        if(player->inventory[i].id == 24){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 24 && player->inventory[j].id == 24){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 13 || player->inventory[i].id == 24){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 24 || player->inventory[i].id == 13 && player->inventory[j].id == 24 || player->inventory[j].id == 13){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 13 || player->inventory[i].id == 24 || player->inventory[i].id == 3){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 24 || player->inventory[i].id == 13 || player->inventory[i].id == 3 && player->inventory[j].id == 24 || player->inventory[j].id == 13 || player->inventory[j].id == 3){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 23){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 23 && player->inventory[j].id == 23){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 23 || player->inventory[i].id == 12){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 23 || player->inventory[i].id == 12 && player->inventory[j].id == 23 || player->inventory[j].id == 12){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 23 || player->inventory[i].id == 12 || player->inventory[i].id == 2){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 23 || player->inventory[i].id == 12 || player->inventory[i].id == 2 && player->inventory[j].id == 23 || player->inventory[j].id == 12 || player->inventory[j].id == 2){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 25){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 25 && player->inventory[j].id == 25){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 25 || player->inventory[i].id == 14){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 25 || player->inventory[i].id == 14 && player->inventory[j].id == 25 || player->inventory[j].id == 14){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
        if(player->inventory[i].id == 25 || player->inventory[i].id == 14 || player->inventory[i].id == 4){
            toolPosition = i;
            for(int j = i + 1; j < player->inventoryNextSpace; j++){
                if(player->inventory[i].id == 25 || player->inventory[i].id == 14 || player->inventory[i].id == 4 && player->inventory[j].id == 25 || player->inventory[j].id == 14 || player->inventory[j].id == 4){
                    if(player->inventory[i].durability > player->inventory[j].durability){
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
            findPlantTool(player, resource);
            break;
        case 4:
        case 7:
        case 10:
            findRockTool(player, resource);
            break;
        case 5 :
        case 8 :
        case 11 :
            findWoodTool(player, resource);
        default: break;
    }

    return toolPosition;
}


void checkMapElement(int **map, Player* player, int x, int y){
    int element = map[x][y]; //element stored in the player's future position
    int newPosition[2] = {x,y};

    switch(element){
        case -3 : //third zone's portal
            break;
        case -2 : //second zone's portal
            break;
        case -1 : //first zone's portal
            break;
        case 0 : //change player's position
            player->position = newPosition;
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
            //getResourse(player, x, y,1,map);
            //addToRespawnList();
            break;
        case 99 :
            //engage fight with boss
            break;
        default:
            //engage fight with monster
            break;
    }
}


int main() {
    Player* player = initPlayer();

    //Test to see if initPlayer and the other functions associated work
    printf("Player's position : %d, %d\n", player->position[0], player->position[1]);
    for (int i = 0; i < player->inventoryNextSpace; ++i) {
        printf("Item id: %d, damage: %d, durability: %d\n", player->inventory[i].id, player->inventory[i].damage, player->inventory[i].durability);
    }

    //Test for monster creation
    Monster* monster = newMonster("Dragon", 12, 25, 15, 100);
    printf("Nom du monstre : %s, id : %d, HP : %d, attack : %d, expDrop : %d\n", monster->name, monster->id, monster->HP, monster->attack, monster->expDrop);

    //Test for PNJ creation
    PNJ* pnj1 = newPNJ();
    addToPNJInventory(pnj1, newItem(1, 1, 10, -1, -1, -1));
    for (int i = 0; i < pnj1->inventoryNextSpace; ++i) {
        printf("PNJ1's inventory : id: %d, damage: %d, durability: %d\n", pnj1->inventory[i].id, pnj1->inventory[i].damage, pnj1->inventory[i].durability);
    }

    //test map
    //displayMap(initMap(5,5,1),5,5);
    printf("\n\n");

    //test  = portail 3
    mapElement* firstElement= newMapElement(-3);
    printf("\nid : %d\n",firstElement->id);

    //update map with element on a position
    int** theMap = initMap(10,10);
    displayMap(theMap,10,10);
    printf("\n\n");

    theMap = buildMap(theMap,1);

    //updateMap(theMap,NULL,firstElement,5,3);
    printf("Map après update :\n");
    displayMap(theMap,10,10);

    checkMapElement(theMap,player,5,3);

    //Test for the linked list of things to respawn (NOT_WORKING)
    thingToRespawn* thingsToRespawn = malloc(sizeof(thingToRespawn) * 100);
    thingsToRespawn = addToRespawnList(thingsToRespawn, 3, 1, 1, 1,10);
    while (thingsToRespawn->next != NULL){
        printf("Thing -> type : %d, coordinates : {%d, %d} , map : %d, in how many turns : %d\n", thingsToRespawn->type, thingsToRespawn->x, thingsToRespawn->y, thingsToRespawn->map, thingsToRespawn->turnsUntilRespawn);
        thingsToRespawn = thingsToRespawn->next;
    }

}
