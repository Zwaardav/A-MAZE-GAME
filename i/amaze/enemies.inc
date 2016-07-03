	jp ENDINCLUDE_enemies
	
;==========================================
; Code that handles enemies
; David K
; June/July 2016
;==========================================
; I N C L U D E D   C O D E
;==========================================

;------------------------------------------
; E n e m y   d i s p l a y i n g ,   p l a y e r   c o l l i s i o n   c h e c k
;------------------------------------------
ShowHideAllEnemies: ; Called, expects A to indicate whether to show (1) or hide (0)
	ld (showNotHide), a
	
	xor a
	ld (playerWasHit), a
	
	; Anything at all?
	ld a, (currentLevelAmountEnemies)
	or a
	ret z
	
	ld hl, (currentLevelEnemyPointer)
	
	ld b, a
ShowAllEnemiesLoop:  ; Entry, jumped loop
	push bc
	
	ld a, (showNotHide)
	or a
	call z, UnShowThisEnemy
	call nz, ShowThisEnemy
	
	ld bc, 6
	add hl, bc
	
	pop bc
	djnz ShowAllEnemiesLoop
	
	; Does the player currently have any reason to lose a life?
	ld a, (playerWasHit)
	or a
	jr nz, PlayerLosesLife  ; Forget that ret, we'll take care of that
	
	; If not, we're done now
	ret

ShowThisEnemy:  ; Called, expects hl to be pointer to enemy
	ld a, (hl)
	ld (CURCOL), a
	ld a, (scrolled) ; We need to subtract the scroll value
	ld b, a
	inc hl
	ld a, (hl)
	dec hl
	call EnemyOnScreen
	ret nz
	
	sub b  ; scrolled from EnemyOnScreen
	ld (CURROW), a
	ld a, $18
	B_CALL(_PutMap)
	
	; If we're re-showing the enemies now, we also want to check for collision with the player! Both the enemies and the player have been updated already
	ld a, (hl)
	ld b, a
	ld a, (playerX)
	cp b
	ret nz
	inc hl
	ld a, (hl)
	dec hl
	ld b, a
	ld a, (playerY)
	cp b
	ret nz
	
	; Boo, you must've been really cornered! But alright, noted.
	ld a, 1
	ld (playerWasHit), a
	
	ret
	
UnShowThisEnemy:  ; Called, expects hl to be pointer to enemy
	ld a, (hl)
	ld (CURCOL), a
	ld a, (scrolled) ; We need to subtract the scroll value
	ld b, a
	inc hl
	ld a, (hl)
	dec hl
	call EnemyOnScreen
	ret nz
	
	sub b  ; scrolled from EnemyOnScreen
	ld (CURROW), a
	ld a, ' '
	B_CALL(_PutMap)
	
	; It was 0, make sure we don't have a problem with the comparison above
	ld a, 0
	or a
	
	ret
	
PlayerLosesLife:
	pop hl  ; Uncall
	
	call Delay
	call Delay
	
	ld a, (playerLives)
	dec a
	ld (playerLives), a
	
	ld hl, (currentLevelMasterPointer)
	jp LoadLevel
;------------------------------------------
; M o v e   e n e m i e s
;------------------------------------------
UpdateAllEnemies:  ; Called, make all enemies move one step.
	; Are there any enemies at all?
	ld a, (currentLevelAmountEnemies)
	or a
	ret z
	
	ld hl, (currentLevelEnemyPointer)
	
	ld b, a
UpdateAllEnemiesLoop:  ; Entry, jumped loop
	push bc
	
	inc hl \ inc hl
	ld a, (hl)  ; Direction ("-^v<>")
	dec hl
	cp 1
	call z, EnemyGoUp
	cp 2
	call z, EnemyGoDown
	dec hl
	cp 3
	call z, EnemyGoLeft
	cp 4
	call z, EnemyGoRight
	
	;ld ix, hl
	push hl
	pop ix
	push hl  ; Because CheckEnemyCollision destroys hl
	call CheckEnemyCollision
	pop hl
	
	ld bc, 6
	add hl, bc
	
	pop bc
	djnz UpdateAllEnemiesLoop
	
	; We're done now
	ret
	
EnemyGoUp:  ; Called, expects hl to be properly done, see above
	ld a, (hl)
	dec a
	ld (hl), a
	
	xor a
	ret
EnemyGoDown:  ; Called, see above also
	ld a, (hl)
	inc a
	ld (hl), a
	
	xor a
	ret
EnemyGoLeft:  ; And here also
	ld a, (hl)
	or a
	call z, EnemyWrapLeft
	ret z
	
	dec a
	ld (hl), a
	
	xor a
	ret
EnemyGoRight:  ; And here
	ld a, (hl)
	cp 15
	call z, EnemyWrapRight
	ret z
	
	inc a
	ld (hl), a
	
	xor a
	ret
	
EnemyWrapLeft:  ; Called, expects hl to point to the enemy (x)
	; If you make a path on the side, then you have a hole on the other side as well; otherwise it's bad level design!
	ld a, 15
	ld (hl), a
	
	ret
	
EnemyWrapRight:  ; Called, expects hl to point to the enemy (x)
	; [2]
	xor a
	ld (hl), a
	
	ret
;------------------------------------------
; S o m e   m o r e   d i s p l a y   s t u f f
;------------------------------------------
EnemyOnScreen:  ; Called, expects the enemy Y in A, compares
	ld e, a
	
	ld a, (scrolled)
	ld b, a
	
	ld a, e
	
	cp b  ; Compare enemyY x scrolled
	jr c, RetNZ  ; It's above the screen
	
	; So it's at at least the screen top edge... But is it too far?
	ld a, b
	add a, 8  ; one below the bottom edge
	ld b, a
	ld a, e
	
	cp b  ; Compare enemyY x scrolled+8
	jr nc, RetNZ
	
	; Ok, so it's on the screen!
	ld a, (scrolled)
	ld b, a  ; Let b be scrolled
	xor a
	or a  ; Produce Z
	ld a, e
	
	ret
	
RetNZ:  ; Jumped
	; It's not on the screen.
	ld a, (scrolled)
	ld b, a  ; Let b be scrolled
	ld a, 1
	or a  ; Produce NZ
	ld a, e
	
	ret
	
#ifdef blockComment

	EnemyOnScreen:  ; Called, expects the enemy Y in A, compares
		push af
		dec a
		push af  ; So that we instantly have the right Y -1 again...
		push af  ; 1st modulo number
		ld a, 6
		push af  ; 2nd modulo number
		call ByteMod
		ld b, a  ; Remainder
		
		pop af  ; ...over here!
		sub b
		
		; The scroll value is now a. Compare it to the scrolled value
		ld c, a
		ld a, (scrolled)
		ld b, a
		ld a, c
		
		cp b
		
		; And we also need that value back.
		pop de
		ld a, d
		
		ret

#endif
;------------------------------------------
; E n e m y   r e d i r e c t o r s
;------------------------------------------
CheckEnemyCollision:  ; Called. Expects ix to be a pointer to the enemy
	ld hl, (currentLevelPointer)
	
	; Get the position of the tile we want.
	ld a, (ix+1)
	call TilePointerHLy
	ld a, (ix)
	call TilePointerHLx
	
	;DEBUG START
	;ld (hl), 99
	;ld hl, (currentLevelPointer)
	;call DisplayLevel
	;pop hl
	;jp MainGameKeyLoop
	;DEBUG END
	
	ld a, (hl)
	
	cp 0
	ret z
	cp 5
	jr c, EnemyChangeDir
	
	ret
	
EnemyChangeDir:  ; Jumped (after call)
	ld (ix+2), a
	ret

;==========================================
; D A T A
;==========================================

showNotHide: .db 0
playerWasHit: .db 0

;==========================================
; E N D
;==========================================

ENDINCLUDE_enemies: