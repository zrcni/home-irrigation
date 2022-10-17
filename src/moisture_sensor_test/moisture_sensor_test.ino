// power
const int OUTPUT_PIN = 3;
// sig
const int ANALOG_PIN = 0;
const int MOISTURE_LIMIT = 800;

int value = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(ANALOG_PIN, INPUT);
}

void loop()
{
  analogWrite(OUTPUT_PIN, 255);
  delay(200);

  digitalWrite(LED_BUILTIN, HIGH);

  value = analogRead(ANALOG_PIN);

  if (isWet(value)) {
    Serial.println("wet :)");
  } else {
    Serial.println("dry :(");
  }

  digitalWrite(LED_BUILTIN, LOW);

  analogWrite(OUTPUT_PIN, 0);

  delay(2000);
}

bool isWet(int value) {
  return MOISTURE_LIMIT > value;
}
