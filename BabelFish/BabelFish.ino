#include <IRremote.h>

int RECV_PIN = 11;
int BEEP_PIN = 6;

unsigned long INTERVALL;


unsigned long SamsTVon = 0xE0E040BF;
unsigned long SamsVolPlus = 0xE0E0E01F;
unsigned long SamsVolMinus = 0xE0E0D02F;
unsigned long SamsVolMute = 0xE0E0F00F;

unsigned long SonyTVon = 0xA90;
unsigned long SonyTVMinus = 0xC90;

unsigned long YvolPlus = 0x5EA158A7;
unsigned long YvolMinus = 0x5EA1D827;
unsigned long YvolMute = 0x5EA138C7;

long lastMovement;
int NBRVOLUME = 5;


IRrecv irrecv(RECV_PIN);

IRsend irsend;

decode_results results;

void setup()
{
  // setup LED pins
  pinMode(BEEP_PIN, OUTPUT);
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  lastMovement = millis();

  INTERVALL = 20 * 60000; // 20 minutes
}

void loop() {
  if (millis() > lastMovement + INTERVALL) {
    lastMovement = millis();
    Serial.println("Watchdog");
    beep();
    for (int i = 0; i < 8; i++) {
      irsend.sendSony(SonyTVMinus, 12);
      delay(20);
    }
    irrecv.enableIRIn();
  }
  if (irrecv.decode(&results)) {
    Serial.println(results.value , HEX);
    irrecv.resume(); // Receive the next value
    if (results.value == SamsVolPlus) {
      beep();
      sendPlus(NBRVOLUME);
      lastMovement = millis();
    }
    if (results.value == SamsVolMinus) {
      beep();
      sendMinus(NBRVOLUME);
      lastMovement = millis();
    }
    if (results.value == SamsVolMute) {
      beep();
      Serial.println("Mute");
      irsend.sendNEC(YvolMute, 32);
      irrecv.enableIRIn();
      lastMovement = millis();
    }
    if (results.value == SamsTVon) {
      beep();
      Serial.println("TV on");
      for (int i = 0; i < 10; i++) {
        irsend.sendSony(0xA90, 12);
        delay(40);
      }
      irrecv.enableIRIn();
      lastMovement = millis();
    }
  }
}

void sendPlus(int nbr) {
  int i = nbr;
  Serial.println("Plus");
  while (i > 1) {
    irsend.sendNEC(YvolPlus, 32);
    irrecv.enableIRIn();
    delay(400);
    i--;
  }
  irsend.sendNEC(YvolPlus, 32);
  irrecv.enableIRIn();
}

void sendMinus(int nbr) {
  int i = nbr;
  Serial.println("Minus");
  while (i > 1) {
    irsend.sendNEC(YvolMinus, 32);
    irrecv.enableIRIn();
    delay(400);
    i--;
  }
  irsend.sendNEC(YvolMinus, 32);
  irrecv.enableIRIn();

}

void beep() {
  digitalWrite(BEEP_PIN, HIGH);
  delay(100);
  digitalWrite(BEEP_PIN, LOW);
}
