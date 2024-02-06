#include <GyverTM1637.h> 
#include <Adafruit_NeoPixel.h> 
 
#define PIN_LED 9 //пин светодиодной ленты 
#define NUM_LED 42 //количество светодиодов в ленте 
#define CLK1 14 //пины для подключения 1-ого экрана 
#define CLK2 16 
#define DIO1 15 //пины для подключения 2-ого экрана 
#define DIO2 17 
 
//массив для работы со светодиодами 
byte table[7][6] = { 
 {0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0} 
}; // массив 7x6 
 
bool gameOver = false; //статус законченности игры 
 
//Работа с временем 
unsigned long lastTime = 0; 
 
byte ten_count1 = 0;//десятки в счётчике побед 1-ого игрока 
byte ten_count2 = 0;//десятки в счётчике побед 2-ого игрока 
byte wins_first_player = 0;//единицы в счётчике побед 1-ого игрока 
byte wins_second_player = 0;//единицы в счётчике побед 2-ого игрока 
 
//timer working для таймеров 
int minute1 = 3;//1-ого игрока 
int minute2 = 3;//2-ого игрока 
 
int tenSec1 = 0; 
int tenSec2 = 0; 
 
int second1 = 0; 
int second2 = 0; 
 
 
bool firstOut = false;//статус проигрыша 1 и 2 игроков 
bool secondOut = false; 
 
Adafruit_NeoPixel strip (NUM_LED, PIN_LED, NEO_GRB + NEO_KHZ800); //инициализация светодиодной ленты 
GyverTM1637 disp1 (CLK1, DIO1);//инициализация 1-ого дисплея 
GyverTM1637 disp2 (CLK2, DIO2);//инициализация 2-ого дисплея 
 
const int PIN_TOUCH[] = {8, 7, 6, 5, 4, 3, 2}; //Определение пинов сенсоров по секторам 
//мелодия приветствия игроков после включения устройства 
void HelloSound() { 
 tone(13,600); 
 delay(100); 
 tone(13,650); 
 delay(100); 
 tone(13,800); 
 delay(150); 
 noTone(13); 
} 
 
void PrintDisp() { 
 disp1.point(true); 
 disp2.point(true); 
 disp1.display(1,wins_first_player); 
 disp1.display(2,wins_second_player); 
 disp2.display(1,wins_first_player); 
 disp2.display(2,wins_second_player); 
 
} 
void setup(){ 
 //инициализация сенсоров 
 for (int i = 0; i < 7; i++) { 
 pinMode(PIN_TOUCH[i], INPUT); 
 } 
 
 Serial.begin(9600); 
 
 strip.begin(); //PORT 
 strip.setBrightness(255);//настройка яркости светодиодов 
 
 HelloSound(); 
 PrintDisp(); 
 
 //delay(2000); 
 
 disp1.display(1,minute1); 
 disp1.display(2,tenSec1); 
 disp1.display(3,second1); 
 
 disp2.display(1,minute2); 
 disp2.display(2,tenSec2); 
 disp2.display(3,second2); 
 
 strip.clear(); 
 disp1.brightness(7); 
 disp2.brightness(7); 
 strip.show(); 
} 
//вывод на дисплей оставшегося времени 1-ого игрока 
void timerAlg1(){ 
 second1 -= 1; 
 if(second1 == -1){ 
 if(tenSec1==0){ 
 if(minute1 == 0){ 
 //КОНЕЦ ТАЙМЕРА 
 disp1.scroll(1,0,0); 
 disp1.scroll(2,0,0); 
 disp1.scroll(3,0,0); 
 firstOut = true; 
 }else{ 
 minute1 -= 1; 
 tenSec1 = 5; 
 second1 = 9; 
 disp1.scroll(1,minute1,0); 
 disp1.scroll(2,tenSec1,0); 
 disp1.scroll(3,second1,0); 
 } 
 }else{ 
 second1 = 9; 
 tenSec1 -= 1; 
 disp1.scroll(2,tenSec1,0); 
 disp1.scroll(3,second1,0); 
 } 
 }else{ 
 disp1.scroll(3,second1,0); 
 } 
} 
//вывод на дисплей оставшегося времени 2-ого игрока 
void timerAlg2(){ 
 second2 -= 1; 
 if(second2 == -1){ 
 if(tenSec2==0){ 
 if(minute2 == 0){ 
 //КОНЕЦ ТАЙМЕРА 
 disp2.scroll(1,0,0); 
 disp2.scroll(2,0,0); 
 disp2.scroll(3,0,0); 
 secondOut = true; 
 }else{ 
 minute2 -= 1; 
 tenSec2 = 5; 
 second2 = 9; 
 disp2.scroll(1,minute2,0); 
 disp2.scroll(2,tenSec2,0); 
 disp2.scroll(3,second2,0); 
 } 
 }else{ 
 second2 = 9; 
 tenSec2 -= 1; 
 disp2.scroll(2,tenSec2,0); 
 disp2.scroll(3,second2,0); 
 } 
 }else{ 
 disp2.scroll(3,second2,0); 
 } 
} 
byte player=0; 
//инициализация цветов 
uint32_t color; 
uint32_t player_color[2] ={ 
 strip.Color(255, 0, 0), strip.Color(0, 0, 255) 
}; 
//функция записи ход в массив 
void make_turn(byte col){ 
 for(int i = 0; i < 6; i++) { 
 if (table[col][i]==0){ 
 table[col][i]=(player+1); 
 if(player==0){ 
 player=1; 
 }else{ 
 player=0; 
 } 
 break; 
 } 
 } 
} 
void animation_make_turn(byte col){ 
 for(int i = 5;i > 0;i--){ 
 if(table[col][i] == 0){ 
 strip.setPixelColor(i+col*6,player_color[player]); 
 strip.show(); 
 tone(13,500); 
 delay(50); 
 noTone(13); 
 delay(50);