	jp ENDINCLUDE_moveplayer
	
;==========================================
; Code that generally handles moving the player and collision detection
; David K
; June/July 2016
;==========================================
; I N C L U D E D   C O D E
;==========================================

;------------------------------------------
; D i r e c t i o n s
;------------------------------------------
PlayerGoDown:  ; Jumped
	ld de, 16
	call CheckCollision
	
	call UnShowPlayer
	xor a \ call ShowHideAllEnemies
	ld a, (playerY)
	inc a
	ld (playerY), a
	call MaybeScroll
	call UpdateAllEnemies
	call ShowPlayer
	ld a, 1 \ call ShowHideAllEnemies
	jp MainGameKeyLoop
	
PlayerGoUp:  ; Jumped
	ld de, -16
	call CheckCollision
	
	call UnShowPlayer
	xor a \ call ShowHideAllEnemies
	ld a, (playerY)
	dec a
	ld (playerY), a
	call MaybeScroll
	call UpdateAllEnemies
	call ShowPlayer
	ld a, 1 \ call ShowHideAllEnemies
	jp MainGameKeyLoop
	
PlayerGoLeft:  ; Jumped
	; Wrapping around?
	ld a, (playerX)
	or a
	jr z, PlayerWrapLeft

	ld de, -1
	call CheckCollision
	
	call UnShowPlayer
	xor a \ call ShowHideAllEnemies
	ld a, (playerX)
	dec a
	ld (playerX), a
	call UpdateAllEnemies
	call ShowPlayer
	ld a, 1 \ call ShowHideAllEnemies
	jp MainGameKeyLoop
	
PlayerGoRight:  ; Jumped
	; Wrapping around?
	ld a, (playerX)
	cp 15
	jr z, PlayerWrapRight
	
	ld de, 1
	call CheckCollision
	
	call UnShowPlayer
	xor a \ call ShowHideAllEnemies
	ld a, (playerX)
	inc a
	ld (playerX), a
	call UpdateAllEnemies
	call ShowPlayer
	ld a, 1 \ call ShowHideAllEnemies
	jp MainGameKeyLoop
;------------------------------------------
; W r a p p i n g
;------------------------------------------
PlayerWrapLeft:  ; Jumped
	; If you make a path on the side, then you have a hole on the other side as well; otherwise it's bad level design!
	call UnShowPlayer
	xor a \ call ShowHideAllEnemies
	ld a, 15
	ld (playerX), a
	call UpdateAllEnemies
	call ShowPlayer
	ld a, 1 \ call ShowHideAllEnemies
	
	jp MainGameKeyLoop
	
PlayerWrapRight:  ; Jumped
	; [2]
	call UnShowPlayer
	xor a \ call ShowHideAllEnemies
	xor a
	ld (playerX), a
	call UpdateAllEnemies
	call ShowPlayer
	ld a, 1 \ call ShowHideAllEnemies
	
	jp MainGameKeyLoop
;------------------------------------------
; C o l l i s i o n   d e t e c t i o n
;------------------------------------------
CheckCollision:  ; Called. Expects block to check in DE. Old: Will return a as 1 if block is solid.
	ld hl, (currentLevelPointer)

	; Add the offset of the direction...
	add hl, de
	
	; Now we still need to offset the player's actual position
	ld a, (playerY)
	call TilePointerHLy
	ld a, (playerX)
	call TilePointerHLx
	
	;DEBUG START
	;ld (hl), 99
	;ld hl, (currentLevelPointer)
	;call DisplayLevel
	;pop hl
	;jp MainGameKeyLoop
	;DEBUG END
	
	ld a, (hl)
	
	cp 32
	ret c  ; Air
	cp 64
	jr c, HandleTrigger
	cp 96
	jr c, SolidityReturnSolid
	jr z, HandleFinish
	
	; Fallthrough: whatever, solid
	
SolidityReturnSolid:  ; Jumped (but could be entry)
	pop hl  ; uncall
	jp MainGameKeyLoop
HandleFinish:  ; Jumped
	pop hl  ; Uncall
	jp StartNextLevel
;------------------------------------------
; T r i g g e r s
;------------------------------------------
HandleTrigger:  ; Jumped (during a call, to clarify)
	; What kind of trigger is it?
	cp 48
	jr c, HandleOpenerTrigger
	
	; So it's a warp trigger. But which one is it?
	sub 48
	rla  ; 2 bytes of data for each warp trigger
	
	ld hl, (currentLevelWarpPointer)
	
	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	
	; Now we're pointed to the address we need! Here's the warp destination
	ld a, (hl)
	ld (playerX), a
	inc hl
	ld a, (hl)
	ld (playerY), a
	
	; The scroll height might need to change...
	call ReScroll
	
	; Now redraw!
	ld hl, (currentLevelPointer)
	call DisplayLevel
	
	; Also show the player once more (has been unshown already by redrawing the level)
	call ShowPlayer
	
	; Hey, you're still setting a step
	call UpdateAllEnemies
	
	; And we don't want the enemies to disappear now...
	ld a, 1 \ call ShowHideAllEnemies
	
	; Handle the rest as solid; we don't suddenly want another step to be taken
	pop hl  ; Uncall
	jp MainGameKeyLoop
HandleOpenerTrigger: ; Jumped (during a call)
	; So which opener trigger is it?
	sub 32
	rla  ; Same story here, each opener has 2 bytes of data
	
	ld hl, (currentLevelOpenerPointer)
	
	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	
	; Now we're pointing to the address that contains the target tile number for the opener. But unfortunately it looks like ld hl, (hl) is too much to ask...
	ld e, (hl)
	inc hl
	ld d, (hl)
	
	ld hl, (currentLevelPointer)
	
	add hl, de
	
	; Replace that one!
	ld a, (hl)
	cp 32
	ret c  ; Do nothing if the tile is already solid!
	
	sub 59  ; The non-solid equivalent
	ld (hl), a
	
	; Now that that's over, redraw
	ld hl, (currentLevelPointer)
	call DisplayLevel
	
	; That's all
	ret
;------------------------------------------
; G e t   t i l e   o f f s e t s   f r o m   Y / X   v a l u e s
;------------------------------------------
TilePointerHLy:  ; Called
	ld d, 0 \ ld e, a  ; ld de, a
	push hl
	ex de, hl  ;ld h, d \ ld l, e  ; ld hl, de
	add hl, hl \ add hl, hl \ add hl, hl \ add hl, hl ; x16, would 16 bit shift have existed
	ex de, hl  ;ld d, h \ ld e, l  ; ld de, hl
	pop hl
	add hl, de
	ret
TilePointerHLx:  ; Called
	ld d, 0 \ ld e, a  ; ld de, a
	add hl, de
	ret
;------------------------------------------
; S c r e e n   s c r o l l i n g
;------------------------------------------
MaybeScroll:  ; Called, expects a to be the playerY
	push af  ; So that we still have it later
	dec a
	push af  ; 1st modulo number
	ld a, 6
	push af  ; 2nd modulo number
	call ByteMod
	or a  ; a is result
	jr z, ScrollDown
	
	; <Insert popping and right away pushing af here>
	ld a, 6
	push af  ; 2nd modulo number
	call ByteMod
	or a  ; a is result again
	jr z, ScrollUp
	
	ret  ; Scrolled neither down nor up, back we go!
	
ScrollDown:  ; Jumped
	; Importantly, we need another pop here
	pop af
	
	; Now we almost have the scroll value we want!
	sub 1
	
	ld (scrolled), a
	ld hl, (currentLevelPointer)
	call DisplayLevel
	ret
ScrollUp:  ; Jumped
	ld a, (playerY)
	sub 6
	ld (scrolled), a
	ld hl, (currentLevelPointer)
	call DisplayLevel
	ret
	
ReScroll:  ; Called, decides what the scroll value should be after warping, or when starting the level
	ld a, (playerY)
	dec a
	push af  ; So that we instantly have the right player Y -1 again...
	push af  ; 1st modulo number
	ld a, 6
	push af  ; 2nd modulo number
	call ByteMod
	ld b, a  ; Remainder
	
	pop af  ; ...over here!
	sub b
	
	; Voilà, scroll value
	ld (scrolled), a
	
	ret
	

;==========================================
; D A T A
;==========================================



;==========================================
; E N D
;==========================================

ENDINCLUDE_moveplayer: