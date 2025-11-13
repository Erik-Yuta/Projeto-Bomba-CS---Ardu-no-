#include <Keypad.h>

int Hours = 0, Minutes = 0, Seconds = 0;
int trycount = 0;
int keycount = 0;

long secMillis = 0;
long interval = 1000; 

char password[4];
char entered[4];

// === TECLADO ===
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '#', '0'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {A0, A1, A2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === BUZZER ===
#define BUZZER 10

// === Funções auxiliares ===
void beepOk()   { tone(BUZZER, 4000, 80); delay(80); }
void beepErr()  { tone(BUZZER, 1200, 200); delay(200); }
void beepFast() { tone(BUZZER, 5000, 50); delay(50); }
void beepExplode() { tone(BUZZER, 700, 500); delay(200); }

void showTime() {
  Serial.print("Tempo: ");
  if (Hours < 10) Serial.print("0");
  Serial.print(Hours);
  Serial.print(":");
  if (Minutes < 10) Serial.print("0");
  Serial.print(Minutes);
  Serial.print(":");
  if (Seconds < 10) Serial.print("0");
  Serial.println(Seconds);
}

void defineCode() {
  Serial.println("=== ARME A BOMBA ===");
  Serial.print("Digite o código de 4 dígitos: ");
  keycount = 0;

  while (keycount < 4) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      password[keycount] = key;
      Serial.print("*");
      beepOk();
      keycount++;
    }
  }

  delay(500);
  Serial.println("\nCódigo salvo com sucesso!");
  delay(1500);
}

void defineTimer() {
  int digits[6] = {0};
  Serial.println("=== DEFINA O TEMPO (HHMMSS) ===");
  keycount = 0;

  while (keycount < 6) {
    char k = keypad.getKey();
    if (k >= '0' && k <= '9') {
      digits[keycount] = k - '0';
      Serial.print(k);
      beepOk();
      keycount++;
    }
  }

  Hours   = digits[0] * 10 + digits[1];
  Minutes = digits[2] * 10 + digits[3];
  Seconds = digits[4] * 10 + digits[5];

  Serial.println("\nTempo definido:");
  showTime();
  delay(2000);
}

void armBomb() {
  Serial.println("Pressione # para armar a bomba...");
  while (true) {
    char k = keypad.getKey();
    if (k == '#') {
      beepOk();
      Serial.println("BOMBA ARMADA! Iniciando...");
      delay(2000);
      break;
    }
  }
}

void disarmBomb() {
  Serial.println("Digite o código para desarmar:");
  keycount = 0;

  while (keycount < 4) {
    char k = keypad.getKey();
    if (k == '#') {  
      keycount = 0;
      Serial.println("Reiniciando tentativa...");
      beepErr();
    } else if (k >= '0' && k <= '9') {
      entered[keycount] = k;
      Serial.print("*");
      beepOk();
      keycount++;
    }
  }

  bool ok = true;
  for (int i = 0; i < 4; i++) {
    if (entered[i] != password[i]) ok = false;
  }

  if (ok) {
    Serial.println("\nBOMBA DESARMADA! 🎉");
    for (int i = 0; i < 3; i++) { beepOk(); delay(150); }
    while (1);
  } else {
    Serial.println("\nSENHA ERRADA!");
    beepErr();
    trycount++;

    if (trycount == 1) {
      Hours /= 2; Minutes /= 2; Seconds /= 2;
    } else if (trycount == 2) {
      Hours /= 2; Minutes /= 2; Seconds /= 2;
      interval /= 10; 
    } else if (trycount >= 3) {
      Hours = 0; Minutes = 0; Seconds = 0;
    }
    delay(2000);
  }
}

void explode() {
  Serial.println(" BOOM! A BOMBA EXPLODIU ");
  for (int i = 0; i < 10; i++) {
    beepExplode();
    delay(150);
  }
  while (1);
}

void timer() {
  unsigned long currentMillis = millis();
  if (currentMillis - secMillis >= interval) {
    secMillis = currentMillis;
    if (Seconds > 0) Seconds--;
    else {
      if (Minutes > 0) {
        Minutes--;
        Seconds = 59;
      } else if (Hours > 0) {
        Hours--;
        Minutes = 59;
        Seconds = 59;
      } else {
        explode();
      }
    }
    showTime();
    beepFast();
  }
}

void setup() {
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  Serial.println("=== SIMULADOR DE BOMBA ===");

  defineCode();
  defineTimer();
  armBomb();
}

void loop() {
  timer();

  char k = keypad.getKey();
  if (k == '*') {
    disarmBomb();
  }
}
