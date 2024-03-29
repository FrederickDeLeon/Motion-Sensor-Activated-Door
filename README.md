The objective of this project is to create an embedded finite state machine system using C++ and 2 Arduino boards in tandem that cycles through a series of door states depending on a motion being detected by the PIR sensor or not. 
These 4 states are 0:doors closed, 1:doors opening, 2:doors open, and 3:doors closing.
The starting state is 0:doors closed, the event that triggers the transition to state 1:doors opening is when the PIR sensor detects something. 
When the current state is 1:doors opening, the event that triggers the transition to state 2:doors open is when the pre-specified door open limit is reached.
When the current state is 2:doors open, if the PIR sensor detects any motion in less than 5 seconds after the current state is 2:doors open, the system will remain in state 2. 
The event that triggers the transition to state 3:doors closing is when the PIR sensor doesn't detect any motion for 5 seconds or more. 
When the current state is 3:doors closing, the state can either transition back to state 1:doors opening if the PIR sensor detects any motion, or it can transition to state 0:doors closed once the pre-specified door close limit is reached.
The door is opened and closed by a motor whose encoder is simulated by an Arduino board. 
The current state of the door is displayed on the 16x2 LCD, along with the value of the encoder, and whether or not motion is being detected by the PIR sensor (denoted as Input = 0 or 1 on the LCD). 
