#include <stdint.h>

#define CLUSTER_CHARS 6
#define WELL_CHARS 6
#define NAME_CHARS 20
#define MAC_CHARS 6

#define Shop_default 0
#define Operator_default 0
#define Field_default 0
#define Cluster_default "0000"

#define NAME_default "0000"
#define NUM_default 0
#define TYPE_default 0
#define DEVICES 4

typedef struct devices
{
	uint8_t Name[NAME_CHARS];
	uint32_t Num;
	uint8_t mac_addr[MAC_CHARS];
	uint16_t Type;
	uint8_t Well[WELL_CHARS];
	uint32_t onTime;
} devices_t;

typedef struct position
{
	uint16_t Shop;		 //номер цеха
	uint16_t Operator; //номер оператора
	uint16_t Field;	 //код месторождения
	// uint8_t Well[WELL_CHARS];		  //номер скважины
	uint8_t Cluster[CLUSTER_CHARS]; //номер куста
} position_t;

void print_hello();
uint32_t strtoint(char *str);