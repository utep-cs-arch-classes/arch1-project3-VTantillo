	.file "_state_macine.c"

	.text
	.global stateMachine

	.extern playGame
	.extern drawControlsScreen
	.extern drawSplashScreen
	.extern state
	.extern switch1
	.extern switch2
	.extern switch3
	.extern switch4
	.extern seed
stateMachine:
	cmp #1, &state
	jz state1
	cmp #2, &state
	jz state2
	jmp default
state1:
	cmp #1, &switch1
	jz play
	cmp #1, &switch2
	jz decrement
	cmp #1, &switch3
	jz increment
	cmp #1, &switch4
	jz control
	jmp done
state2:
	cmp #1, &state
	jz control
	jmp done
default:
	jmp control
play:	mov #3, &state
	CALL #playGame
	RETI
increment:
	add #1, &seed
decrement:	
	sub #1, &seed
control:
	mov #1, &state
	CALL #drawSplashScreen
done:
	RETI
