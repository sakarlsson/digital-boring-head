#include "defines.h"

WiFiWebServer server(80);


int status = WL_IDLE_STATUS;             // the Wi-Fi radio's status

unsigned long previousMillisInfo = 0;     //will store last time Wi-Fi information was updated
unsigned long previousMillisLED = 0;      // will store the last time LED was updated
const int intervalInfo = 5000;            // interval at which to update the board information


#define SLEEP_PIN 3
#define MOTOR_PLUS 4
#define MOTOR_MINUS 5
#define CLK 7
#define DT 6


int currentStateCLK;
int lastStateCLK;
String currentDir = "";

void setup() {
  Serial.begin(9600);
	setupWebserver();

  pinMode(LED_BUILTIN, OUTPUT);
	pinMode(MOTOR_PLUS, OUTPUT);
	pinMode(MOTOR_MINUS, OUTPUT);
	pinMode(SLEEP_PIN, OUTPUT);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);

  lastStateCLK = digitalRead(CLK);

  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(MOTOR_PLUS, LOW);
  digitalWrite(MOTOR_MINUS, LOW);

}

#define DIR_IN true							/* Move to a larger radius */
#define DIR_OUT false

// tokenize(s) split s into global array cmd 
// return number of tokens
#define MAX_ARGSZ 15
#define MAX_ARGS 4
char cmd[MAX_ARGS][MAX_ARGSZ+1];
int tokenize(char *s) {
	int i = 0;
	char *p;
	if(strlen(s) == 0) {
		return 0;
	}
	strncpy(cmd[i], strtok(s, " \t"), MAX_ARGSZ);
	cmd[i][MAX_ARGSZ] = '\0';
	i++;
	while ((p = strtok(NULL, " \t")) != NULL && i < MAX_ARGS) {
		strncpy(cmd[i], p, MAX_ARGSZ);
		cmd[i][MAX_ARGSZ] = '\0';
		i++;
	}
	return i;
}
// get_command() reads from serial until a line is complete
// do not block
// returns tokenize(line) or 0 if not yet a full line 
int get_command(){
	static char line[MAX_ARGS * (MAX_ARGSZ + 1) + 1];
	static int lineix = 0;
	char buf[8];
	if (Serial.available())  {
		char c = Serial.read(); 
		sprintf(buf, "X %02x", c);
		/* Serial.println(buf);			 */
		if (c == '\r' || c == '\n') {  
      Serial.read(); //gets rid of the following \r or \n
      line[lineix] = '\0';
      lineix = 0;
      return tokenize(line);
		} else {     
      line[lineix] = c; 
      lineix++;
		}
	}
	return 0;
}

bool dir = DIR_OUT;
bool running = false;
bool stopping = false;
int speed = 255;
unsigned long tstop;
long pos_current = 0;
long pos_goto = 0;


void stop() {
	pinMode(MOTOR_MINUS, OUTPUT);
	digitalWrite(MOTOR_MINUS, dir);
	digitalWrite(MOTOR_PLUS, dir);
	stopping = true;
	tstop = millis();
	running = false;
}

void run() {
	digitalWrite(MOTOR_MINUS, dir);
	digitalWrite(MOTOR_PLUS, !dir);
	stopping = false;
	running = true;
}

void goto_pos(long pos) {
	pos_goto = pos;
	if ( pos_goto < pos_current ) {
		dir = DIR_IN;
	}	else {
		dir = DIR_OUT;
	}
	run();
}

#define DECCELRAMP 100
#define SLOW 64									/* pwm */
#define FAST 192

void run_control() {
	if ( ! running ) {
		return;
	}
	int steps_to_go = abs(pos_goto - pos_current);
	if (steps_to_go < DECCELRAMP) {
		// Use linear interpolation for acceleration/deceleration
		speed = SLOW + (FAST - SLOW) * steps_to_go / DECCELRAMP;
		if (!dir) {
			speed = 256 - speed; 			/* invert pwm curve */
		}
		analogWrite(MOTOR_MINUS, speed);
	} else {
		pinMode(MOTOR_MINUS, OUTPUT);
		digitalWrite(MOTOR_MINUS, dir);
	}

	if (running && dir == DIR_OUT && pos_current >= pos_goto) {
		stop();
	}
	if (running && dir == DIR_IN && pos_current <= pos_goto) {
		stop();
	}
}


void printpos(long pos, long pos_goto) {
	char buf[128];
	sprintf(buf, "Count: %5d, %5d, (%d) Pos: %.3f mm", 
					pos, 
					pos_goto, 
					pos-pos_goto, pos*0.001647); /* 0.001583 e nominell */
	Serial.println(buf);
}

void loop() {
	unsigned long tnow;

	stop();
	
	while(true) {
		int i = get_command();
		if (i > 0) {
      if (strcmp(cmd[0], "pos") == 0) {
				printpos(pos_current, pos_goto);
      } else if (strcmp(cmd[0], "zero") == 0) {
				pos_current = 0;
				pos_goto = 0;
      } else if (strcmp(cmd[0], "goto") == 0) {
				goto_pos(atof(cmd[1]) * 631.7719);
			}
		}

		currentStateCLK = digitalRead(CLK);
		if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
			// If the DT state is different than the CLK state then
			// the encoder is rotating CCW so decrement
			if (digitalRead(DT) != currentStateCLK) {
				pos_current--;
				currentDir = "CCW";
			} else {
				// Encoder is rotating CW so increment
				pos_current++;
				currentDir = "CW";
			}
		}
		lastStateCLK = currentStateCLK;

		if (running) {
			run_control();
		}

		tnow = millis();

		if ( stopping && tnow - tstop >= 200 ) {
			stopping = false;
			printpos(pos_current, pos_goto);
		}
		server.handleClient();
	}
}

