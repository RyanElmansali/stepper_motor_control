// constants 
#define enablePin 4
#define stepPin 7
#define directionPin 8
// machine states
#define DISABLED 0
#define STEP_CW 1
#define STEP_CCW 2
#define MACH_IDLE 9
int flicks;
int total_Flicks;
int i;
float flick_Time;
float total_Time = 0.0;
float a;
float vel_Initial;
float ms_Flick;
// comm states
#define COMM_IDLE 0
#define COMM_STATE 1
// comm settings
#define END_TOKEN '\n'
#define COMM_SIZE (size_t)1
#define VALID_CMD "V\n"
#define BUSY_CMD "B\n"
#define INVALID_CMD "I\n"
#define LENGTH 20


// setting default state
byte mach_state = DISABLED;
byte comm_state = COMM_IDLE;

void setup() {
  // put your setup code here, to run once:
  // config each pin as an output 
  pinMode(enablePin, OUTPUT); 
  pinMode(stepPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  // set a well defined voltage on all digital pins
  digitalWrite(enablePin, HIGH);
  digitalWrite(stepPin, HIGH);
  digitalWrite(directionPin, HIGH);
  // initailize the serial port
  Serial.begin(115200); 
  Serial.setTimeout(500);

  return;
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (mach_state) {
    case MACH_IDLE:
      // machine state use to wait for command in an active state
      delay(1);
      
      break;
    case DISABLED: 
      // logic is high, so all outputs are disabled. Step motor wont move
      digitalWrite(enablePin, HIGH);
      mach_state = MACH_IDLE;
      comm_state = COMM_IDLE;

      break;
    case STEP_CW:
      // enable outputs 
      digitalWrite(enablePin, LOW);
      // set direction to CW
      digitalWrite(directionPin, HIGH);
      // loop through until desired step count is reached
      for (flicks = 1; flicks <= total_Flicks; ++flicks) {
        flick_Time = 1/(a*total_Time + vel_Initial);
        total_Time += flick_Time;
        ms_Flick = flick_Time*1000;
        //need to transition stepPin from LOW to HIGH to move one step (transitions from HIGH to LOW do nothing)
        digitalWrite(stepPin, LOW);
        //short delay
        delay(ms_Flick/2);
        digitalWrite(stepPin, HIGH);
        //short delay
        delay(ms_Flick/2);
        
     }
      //return both states to idle
      mach_state = MACH_IDLE;
      comm_state = COMM_IDLE;

      break;
    case STEP_CCW:
      // enable outputs 
      digitalWrite(enablePin, LOW);
      // set direction to CCW
      digitalWrite(directionPin, LOW);
      // loop through until desired step count is reached
      for (flicks = 1; flicks <= total_Flicks; ++flicks) {
        flick_Time = 1/(a*total_Time + vel_Initial);
        total_Time += flick_Time;
        ms_Flick = flick_Time*1000;
        //need to transition stepPin from LOW to HIGH to move one step (transitions from HIGH to LOW do nothing)
        digitalWrite(stepPin, LOW);
        //short delay
        delay(ms_Flick/2);
        digitalWrite(stepPin, HIGH);
        //short delay
        delay(ms_Flick/2);
        
     }
      //return both states to idle
      mach_state = MACH_IDLE;
      comm_state = COMM_IDLE;
  }

  return;
}

// serial comm
void serialEvent() {
  // define array to store incoming serial data
  static char comm_buffer[LENGTH];
  // define inc serial data
  static size_t data;

  //recieve the inc data from serial port 
  //read inc data until end token or determined length has been read or timeout
  data = Serial.readBytesUntil(END_TOKEN, comm_buffer, LENGTH);
  //if (data != COMM_SIZE) {
    
    //return; //exit as it either timed out we recieved end token or command size was not matching length expected 
    
  //}

  // idle comm state where we are waiting for command to disable, 1 step CW or 1 step CCW
  if ((mach_state == MACH_IDLE) && (comm_state == COMM_IDLE)) {
    // we take commands "D\n", "H\n", "L\n"
    switch (comm_buffer[0]) { //looking only at first element
      case 'D':
        mach_state = DISABLED;
        comm_state = COMM_STATE; // no longer in idle, machine state unchanged
        if (Serial.availableForWrite() >= LENGTH) { // making sure buffer has at least two characters available to write without blocking
          Serial.write(VALID_CMD, LENGTH); // writing to serial moniter that command recieved is valid
        }
        
        return; // exit on disable output command
      case 'H':
        // remove H from buffer 
        //Serial.write(comm_buffer, LENGTH);
        for (i = 1; i < LENGTH; i++) {
          comm_buffer[i-1] = comm_buffer[i];  
        }
        Serial.write(comm_buffer, LENGTH);
        // Extract the first token (step number)
        char * step_buffer = strtok(comm_buffer, ",");
        // Extract second (time between on/off)
        char * speed_buffer = strtok(NULL, ",");
        char * inc_buffer = strtok(NULL, ",");
        //convert both from string to int through ascii
        total_Flicks = atoi(step_buffer);
        vel_Initial = atof(speed_buffer);
        a = atof(inc_buffer);
        mach_state = STEP_CW;
        comm_state = COMM_STATE;
        if (Serial.availableForWrite() >= LENGTH) {
          Serial.write(VALID_CMD, LENGTH);
        }
        
        return; // exit on CW command
      case 'L':
        // remove L from buffer 
        //Serial.write(comm_buffer, LENGTH);
        for (i = 1; i < LENGTH; i++) {
          comm_buffer[i-1] = comm_buffer[i];  
        }
        Serial.write(comm_buffer, LENGTH);
        // Extract the first token (step number)
        char * CCW_buffer = strtok(comm_buffer, ",");
        // Extract second (time between on/off)
        char * CCWS_buffer = strtok(NULL, ",");
        char * CCWI_buffer = strtok(NULL, ",");
        //convert both from string to int through ascii
        total_Flicks = atoi(CCW_buffer);
        vel_Initial = atof(CCWS_buffer);
        a = atof(CCWI_buffer);
        mach_state = STEP_CCW;
        comm_state = COMM_STATE;
        if (Serial.availableForWrite() >= LENGTH) {
          Serial.write(VALID_CMD, LENGTH);
        }
        
        return; // exit on CCW command
    
      default:
        // if we reached this location in the code is because we have been unable to decode
        if (Serial.availableForWrite() >= LENGTH) {
          Serial.write(INVALID_CMD, LENGTH);
        }
        
        break;
    
    
    }

    return;
  }
  
  
  
  // handle case where we recieve command but comm state is not in comm idle (so were busy)
  if (comm_state != COMM_IDLE) {
    switch (comm_buffer[0]) {
      case 'D':
      case 'H':
      case 'L':
        // the only valid commands we recieve, reply with busy 
        if (Serial.availableForWrite() >= LENGTH) {
          Serial.write(BUSY_CMD, LENGTH);
        }
        
      default:
      // if none of the cases are satified we send back invalid command 
        if (Serial.availableForWrite() >= LENGTH) {
        Serial.write(INVALID_CMD, LENGTH);
        }
        
      break;
  
    }
    
  }
  
  return;
}
