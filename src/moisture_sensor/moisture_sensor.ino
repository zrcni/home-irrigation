const int SENSOR_OUTPUT_PIN = 3;
const int SENSOR_ANALOG_PIN = 0;
const int RELAY_OUTPUT_PIN = 10;
const int MOISTURE_LIMIT = 780;
const int PUMP_ON_DURATION = 2500;
// 30 minutes
const uint32_t CHECK_INTERVAL = 1800000UL;
// const uint32_t CHECK_INTERVAL = 5000L;

int value = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR_OUTPUT_PIN, OUTPUT);
  pinMode(SENSOR_ANALOG_PIN, INPUT);
  pinMode(RELAY_OUTPUT_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  analogWrite(SENSOR_OUTPUT_PIN, 255);

  // Wait a bit for the sensor to get
  // set up after powering it up.
  delay(100);

  value = analogRead(SENSOR_ANALOG_PIN);

  analogWrite(SENSOR_OUTPUT_PIN, 0);
 
  Serial.println(value);
 
  if (isDry(value)) {
    Serial.println("dry :(");
    analogWrite(RELAY_OUTPUT_PIN, 255);
    delay(PUMP_ON_DURATION);
    analogWrite(RELAY_OUTPUT_PIN, 0);
  } else {
    Serial.println("wet :)");
  }

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(CHECK_INTERVAL, DEC);
  delay(CHECK_INTERVAL);
}

bool isDry(int value) {
  return value > MOISTURE_LIMIT;
}
