const int SENSOR_OUTPUT_PIN = 3;
const int SENSOR_ANALOG_PIN = 0;
const int RELAY_OUTPUT_PIN = 10;
const int MOISTURE_LIMIT = 800;

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
    delay(1000);
    analogWrite(RELAY_OUTPUT_PIN, 0);
  } else {
    Serial.println("wet :)");
  }

  digitalWrite(LED_BUILTIN, LOW);
  
  delay(5000);
}

bool isDry(int value) {
  return value > MOISTURE_LIMIT;
}
