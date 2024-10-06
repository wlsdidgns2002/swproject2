#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100.0
#define _DIST_MAX 300.0
#define _DIST_OPT 200.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);
}

void loop() {
  float distance;

  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  if (distance == 0.0 || isinf(distance)) {
    Serial.println("Error: Invalid distance measurement (0 or infinity).");
    Serial.print("Measured distance: "); Serial.println(distance);
    return;
  }

  if (distance > _DIST_MAX) {
    distance = _DIST_MAX + 10.0;
  } else if (distance < _DIST_MIN) {
    distance = _DIST_MIN - 10.0;
  }

  int brightness = calculate_brightness(distance);
  analogWrite(PIN_LED, brightness);

  Serial.print("Distance: ");  Serial.print(distance);
  Serial.print(" mm, Brightness: ");  Serial.println(brightness);

  last_sampling_time += INTERVAL;
}

int calculate_brightness(float distance) {
  float brightness;

  if (distance <= _DIST_MIN || distance >= _DIST_MAX) {
    brightness = 255;
  } else if (distance <= _DIST_OPT) {
    brightness = 255 * (1 - (distance - _DIST_MIN) / (_DIST_OPT - _DIST_MIN));
  } else {
    brightness = 255 * ((distance - _DIST_OPT) / (_DIST_MAX - _DIST_OPT));
  }

  if (isnan(brightness) || isinf(brightness)) {
    Serial.println("Error: Invalid brightness calculation (NaN or infinity).");
    Serial.print("Brightness value: "); Serial.println(brightness);
  }

  return constrain(brightness, 0, 255);
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
