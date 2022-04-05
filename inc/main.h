#include <stdint.h>

#define CLUSTER_CHARS 6 //размер номера куста
#define WELL_CHARS 6		//резмер номера скважины
#define NAME_CHARS 20	//размер именя
#define MAC_CHARS 6		//размер mac адресса

#define SHOP_DEFAULT 0			 //цех
#define OPERATOR_DEFAULT 0		 //оператор
#define FIELD_DEFAULT 0			 //месторождение
#define CLUSTER_DEFAULT "0000" //куст

#define NAME_DEFAULT "0000"	  //имя
#define NUM_DEFAULT 0			  //номер
#define MAC_DEFAULT 0			  // mac адресс
#define WELL_DEFAULT 0			  //номер скважины
#define ONTIME_DEFAULT 0		  //период включения
#define HOLE_PRESSURE_DEFAULT 0 //затрубное давление
#define BUF_PRESSURE_DEFAULT 0  //буферное давление
#define LINE_PRESSURE_DEFAULT 0 //линейное давление

#define GEN_PUMP_TYPE 0 //обычный (балансир)
#define CHN_PUMP_TYPE 1 //цепной
#define HDR_PUMP_TYPE 2 //гидро

#define DYN_PERIOD_DEFAULT 10000		 //период
#define DYN_APERTURE_DEFAULT 1		 //отверстие
#define DYN_TRAVEL_DEFAULT 4000		 //перемещение
#define DYN_PU_DEFAULT GEN_PUMP_TYPE //тип привода
#define DYN_ROD_DEFAULT 320			 //шток

//биты режима записи эхограммы
#define INLET BIT(0)		 //впуск газа (инверсный сигнал)
#define AUTO BIT(1)		 //срабатывание автоматического клапана
#define DUR_AUTO BIT(2)	 //длительность срабатывания авт. клапана
#define KVD BIT(5)		 //переключение в КВД при уровне < 50м, или кол. отражений > 8
#define LEVEL6000 BIT(6) //макс. уровень - 6000м
#define GAIN BIT(9)		 //доп. усиление
#define SERV BIT(15)		 //сервисный режим
#define BIT(n) (1 << n)

#define STATIC_ECHO_RESEARCH_TYPE 1	 //статическая эхограмма
#define DYNAMIC_ECHO_RESEARCH_TYPE 2 //динамическая эхограмма
#define KVU_ECHO_RESEARCH_TYPE 3		 //КВУ
#define KPU_ECHO_RESEARCH_TYPE 4		 //КПУ

#define DEVICES 4 //колличество девайсов в планировщике

typedef enum
{
	FIND_ERROR_NULL_PARAM = 0, //ошибка, переданы нулевые параметры
	FIND_PARAM_TRUE,				//найдены параметры
	FIND_PARAM_DEFAULT,			//парамеры не найдены
} find_return_t;

typedef struct param_ddim
{
	uint32_t dyn_period;	  //период качания, мс
	uint16_t dyn_aperture; //номер отверстия
	uint16_t dyn_travel;	  //длина хода, мм
	uint16_t dyn_pu;		  //тип привода шгн
	uint16_t dyn_rod;		  //диаметр штока, 0.1мм
} param_ddim_t;

typedef struct param_dua
{
	uint16_t echotype; //тип эхограммы
	uint16_t echomode; //режим исследования
	uint16_t table;	 //таблица скорости звука
	uint16_t speed;	 //скорость звука, 0.1м/с
} param_dua_t;

typedef struct devices
{
	uint8_t name[NAME_CHARS];	  //имя приборsа
	uint32_t num;					  //номер прибора
	uint8_t mac_addr[MAC_CHARS]; // mac адресс
	uint8_t well[WELL_CHARS];	  //номер скважины
	int16_t hole_press;			  //затрубное давление, 0.1 атм
	int16_t buf_press;			  //буферное давление, 0.1 атм
	int16_t line_press;			  //линейное давление, 0.1 атм
	uint32_t ontime;				  //период включения
	uint32_t *param_device;		  //указатель на параметры
} devices_t;

typedef struct position
{
	uint16_t shop;						  //номер цеха
	uint16_t oper;						  //номер оператора
	uint16_t field;					  //код месторождения
	uint8_t cluster[CLUSTER_CHARS]; //номер куста
} position_t;

void print_hello();
uint32_t strtoint(char *str);