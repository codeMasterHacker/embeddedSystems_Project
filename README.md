# embeddedSystems_Project
My embedded systems project consists of two Atmega1284 micro controllers communicating with each other via USART.

Each micro controller has a 16 key keypad connected to its PORTA.

Each micro controller has an LCD connected to its PORTC and PORTD. The LCD's data bus is connected to PORTC and the LCD's control bus (the Enable line and Register Select line) is connected to PORTD, specifically pins D7 and D6.

The two microcontrollers are connected to each other by their respective PORTD pins, specifically pins D0 and D1, where their USART0 is found.

The function of my project is to allow two users to send and receive alphanumeric strings to each other. Using his/her respective keypad, a user is able to type any alphanumeric string with punctuation. The string is displayed on his/her respective LCD, as the user types. When ready, the user presses the A key on the keypad to send the string to the other user. The other user receives the string, types his/her response, and sends it. The process continues as long as the the users wish.

The digit keys print their respective digits.
The A key is to send a string.
The B key is the "newline character".
The C key is to clear the screen.
The D key is to delete a character.
Pressing and holding a digit key, except for the 0 key, will cycle through a list of 3 alphabet letters per digit key.
Pressing and holding the 1 key will cycle through the letters: A, B, C
Pressing and holding the 2 key will cycle through the letters: D, E, F
Pressing and holding the 3 key will cycle through the letters: G, H, I
Pressing and holding the 4 key will cycle through the letters: J, K, L
Pressing and holding the 5 key will cycle through the letters: M, N, ~N
Pressing and holding the 6 key will cycle through the letters: O, P, Q
Pressing and holding the 7 key will cycle through the letters: R, S, T
Pressing and holding the 8 key will cycle through the letters: U, V, W
Pressing and holding the 9 key will cycle through the letters: X, Y, Z
Pressing and holding the * key will cycle through the characters: '.' , '!' , '?' , ','
Pressing and holding the # key will cycle through the characters: ' ' , '\'' , '&' , '$'
