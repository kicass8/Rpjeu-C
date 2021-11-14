//
// Created by kicas on 11/10/2021.
//

#ifndef PROJET_C_RPG_H
#define PROJET_C_RPG_H

//Item structure, all values should be either -1 (used to indicate that this item does not possess such characteristic) or a positive value.
struct Item {
    int id;
    int damage;
    int durability;
    int quantity;
    float protection;
    int heal;
};

typedef struct Item Item;

//Player structure, 'inventoryNextSpace' is used to keep track of the next available index of the inventory.
struct Player {
    int level;
    int exp;
    int expToNextLvl;
    int HP;
    int maxHP;
    Item* inventory;
    int inventoryNextSpace;
    int* position;
};

typedef struct Player Player;

//Monster structure, id must be comprised between 12 and 99
struct Monster {
    char* name;
    int id;
    int HP;
    int attack;
    int expDrop;
};

typedef struct Monster Monster;

//PNJ structure, holds items, 'inventoryNextSpace' is used to keep track of the next available index of the inventory.
struct PNJ {
    Item* inventory;
    int inventoryNextSpace;
};

typedef struct PNJ PNJ;

//Linked list of things to respawn
struct thingToRespawn {
    int type;
    int x;
    int y;
    int map;
    int turnsUntilRespawn;
    struct thingToRespawn* next;
};

typedef struct thingToRespawn thingToRespawn;

struct mapElement {
    int id;
};

typedef struct mapElement mapElement;

struct Map {
    int width;
    int height;
    int level;
    int **map;
};
typedef struct Map Map;

thingToRespawn* addToRespawnList(thingToRespawn* list,int type, int x, int y, int map, int nbturns);
PNJ* newPNJ();
Monster* newMonster(char* name, int id, int HP, int attack, int expDrop);
Item* newItem(int id, int damage, int durability, int quantity, float protection, int heal);
void addToPlayerInventory(Player* player, Item* item);
void addToPNJInventory(PNJ* pnj, Item* item);
Player* initPlayer();
Map* initMap(int width, int height,int level);
void displayMap(Map* pMap);
mapElement* newMapElement(int id);
int** updateMap(int** map, mapElement* element,int x, int y);
/*Map**/void buildMap(Map* pMap);
int findTool(Player* player, int ressource);
int findWoodTool(Player* player, int resource);
int findRockTool(Player* player, int resource);
int findPlantTool(Player* player, int resource);
int useTool(Player* player, int zone, int toolPosition);
int getResourse(Player* player,int resource, int** map);
int findPlantTool(Player* player, int resource);
void addResourseToInventory(int resource, Player* player, int nbResource);
void dropResources(int resource, Player* player);
void interactWithPNJ(PNJ* pnj, Player* player);
void checkMapElement(Map* pMap, Player* player, int x, int y);

#endif //PROJET_C_RPG_H
