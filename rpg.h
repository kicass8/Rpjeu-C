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
    Item* inventory[10];
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
    Item* inventory[255];
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
void levelUp(Player* player);
int fight(Player* player, Monster* monster);
int monsterTurn(Monster* monster, Player* player);
int playerTurn(Player* player, Monster* monster, int weaponIndex);
int weaponCheck(Player* player);
int changeWeapon(Player* player);
int attack(Player* player, Monster* monster, int index);
int findheal(Player* player);
int selectheal(Player* player, int nbPotion1, int nbPotion2, int nbPotion3);
void heal(Player* player, int choice);
void deletePotion(Player* player, int heal);
int processOutcome(int outcome);
int updateTimerToRespawn(thingToRespawn* head);
void respawnThings(thingToRespawn* head, Map* currentMap);
PNJ* newPNJ();
Monster* newMonster(char* name, int id, int HP, int attack, int expDrop);
Item* newItem(int id, int damage, int durability, int quantity, float protection, int heal);
void addToPlayerInventory(Player* player, Item* item);
void addToPNJInventory(PNJ* pnj, Item* item);
void removeFromPlayerInventory(Player* player, int index);
void removeFromPNJInventory(PNJ * pnj, int index);
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
int findPlantTool(Player* player, int resource);
int getResourse(Player* player, int resource, Map* pMap, thingToRespawn* head, int x, int y);
int checkInventory(Player* player);
void addResourseToInventory(int resource, Player* player, int nbResource);
void takeInPNJInventory(PNJ* pnj, Player* player, Item* allDurabilityItems);
void storeInPNJInventory(PNJ* pnj, Player* player, Item* allDurabilityItems);
void usePNJ(PNJ* pnj, Player* player, int choice, Item* allDurabilityItems);
int isResourceExistInInventory(Player* player, int resource, int nbResource);
void showPNJInventory(PNJ* pnj, Player* player, Item* allDurabilityItems);
void dropResources(int resource, Player* player, Map* pMap, thingToRespawn* head, int x, int y);
void interactWithPNJ(PNJ* pnj, Player* player, Item* allDurabilityItems);
void checkMapElement(Map* pMap, Player* player, int x, int y, thingToRespawn* respawnList);
void putPortalOnMap(Map* pmap1,Map* pmap2,Map* pmap3);
int getRandomNum(int limit);
void putElementHere(Map* pMap, int x, int y, int elementID);
void movePlayer(Player* player,  Map* pMap, char movement, thingToRespawn* respawnList);
#endif //PROJET_C_RPG_H
