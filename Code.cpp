//Pin assignment
#define lSensor 4 //left sensor input
#define rSensor 3 //right sensor input

#define lMotPos 8  //left motor(+)
#define lMotNeg 7  //left motor(-)

#define rMotNeg 12  //right motor (-)
#define rMotPos 13 //right motor (+)

#define lMotPWM 6 //left motor speed(pwm) pin
#define rMotPWM 5 //right motor speed(pwm) pin

int lMotSpeed = 100;
int rMotSpeed = 100;

void setup()
{
  TCCR0B = TCCR0B & B11111000 | B00000010 ;
  //IR sensor pins
  pinMode (lSensor,INPUT);
  pinMode (rSensor,INPUT);
  
  //L298 driver pins
  pinMode (lMotNeg, OUTPUT);
  pinMode (lMotPos, OUTPUT);
  pinMode (lMotPWM, OUTPUT);

  pinMode (rMotNeg, OUTPUT);
  pinMode (rMotPos, OUTPUT);
  pinMode (rMotPWM, OUTPUT);
  
  analogWrite(lMotPWM, lMotSpeed);
  analogWrite(rMotPWM, rMotSpeed);
  Serial.begin(9600);

  delay(100);
}

void loop()
{  

  int lSensStat = digitalRead(lSensor);
  int rSensStat = digitalRead(rSensor);
  if (!lSensStat && !rSensStat) //both sensors detected white floor. go straight or check side
  {
    forward();
    Serial.println("forward");
  }
  if (!lSensStat && rSensStat) //detected black floor on right (off), so turn right 
  {
    turnRight();
  }

  if (lSensStat && !rSensStat) //detected black floor on left (off), so turn left
  {
    turnLeft();
  }

  if (lSensStat && rSensStat) //detected black floor on both sensor, so stop!
  {
    stop();
  }

}

void forward ()
{
  //set left motor control parameter
  digitalWrite (lMotNeg, LOW);
  digitalWrite (lMotPos, HIGH);
  analogWrite(lMotPWM, lMotSpeed);   
   
  //set right motor control parameters
  digitalWrite (rMotNeg, LOW);    
  digitalWrite (rMotPos, HIGH);
  analogWrite(rMotPWM, rMotSpeed);
}

void turnRight ()
{
  digitalWrite (lMotNeg, LOW);    //left motor forward
  digitalWrite (lMotPos, HIGH);
  analogWrite(lMotPWM, lMotSpeed);
  
  digitalWrite (rMotNeg, HIGH);    //right motor reverse
  digitalWrite (rMotPos, LOW);
  analogWrite(rMotPWM, rMotSpeed);
}

void turnLeft ()
{
  digitalWrite (lMotNeg, HIGH);  //left motor reverse
  digitalWrite (lMotPos, LOW);
  analogWrite(lMotPWM, lMotSpeed);
  
  digitalWrite (rMotNeg, LOW);    //right motor forward
  digitalWrite (rMotPos, HIGH);
  analogWrite(rMotPWM, rMotSpeed);
}

void stop() 
{
  digitalWrite (lMotNeg, LOW);
  digitalWrite (lMotPos, LOW);
  digitalWrite (rMotNeg, LOW);
  digitalWrite (rMotPos, LOW);
}
