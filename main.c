#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int** initMap(int width, int height, int level)
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
    for(int i=0 ; i<(sizeof(map)/sizeof(map[0])) ; i++)
    {
        for(int j=0 ; j<(sizeof(map)/sizeof(map[0])) ; j++)
        {
            if((x==i)&&(y==j))
            {
                if(player==NULL)
                {
                    map[i][j] = element->id;
                }
                else if (element==NULL)
                {
                    map[i][j] = 1;
                }
            }
        }
    }

    return(map);
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
    mapElement* firstElement= newMapElement(-3, "Portail 3");
    printf("\n%s : id : %d\n",firstElement->name,firstElement->id);

    //update map with element on a position
    int** theMap = initMap(10,10,2);
    displayMap(theMap,10,10);
    printf("\n\n");
    int** theMap2 = updateMap(theMap,NULL,firstElement,5,5);
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
