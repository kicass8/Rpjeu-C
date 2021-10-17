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
mapElement* newMapElement(int id, char* name)
{
    mapElement* element = malloc(sizeof(mapElement));

    element->id = id;
    element->name = name;

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
    srand(time(NULL));

    //monsters positions
    int monsterPosition[10];
    for(int i=0 ; i<10 ; i++)
    {
        x = rand()%98;
        y = rand()%98;
        updateMap(map, NULL, newMapElement(12,"Monster"),x,y);
    }

    //rocks positions
    int rockPosition[3];
    for(int i=0 ; i<3 ; i++)
    {
        x = rand()%98;
        y = rand()%98;
        if(level==1)
        {
            updateMap(map, NULL, newMapElement(4,"Rock 1"),x,y);
        }
        else if(level==2)
        {
            updateMap(map, NULL, newMapElement(7,"Rock 2"),x,y);
        }
        else
        {
            updateMap(map, NULL, newMapElement(10,"Rock 3"),x,y);
        }
    }

    //plants positions
    int plantPosition[3];
    for(int i=0 ; i<3 ; i++)
    {
        x = rand()%98;
        y = rand()%98;
        if(level==1)
        {
            updateMap(map, NULL, newMapElement(3,"Plant 1"),x,y);
        }
        else if(level==2)
        {
            updateMap(map, NULL, newMapElement(6,"Plant 2"),x,y);
        }
        else
        {
            updateMap(map, NULL, newMapElement(9,"Plant 3"),x,y);
        }
    }

    //wood positions
    int woodPosition[3];
    for(int i=0 ; i<3 ; i++)
    {
        x = rand()%98;
        y = rand()%98;

        if(level==1)
        {
            updateMap(map, NULL, newMapElement(5,"Wood 1"),x,y);
        }
        else if(level==2)
        {
            updateMap(map, NULL, newMapElement(8,"Wood 2"),x,y);
        }
        else
        {
            updateMap(map, NULL, newMapElement(11,"Wood 3"),x,y);
        }
    }

    return(map);
}

//Add the resource to the player inventory and change the position of the player if it's possible
void getResourse(Player* player, int x, int y, int zone, int** map){
    //  Check if the inventory is full
    if(player->inventoryNextSpace >=  255) {
        printf("Inventory full ! The ressource will not be collected");
    }
    int res = -1;
    int randRessources = rand()% 4 + 1;
    Item* item = malloc(sizeof(Item));
    mapElement* Element = malloc(sizeof(Element));
    switch (map[x][y]) {
        case 3 :
            res = useTool(player, zone, findTool(player, map[x][y]));
            //  Verify if the player have the right tool in his inventory
            if(res != -1) {
                item = newItem(map[x][y], -1, -1, randRessources, -1, -1);// Create a new item with the ressource
                addToPlayerInventory(player, item); // add it to the player inventory
                map[x][y] = 0;  // switch the case of the resource on 0
                updateMap(map, player, newMapElement(3, "Plant"), x, y ); // update the map ( dans newElementMap je pensais qu'il fallait mettre 0 )
                buildMap(map, zone); // build the new map
                displayMap(map, 10, 10); // display the new map
            }
            break;
        case 4:
            res = useTool(player, zone, findTool(player, map[x][y]));
            if(res != -1) {
                item = newItem(map[x][y], -1, -1, randRessources, -1, -1);
                addToPlayerInventory(player, item);
                map[x][y] = 0;
                updateMap(map, player, newMapElement(4, "Rock"), x, y );
                buildMap(map, zone);
                displayMap(map, 10, 10);
            }
            break;
        case 5 :
            res = useTool(player, zone, findTool(player, map[x][y]));
            if(res != -1) {
                item = newItem(map[x][y], -1, -1, randRessources, -1, -1);
                addToPlayerInventory(player, item);
                map[x][y] = 0;
                updateMap(map, player, newMapElement(5, "Wood"), x, y );
                buildMap(map, zone);
                displayMap(map, 10, 10);
            }
            break;
        default:
            break;
    }
}


//  when the player use a tool the function decrease the durability of the tool
int useTool(Player* player, int zone, int toolPosition){
    if(toolPosition != -1) {
        switch (zone) {
            case 1 :
                player->inventory[toolPosition].durability *= 0.10;
                if (player->inventory[toolPosition].durability <= 0) {
                    //  player->inventory[toolPosition] = -1; // remove the tool from the inventory if the durability is equal to 0
                }
                return 1;
            case 2:
                player->inventory[toolPosition].durability *= 0.20;
                if (player->inventory[toolPosition].durability <= 0) {
                    //  player->inventory[toolPosition] = -1;
                }
                return 1;
            case 3 :
                player->inventory[toolPosition].durability *= 0.40;
                if (player->inventory[toolPosition].durability <= 0) {
                    //  player->inventory[toolPosition] = -1;
                }
                return 1;
            default:
                break;
        }
    }
    return -1;
}

// Return the position of the used tool if it exist else return -1
int findTool(Player* player, int ressource){
    int toolPosition = -1;
    // Check each item to find a tool in the player inventory
    switch (ressource) {
        case 3 :
            for (int i = 0; i < player->inventoryNextSpace; i++) {
                // Check if the right tool is available
                if (player->inventory[i].id == 3 || player->inventory[i].id == 13 || player->inventory[i].id == 24) {
                    // Check if the tool durability isn't equal to 0
                    if (player->inventory[i].durability != 0) {
                        toolPosition = i;
                    }
                }
            }
        case 4:
            for (int i = 0; i < player->inventoryNextSpace; i++) {
                // Check if the right tool is available
                if (player->inventory[i].id == 2 || player->inventory[i].id == 12 || player->inventory[i].id == 23) {
                    // Check if the tool durability isn't equal to 0
                    if (player->inventory[i].durability != 0) {
                        toolPosition = i;
                    }
                }
            }
        case 5 :
            for (int i = 0; i < player->inventoryNextSpace; i++) {
                // Check if the right tool is available
                if (player->inventory[i].id == 4 || player->inventory[i].id == 14 || player->inventory[i].id == 25) {
                    // Check if the tool durability isn't equal to 0
                    if (player->inventory[i].durability != 0) {
                        toolPosition = i;
                    }
                }
            }
        default: break;
    }

    return toolPosition;
}
/*
int findAxe(Player* player, int ressource){
    int toolPosition = -1;
    if(ressource == 5) {
        for (int i = 0; i < player->inventoryNextSpace; i++) {
            // Check if the right tool is available
            if (player->inventory[i].id == 4 || player->inventory[i].id == 14 || player->inventory[i].id == 25) {
                // Check if the tool durability isn't equal to 0
                if (player->inventory[i].durability != 0) {
                    toolPosition = i;
                }
            }
        }
    }
    return toolPosition;
}

int findPickaxe(Player* player, int ressource){
    int toolPosition = -1;
    if(ressource == 4) {
        for (int i = 0; i < player->inventoryNextSpace; i++) {
            // Check if the right tool is available
            if (player->inventory[i].id == 2 || player->inventory[i].id == 12 || player->inventory[i].id == 23) {
                // Check if the tool durability isn't equal to 0
                if (player->inventory[i].durability != 0) {
                    toolPosition = i;
                }
            }
        }
    }
    return toolPosition;
}

int findBillhook(Player* player, int ressource){
    int toolPosition = -1;
    if(ressource == 3) {
        for (int i = 0; i < player->inventoryNextSpace; i++) {
            // Check if the right tool is available
            if (player->inventory[i].id == 3 || player->inventory[i].id == 13 || player->inventory[i].id == 24) {
                // Check if the tool durability isn't equal to 0
                if (player->inventory[i].durability != 0) {
                    toolPosition = i;
                }
            }
            return toolPosition;
}

*/




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
    mapElement* firstElement= newMapElement(-3, "Portail 3");
    printf("\n%s : id : %d\n",firstElement->name,firstElement->id);

    //update map with element on a position
    int** theMap = initMap(10,10);
    displayMap(theMap,10,10);
    printf("\n\n");
    int** theMap2 = buildMap(theMap,1);
    printf("Map après update :\n");
    displayMap(theMap2,10,10);

    //Test for the linked list of things to respawn (NOT_WORKING)
    thingToRespawn* thingsToRespawn = malloc(sizeof(thingToRespawn) * 100);
    thingsToRespawn = addToRespawnList(thingsToRespawn, 3, 1, 1, 1,10);
    while (thingsToRespawn->next != NULL){
        printf("Thing -> type : %d, coordinates : {%d, %d} , map : %d, in how many turns : %d\n", thingsToRespawn->type, thingsToRespawn->x, thingsToRespawn->y, thingsToRespawn->map, thingsToRespawn->turnsUntilRespawn);
        thingsToRespawn = thingsToRespawn->next;
    }

}
