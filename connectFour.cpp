#include <GyverTM1637.h>
#include <Adafruit_NeoPixel.h>

#define PIN_LED 9 //пин светодиодной ленты
#define NUM_LED 42 //количество светодиодов в ленте
#define CLK1 14 //пины для подключения 1-ого экрана
#define CLK2 16
#define DIO1 15 //пины для подключения 2-ого экрана
#define DIO2 17

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

byte ten_count1 = 0;
byte ten_count2 = 0;
byte wins_first_player = 0;
byte wins_second_player = 0;

//timer working
int minute1 = 3;
int minute2 = 3;

int tenSec1 = 0;
int tenSec2 = 0;

int second1 = 0;
int second2 = 0;

bool firstOut = false;
bool secondOut = false;

Adafruit_NeoPixel strip(NUM_LED, PIN_LED, NEO_GRB + NEO_KHZ800); //инициализация светодиодной ленты
GyverTM1637 disp1(CLK1, DIO1);
GyverTM1637 disp2(CLK2, DIO2);

const int PIN_TOUCH[] = { 8, 7, 6, 5, 4, 3, 2 }; //Определение пинов сенсоров по секторам

void HelloSound() {
    tone(13, 600);
    delay(100);
    tone(13, 650);
    delay(100);
    tone(13, 800);
    delay(150);
    noTone(13);
}
void PrintDisp() {
    disp1.point(true);
    disp2.point(true);
    disp1.display(1, wins_first_player);
    disp1.display(2, wins_second_player);
    disp2.display(1, wins_first_player);
    disp2.display(2, wins_second_player);

}
void setup() {
    //инициализация сенсоров
    for (int i = 0; i < 7; i++) {
        pinMode(PIN_TOUCH[i], INPUT);
    }

    Serial.begin(9600);

    strip.begin(); //PORT
    strip.setBrightness(255);//настройка яркости светодиодов

    //display setup
    HelloSound();
    PrintDisp();

    //delay(2000);

    disp1.display(1, minute1);
    disp1.display(2, tenSec1);
    disp1.display(3, second1);

    disp2.display(1, minute2);
    disp2.display(2, tenSec2);
    disp2.display(3, second2);

    strip.clear();
    disp1.brightness(7);
    disp2.brightness(7);
    strip.show();
}

void timerAlg1() {
    second1 -= 1;
    if (second1 == -1) {
        if (tenSec1 == 0) {
            if (minute1 == 0) {
                //КОНЕЦ ТАЙМЕРА
                disp1.scroll(1, 0, 0);
                disp1.scroll(2, 0, 0);
                disp1.scroll(3, 0, 0);
                firstOut = true;
            }
            else {
                minute1 -= 1;
                tenSec1 = 5;
                second1 = 9;
                disp1.scroll(1, minute1, 0);
                disp1.scroll(2, tenSec1, 0);
                disp1.scroll(3, second1, 0);
            }
        }
        else {
            second1 = 9;
            tenSec1 -= 1;
            disp1.scroll(2, tenSec1, 0);
            disp1.scroll(3, second1, 0);
        }
    }
    else {
        disp1.scroll(3, second1, 0);
    }
}
void timerAlg2() {
    second2 -= 1;
    if (second2 == -1) {
        if (tenSec2 == 0) {
            if (minute2 == 0) {
                //КОНЕЦ ТАЙМЕРА
                disp2.scroll(1, 0, 0);
                disp2.scroll(2, 0, 0);
                disp2.scroll(3, 0, 0);
                secondOut = true;
            }
            else {
                minute2 -= 1;
                tenSec2 = 5;
                second2 = 9;
                disp2.scroll(1, minute2, 0);
                disp2.scroll(2, tenSec2, 0);
                disp2.scroll(3, second2, 0);
            }
        }
        else {
            second2 = 9;
            tenSec2 -= 1;
            disp2.scroll(2, tenSec2, 0);
            disp2.scroll(3, second2, 0);
        }
    }
    else {
        disp2.scroll(3, second2, 0);
    }
}

uint32_t color;
byte player = 0;

uint32_t player_color[2] = {
  strip.Color(255, 0, 0), strip.Color(0, 0, 255)
};

void make_turn(byte col) {
    for (int i = 0; i < 6; i++) {
        if (table[col][i] == 0) {
            table[col][i] = (player + 1);
            if (player == 0) {
                player = 1;
            }
            else {
                player = 0;
            }
            break;
        }
    }
}

void draw_all() {
    strip.clear();
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            if (table[i][j] > 0) {
                strip.setPixelColor(j + i * 6, player_color[table[i][j] - 1]);
                //Serial.print(player_color[table[i][j]]);
            }
        }
    }
    strip.show();
}

byte win_check() {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            if (table[i][j] != 0 && table[i][j] == table[i][j + 1] && table[i][j] == table[i][j + 2] && table[i][j] == table[i][j + 3]) {
                flash_dio(i, j, 0, table[i][j]);
                byte x = table[i][j];
                return x;
            }
        }
    }
    for (int j = 0; j < 6; j++) {
        for (int i = 0; i < 4; i++) {
            if (table[i][j] != 0 && table[i][j] == table[i + 1][j] && table[i][j] == table[i + 2][j] && table[i][j] == table[i + 3][j]) {
                flash_dio(i, j, 1, table[i][j]);
                byte x = table[i][j];
                return x;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (table[i][j] != 0 && table[i][j] == table[i + 1][j + 1] && table[i][j] == table[i + 2][j + 2] && table[i][j] == table[i + 3][j + 3]) {
                flash_dio(i, j, 2, table[i][j]);
                byte x = table[i][j];
                return x;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 3; j < 6; j++) {
            if (table[i][j] != 0 && table[i][j] == table[i + 1][j - 1] && table[i][j] == table[i + 2][j - 2] && table[i][j] == table[i + 3][j - 3]) {
                flash_dio(i, j, 3, table[i][j]);
                byte x = table[i][j];
                return x;
            }
        }
    }
    return 0;
}
byte nech_check() {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            if (table[i][j] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

void ShowScore() {
    disp1.scroll(0, ten_count1, 0);
    disp1.scroll(1, wins_first_player, 0);
    disp2.scroll(0, ten_count1, 0);
    disp2.scroll(1, wins_first_player, 0);
    disp1.scroll(2, ten_count2, 10);
    disp1.scroll(3, wins_second_player, 10);
    disp2.scroll(2, ten_count2, 10);
    disp2.scroll(3, wins_second_player, 10);
}

void win_music() {
    tone(13, 1000);
    delay(100);
    tone(13, 1500);
    delay(100);
    tone(13, 1600);
    delay(150);
    tone(13, 1000);
    delay(100);
    tone(13, 1500);
    delay(100);
    tone(13, 1600);
    delay(150);
    noTone(13);
    tone(13, 1000);
    delay(100);
    tone(13, 1500);
    delay(100);
    tone(13, 1600);
    delay(150);
    noTone(13);
}
void turn_music() {
    tone(13, 1000);
    delay(100);
    tone(13, 1500);
    delay(100);
    tone(13, 1600);
    delay(150);
    noTone(13);
}
void clear_pole() {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            table[i][j] = 0;
        }
    }
    draw_all();
}
void restart() {
    delay(5000);
    disp1.clear();
    disp2.clear();
    minute1 = 3;
    minute2 = 3;
    tenSec1 = 0;
    tenSec2 = 0;
    second1 = 0;
    second2 = 0;
    disp1.display(1, minute1);
    disp1.display(2, tenSec1);
    disp1.display(3, second1);

    disp2.display(1, minute2);
    disp2.display(2, tenSec2);
    disp2.display(3, second2);
    firstOut = false;
    secondOut = false;
}
void player_win(byte whowin) {
    if (whowin == 2) {
        wins_second_player += 1;
        if (wins_second_player == 10) {
            ten_count2++;
            wins_second_player = 0;
        }
    }
    else if (whowin == 1) {
        wins_first_player += 1;
        if (wins_first_player == 10) {
            ten_count1++;
            wins_first_player = 0;
        }
    }
}


void flash_dio(byte i, byte j, byte v, byte p) {
    for (byte z = 0; z < 4; z++) {
        clear_pole();
        draw_all();
        delay(300);
        //horizontal
        if (v == 0) {
            table[i][j] = p;
            table[i][j + 1] = p;
            table[i][j + 2] = p;
            table[i][j + 3] = p;
        }
        else if (v == 1) {
            table[i][j] = p;
            table[i + 1][j] = p;
            table[i + 2][j] = p;
            table[i + 3][j] = p;
        }
        else if (v == 2) {
            table[i][j] = p;
            table[i + 1][j + 1] = p;
            table[i + 2][j + 2] = p;
            table[i + 3][j + 3] = p;
        }
        else if (v == 3) {
            table[i][j] = p;
            table[i + 1][j - 1] = p;
            table[i + 2][j - 2] = p;
            table[i + 3][j - 3] = p;
        }
        draw_all();
        delay(300);
    }
}
void loop() {
    //unsigned long currentMillis = (millis() -2000);
    unsigned long currentMillis = (millis());
    if (player == 0) {
        if (currentMillis - lastTime >= 1000) {
            lastTime = currentMillis;
            timerAlg1();
            if (firstOut) {
                player_win(2);
                win_music();
                clear_pole();
                //delay(400);
                disp1.clear();
                disp2.clear();
                ShowScore();
                //КОНЕЦ ИГРЫ
                //ScorePrint();
                //delay(2000);
                restart();
            }
        }
    }
    else if (player == 1) {
        if (currentMillis - lastTime >= 1000) {
            lastTime = currentMillis;
            timerAlg2();
            if (secondOut) {
                player_win(1);
                win_music();
                clear_pole();
                //delay(400);
                disp1.clear();
                disp2.clear();
                ShowScore();
                //ScorePrint();
                //delay(2000);
                restart();
            }
        }
    }

    if (digitalRead(PIN_TOUCH[0]) == HIGH || digitalRead(PIN_TOUCH[1]) == HIGH || digitalRead(PIN_TOUCH[2]) == HIGH || digitalRead(PIN_TOUCH[3]) == HIGH || digitalRead(PIN_TOUCH[4]) == HIGH || digitalRead(PIN_TOUCH[5]) == HIGH || digitalRead(PIN_TOUCH[6]) == HIGH) {
        for (int i = 0; i < 7; i++) {
            if (digitalRead(PIN_TOUCH[i]) == HIGH) {
                make_turn(i);
                draw_all();
                turn_music();
                delay(400);
                byte whowin = win_check();
                if (whowin > 0) {
                    //ScorePrint();
                    player_win(whowin);
                    win_music();
                    clear_pole();
                    //delay(400);
                    disp1.clear();
                    disp2.clear();
                    ShowScore();
                    restart();
                }
                if (nech_check() > 0) {
                    win_music();
                    clear_pole();
                    disp1.clear();
                    disp2.clear();
                    ShowScore();
                    restart();
                }
                break;
            }
        }

    }

}
