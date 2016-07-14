	jp ENDINCLUDE_codeinput
	
;==========================================
; Continue code input
; David K
; June/July 2016
;==========================================
; I N C L U D E D   C O D E
;==========================================

;------------------------------------------
; P a s s   i n p u t   d i s p l a y
;------------------------------------------
DisplayCodeInput:  ; Called
	ld a, 4
	ld (CURCOL), a
	;ld a, 4
	ld (CURROW), a
	
	ld hl, charInput
	
	ld b, 7
DisplayInputFieldLoop:
	push bc
	
	ld a, b
	cp 5
	call z, AdvanceCursor
	
	pop bc
	ld a, (hl)
	or a
	call z, BlankInputChar
	
	B_CALL(_PutC)
	
	inc hl
	
	djnz DisplayInputFieldLoop
	
	ret
	
AdvanceCursor:  ; Called. Simply increments CURCOL, no wrapping or anything fancy like that.
	ld a, (CURCOL)
	inc a
	ld (CURCOL), a
	ret
	
BlankInputChar:  ; Called
	ld a, 7
	sub b
	ld c, a
	ld a, (inputLen)
	cp c
	jr z, CurrentBlankInputChar
	
	; Not the current character, so just a blank space
	ld a, '_'
	ret
CurrentBlankInputChar:  ; Jumped
	cp 2
	jr c, CurrentBlankNumberInputChar
	
	; Not a number, so a letter
	ld a, $E2
	ret
CurrentBlankNumberInputChar:  ; Jumped
	; A number
	ld a, $E0
	ret
;------------------------------------------
; H a n d l e   p a s s w o r d   i n p u t
;------------------------------------------
PasswordInputScreen:  ; Jumped
	; :D
	ld hl, 0
	ld (inputLen), hl
	ld (inputLen+2), hl
	ld (inputLen+4), hl
	ld (inputLen+6), hl
	
PasswordInputScreenLoop:  ; Entry, jumped loop
#ifdef cutThisOut
	jr PasswordInputScreenLoop ;DEBUG
	
	push hl  ;DEBUG DEBUG DEBUG
	; DEBUG
	ld hl, $0000
	ld (PasswordInputScreenLoop), hl ; DEBUG NOP OUT
	
	pop hl
	
#endif
	
	call DisplayCodeInput
	
	; Do we have a complete password filled in?
	ld a, (inputLen)
	cp 7
	jp z, CheckPassword  ; Holy shit we do
	
	B_CALL(_GetKey)
	ld b, a
	
	ld a, (inputLen)
	cp 2
	jr c, HandlePassInputNum
	jr HandlePassInputLet
	
HandlePassInputNum:  ; Jumped
	call HandlePassInputGeneral
	
	cp $8E  ; 0
	jr c, PasswordInputScreenLoop  ; Under 0, get out of here
	cp $98  ; 9+1
	jr nc, PasswordInputScreenLoop  ; Above 9, what are you doing here
	
	; Excellent, a number.
	sub $8E  ; 0 => $00
	
	ld hl, txtNumbers
	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	ld a, (hl)
	jr AddCharToPassInput
	
HandlePassInputLet:  ; Jumped
	call HandlePassInputGeneral
	
	cp $9A  ; A
	jr c, PasswordInputScreenLoop  ; We only want
	cp $B4  ; Z+1
	jr nc, PasswordInputScreenLoop  ; capital letters here
	
	; Excellent, a capital letter.
	sub $9A  ; A => $00
	
	ld hl, txtLetters
	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	ld a, (hl)
	jr AddCharToPassInput
HandlePassInputGeneral:  ; Called
	ld a, b
	
	cp kClear
	jr z, PassBackspace
	
	ret
PassBackspace:  ; Jumped
	pop hl  ; Uncall
	
	ld hl, charInput
	ld a, (inputLen)
	sub 1  ; Ik heb die carry toch wel hard nodig
	jp c, StartProg  ; The input is already empty, jump back to the main menu instead

	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	
	ld (inputLen), a
	call HandleAlphaLock
	
	xor a
	ld (hl), a  ; Set character to 0x00
	
	jp PasswordInputScreenLoop
	
AddCharToPassInput:  ; Jumped, expects A to be the typed charaacter
	push af
	ld hl, charInput
	ld a, (inputLen)
	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	
	inc a
	ld (inputLen), a
	call HandleAlphaLock
	
	pop af
	ld (hl), a  ; Set character to typed character

	jp PasswordInputScreenLoop
HandleAlphaLock:  ; Called
	cp 2
	jr nc, SetAlphaLock
ResAlphaLock:  ; Entry, but can also be called
	res ShiftAlpha, (IY+ShiftFlags)
	res ShiftALock, (IY+ShiftFlags)
	res ShiftKeepAlph, (IY+ShiftFlags)
	ret
SetAlphaLock:  ; Jumped
	set ShiftAlpha, (IY+ShiftFlags)  ; The latter two apparently aren't enough
	set ShiftALock, (IY+ShiftFlags)
	set ShiftKeepAlph, (IY+ShiftFlags)
	ret
;------------------------------------------
; P a s s w o r d   v a l i d a t i o n
;------------------------------------------
CheckPassword:  ; Jumped
	call ResAlphaLock
	res 0, (IY+Asm_Flag1)  ; Character mismatch in current password
	res 1, (IY+Asm_Flag1)  ; 0: Level ptr,  1: Jump
	ld hl, continueCodes
CheckPasswordLoop:  ; Entry, jumped loop
	ld a, (hl)
	or a
	jr z, PasswordGotNull
	
	call PassStrCmp
	
	; Not matching with the currently checked password?
	jr nz, CheckPasswordLoop
	
	; Match!
	dec hl \ dec hl
	push hl
	
	xor a
	ld (CURCOL), a
	ld a, 7
	ld (CURROW), a
	ld hl, txtPasswordCorrect
	B_CALL(_PutS)
	
	call Delay
	
	pop ix
	ld l, (ix) \ ld h, (ix+1)
	
	bit 1, (IY+Asm_Flag1)
	res 1, (IY+Asm_Flag1)
	jr nz, PasswordCorrectJump
	
	;.echo "[[["
	;.echo $
	;.echo "]]]"
	
	jp LoadLevel
	
PasswordCorrectJump:  ; Jumped
	push hl
	ret
	
PassStrCmp:  ; Called. Expects HL, and gives a pointer to the next password back.
	ld de, charInput
	
	ld a, 10
	ld b, 7
PassStrCmpLoop:  ; Entry, jumped loop
	push bc
	
	ld c, a
	
	ld a, (hl)
	ld b, a
	ld a, (de)
	add a, c
	
	cp b
	jr z, PassSkipSet
	
	set 0, (IY+Asm_Flag1)
PassSkipSet:  ; Entry, jumped
	
	inc hl
	inc de
	
	inc c
	ld a, c
	
	pop bc
	djnz PassStrCmpLoop
	
	; Done. Go past the pointer
	inc hl \ inc hl
	
	; So, does any of the characters mismatch?
	bit 0, (IY+Asm_Flag1)
	res 0, (IY+Asm_Flag1)
	
	ret
PasswordGotNull:  ; Jumped
	bit 1, (IY+Asm_Flag1)
	jr nz, PasswordNoMatches  ; We've already been checking both level passwords and cheats, so it's just a case of "no matches"
	
	; Switching to mode 2!
	set 1, (IY+Asm_Flag1)
	inc hl  ; We obviously need to step beyond that null
	jr CheckPasswordLoop
PasswordNoMatches:  ; Jumped
	res 1, (IY+Asm_Flag1)

	xor a
	ld (CURCOL), a
	ld a, 7
	ld (CURROW), a
	ld hl, txtPasswordIncorrect
	B_CALL(_PutS)
	
	call Delay
	
	B_CALL(_GetKey)
	
	jp StartProg
;------------------------------------------
; P a s s    o n   l e v e l   s c r e e n
;------------------------------------------
DisplayContinueCode:  ; Called
	call FindLevelContinueCode
	
	; We didn't uncall+ret, so a pointer to the code is assumed to be in DE
	ld hl, $2F00  ; 9 pixels lager dan de naam, niet 7
	ld (PENCOL), hl
	ld hl, txtContinuePassword
	B_CALL(_VPutS)
	
	; Now the password itself!
	ex de, hl
	ld c, 10
	ld b, 7
DisplayContinueCodeLoop:  ; Entry, jumped loop
	ld a, b
	cp 5
	call z, PutMapSpace
	ld a, (hl)
	sub c
	B_CALL(_VPutMap)
	inc c
	inc hl
	djnz DisplayContinueCodeLoop
	
	ret
PutMapSpace:  ; Called
	ld a, ' '
	B_CALL(_VPutMap)
	ret
FindLevelContinueCode:  ; Called
	ld hl, continueCodes-2
	ld de, 7
FindLevelContinueCodeLoop:  ; Entry, jumped loop
	inc hl \ inc hl  ; First check whether this is null or not
	ld a, (hl)
	or a
	jr z, LevelContinueCode404
	
	; It's not null, go ahead!
	add hl, de  ; add hl, 7
	; At the first pointer now!
	
	call CheckLevelPointerEqual
	
	jr nz, FindLevelContinueCodeLoop
	
	; Oh, we found a password?! Great!
	ld de, -7
	add hl, de
	ex de, hl
	
	ret
	
CheckLevelPointerEqual:  ; Called. Checks whether the 2 byte value at (hl) equals (currentLevelMasterPointer). "Returns" z/nz
	ld a, (hl)
	ld b, a
	ld a, (currentLevelMasterPointer)
	cp b
	ret nz  ; And use this nz as "return value" immediately!
	
	inc hl
	ld a, (hl)
	dec hl
	ld b, a
	ld a, (currentLevelMasterPointer+1)
	cp b
	
	; The flags tell everything
	ret
	
LevelContinueCode404:  ; Jumped
	; Just uncall and ret
	pop hl
	ret
;------------------------------------------
; C h e a t s
;------------------------------------------
CheatPlusLife:  ; Jumped
	ld a, (playerLives)
	inc a
	ld (playerLives), a
	
	ld hl, txtPlusLife
	B_CALL(_PutS)
	B_CALL(_GetKey)
	jr StartProg

;==========================================
; D A T A
;==========================================

inputLen: .db 0
charInput: .db 0,0,0,0,0,0,0

txtNumbers: .db "0123456789"
txtLetters: .db "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

txtPlusLife: .db " +1 life!",0

;==========================================
; C O N T I N U E   P A S S W O R D S
;==========================================
; Don't cheat yourself
;==========================================

continueCodes:
	.db $3A,$40,$4D,$4F,$51,$53,$55
		.dw level5Data
#ifdef devLevels
	.db $3A,$41,$50,$52,$50,$64,$57
		.dw level6Data
	.db $43,$44,$51,$5B,$53,$5C,$69
		.dw level94Data
#endif
	.db 0
	.db $3A,$3C,$64,$59,$57,$55,$55
		.dw CheatPlusLife
	.db 0

;==========================================
; E N D
;==========================================

ENDINCLUDE_codeinput: