
#include <Servo.h>


const int SERVO_LR = 7;

/* Consigns */
int lr;

/* Servo motors */
Servo servo_lr;

/* Setup the PWMs output */
void setup_pwm_outputs()
{
  servo_lr.attach(SERVO_LR);
}

void setup()
{
  /* PWMs set up */
  setup_pwm_outputs();

  /* Init consigns */
  lr = 0;

  Serial.begin(9600);
}

void loop()
{

    lr = map(analogRead(A0), 0, 1023, 0, 180);
    Serial.println(lr);
    servo_lr.write(lr);

    /*
    lr++;
    if(lr >= 180)
    {
      lr = 0;
    }
    delay(100);
    */
}
