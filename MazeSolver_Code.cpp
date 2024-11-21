// Pin assignment
#define lSensor 4    // Left sensor
#define cSensor 2    // Center sensor
#define rSensor 3    // Right sensor

#define lMotPos 8    // Left motor (+)
#define lMotNeg 7    // Left motor (-)

#define rMotNeg 12   // Right motor (-)
#define rMotPos 13   // Right motor (+)

#define lMotPWM 6    // Left motor speed (PWM)
#define rMotPWM 5    // Right motor speed (PWM)

int lMotSpeed = 100; // Speed for left motor
int rMotSpeed = 100; // Speed for right motor

// Stack for storing movement history (to backtrack in the maze)
#define STACK_SIZE 50
char pathStack[STACK_SIZE];
int stackTop = -1;

// Push a direction onto the stack
void push(char direction) {
  if (stackTop < STACK_SIZE - 1) {
    pathStack[++stackTop] = direction;
  }
}

// Pop the last direction from the stack
char pop() {
  if (stackTop >= 0) {
    return pathStack[stackTop--];
  }
  return 'N'; // Return 'N' for no direction if stack is empty
}

void setup() {
  // Initialize sensors
  pinMode(lSensor, INPUT);
  pinMode(cSensor, INPUT);
  pinMode(rSensor, INPUT);

  // Initialize motor driver pins
  pinMode(lMotNeg, OUTPUT);
  pinMode(lMotPos, OUTPUT);
  pinMode(lMotPWM, OUTPUT);

  pinMode(rMotNeg, OUTPUT);
  pinMode(rMotPos, OUTPUT);
  pinMode(rMotPWM, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Read sensor values
  int lSensStat = digitalRead(lSensor);  // Left sensor
  int cSensStat = digitalRead(cSensor); // Center sensor
  int rSensStat = digitalRead(rSensor);  // Right sensor

  // Line-following logic with maze-solving enhancements
  if (cSensStat && !lSensStat && !rSensStat) {
    forward();
    Serial.println("Forward");
  } else if (!cSensStat && lSensStat && !rSensStat) {
    turnLeft();
    push('L'); // Log left turn
    Serial.println("Turn Left");
  } else if (!cSensStat && !lSensStat && rSensStat) {
    turnRight();
    push('R'); // Log right turn
    Serial.println("Turn Right");
  } else if (cSensStat && lSensStat && rSensStat) {
    turnRight();
    push('R'); // Log right turn at intersection
    Serial.println("Intersection - Turn Right");
  } else if (!cSensStat && !lSensStat && !rSensStat) {
    handleDeadEnd(); // Handle dead end
  } else {
    searchLine();
    Serial.println("Line Lost - Searching");
  }
}

// Movement functions
void forward() {
  digitalWrite(lMotNeg, LOW);
  digitalWrite(lMotPos, HIGH);
  analogWrite(lMotPWM, lMotSpeed);

  digitalWrite(rMotNeg, LOW);
  digitalWrite(rMotPos, HIGH);
  analogWrite(rMotPWM, rMotSpeed);
}

void turnLeft() {
  digitalWrite(lMotNeg, HIGH); // Left motor reverse
  digitalWrite(lMotPos, LOW);
  analogWrite(lMotPWM, lMotSpeed);

  digitalWrite(rMotNeg, LOW); // Right motor forward
  digitalWrite(rMotPos, HIGH);
  analogWrite(rMotPWM, rMotSpeed);
  delay(300); // Adjust delay for a smooth turn
}

void turnRight() {
  digitalWrite(lMotNeg, LOW); // Left motor forward
  digitalWrite(lMotPos, HIGH);
  analogWrite(lMotPWM, lMotSpeed);

  digitalWrite(rMotNeg, HIGH); // Right motor reverse
  digitalWrite(rMotPos, LOW);
  analogWrite(rMotPWM, rMotSpeed);
  delay(300); // Adjust delay for a smooth turn
}

void turnAround() {
  digitalWrite(lMotNeg, HIGH); // Left motor reverse
  digitalWrite(lMotPos, LOW);
  digitalWrite(rMotNeg, LOW); // Right motor forward
  digitalWrite(rMotPos, HIGH);
  delay(600); // Longer delay for 180° turn
  stop();
}

void stop() {
  digitalWrite(lMotNeg, LOW);
  digitalWrite(lMotPos, LOW);
  digitalWrite(rMotNeg, LOW);
  digitalWrite(rMotPos, LOW);
}

void handleDeadEnd() {
  char lastMove = pop(); // Get last move from stack
  if (lastMove == 'L') {
    turnRight(); // Reverse a left turn
    Serial.println("Dead End - Reversing Left Turn");
  } else if (lastMove == 'R') {
    turnLeft(); // Reverse a right turn
    Serial.println("Dead End - Reversing Right Turn");
  } else {
    turnAround(); // If no history, perform a 180° turn
    Serial.println("Dead End - Turning Around");
  }
}

void searchLine() {
  // Rotate slowly to find the line
  for (int i = 0; i < 2; i++) {
    // Rotate left
    digitalWrite(lMotNeg, LOW);
    digitalWrite(lMotPos, HIGH); // Left motor forward
    digitalWrite(rMotNeg, HIGH);
    digitalWrite(rMotPos, LOW); // Right motor reverse
    delay(500); // Adjust for searching duration

    // Check for line after rotating
    if (digitalRead(cSensor) || digitalRead(lSensor) || digitalRead(rSensor)) {
      return; // Exit if line is found
    }

    // Rotate right
    digitalWrite(lMotNeg, HIGH);
    digitalWrite(lMotPos, LOW); // Left motor reverse
    digitalWrite(rMotNeg, LOW);
    digitalWrite(rMotPos, HIGH); // Right motor forward
    delay(500); // Adjust for searching duration

    // Check for line after rotating
    if (digitalRead(cSensor) || digitalRead(lSensor) || digitalRead(rSensor)) {
      return; // Exit if line is found
    }
  }
  // If no line is found, stop
  stop();
}
