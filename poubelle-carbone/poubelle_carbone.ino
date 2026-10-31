#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // RS, E, D4, D5, D6, D7

const int NB = 2;                         // 0 = recyclable, 1 = autre
const int PIN_LDR[NB] = {A0, A1};
const int CO2_PAR_JET[NB] = {20, 120};    // grammes estimes par dechet : a ajuster

const int PIN_VERT = 6;
const int PIN_JAUNE = 7;
const int PIN_BUZZER = 8;
const int PIN_ROUGE = 9;
const int PIN_RESET = 10;

const int SEUIL_JAUNE = 200;              // grammes cumules
const int SEUIL_ROUGE = 500;
const int BASE_MIN = 40;                  // lumiere minimale pour qu'un capteur soit actif

int seuil[NB];
bool actif[NB];                           // false si le capteur ne recoit pas de lumiere
bool coupe[NB];
bool compte[NB];
unsigned long debut[NB];
unsigned long fin[NB];
int nbJets[NB];
long totalCO2 = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(PIN_VERT, OUTPUT);
  pinMode(PIN_JAUNE, OUTPUT);
  pinMode(PIN_ROUGE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RESET, INPUT_PULLUP);
  Serial.begin(9600);
  calibrer();
  afficher();
}

void loop() {
  unsigned long maintenant = millis();

  for (int i = 0; i < NB; i++) {
    if (!actif[i]) continue;                // capteur en panne : on l'ignore
    bool bloque = lire(PIN_LDR[i]) < seuil[i];

    if (bloque && !coupe[i] && maintenant - fin[i] > 250) {
      // un dechet coupe le faisceau : on compte tout de suite
      // (un objet qui tombe ne reste que quelques ms devant le capteur)
      coupe[i] = true;
      Serial.print("Jet capteur ");
      Serial.println(i);
      nbJets[i]++;
      totalCO2 += CO2_PAR_JET[i];
      signaler(i);
      afficher();
    } else if (!bloque && coupe[i]) {
      coupe[i] = false;
      fin[i] = maintenant;
    }
  }

  if (digitalRead(PIN_RESET) == LOW) {
    remiseAZero();
  }
  delay(1);
}

int lire(int pin) {
  // 4 lectures rapides, on garde la plus basse : attrape les coupures tres breves
  int m = 1023;
  for (int k = 0; k < 4; k++) {
    int v = analogRead(pin);
    if (v < m) m = v;
  }
  return m;
}

void calibrer() {
  lcd.clear();
  lcd.print("Calibrage...");
  lcd.setCursor(0, 1);
  lcd.print("Ne touche a rien");
  delay(1500);

  for (int i = 0; i < NB; i++) {
    long somme = 0;
    for (int k = 0; k < 30; k++) {
      somme += analogRead(PIN_LDR[i]);
      delay(10);
    }
    int base = somme / 30;
    seuil[i] = base * 70 / 100;  // coupe si la valeur tombe sous 70 % de la normale

    Serial.print("Capteur ");
    Serial.print(i);
    Serial.print(" base=");
    Serial.print(base);
    Serial.print(" seuil=");
    Serial.println(seuil[i]);

    actif[i] = (base >= BASE_MIN);
    if (base < BASE_MIN) {
      lcd.clear();
      lcd.print("Capteur ");
      lcd.print(i + 1);
      lcd.setCursor(0, 1);
      lcd.print("pas de lumiere");
      delay(2500);
    }
  }
  lcd.clear();
}

void signaler(int i) {
  if (i == 0) {
    tone(PIN_BUZZER, 1500, 120);   // recyclable : aigu et court
  } else {
    tone(PIN_BUZZER, 300, 300);    // autre : grave et long
  }
}

void afficher() {
  char l1[17];
  char l2[17];
  snprintf(l1, sizeof(l1), "CO2:%5ld g", totalCO2);
  snprintf(l2, sizeof(l2), "Reco:%-2d Aut:%-3d", nbJets[0], nbJets[1]);
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);

  digitalWrite(PIN_VERT, HIGH);
  digitalWrite(PIN_JAUNE, totalCO2 >= SEUIL_JAUNE);
  digitalWrite(PIN_ROUGE, totalCO2 >= SEUIL_ROUGE);
}

void remiseAZero() {
  totalCO2 = 0;
  nbJets[0] = 0;
  nbJets[1] = 0;
  lcd.clear();
  afficher();
  tone(PIN_BUZZER, 600, 150);
  delay(250);
  tone(PIN_BUZZER, 600, 150);
  delay(500);
}
