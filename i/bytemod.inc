	jp ENDINCLUDE_bytemod
	
;==========================================
; Modulo calculations for single bytes.
; David K
; Februari 2016
;
; Example use:
;
;	ld a, 7     ; 7
;	push af     ; %
;	ld a, 3     ; 3
;	push af
;	call ByteMod
;	ld (result), a
;
;==========================================
; I N C L U D E D   C O D E
;==========================================
	
ByteMod:  ; Called, expects a%b: push a push b so to speak. Returns outcome as reg a, returns floor(a/b).
	pop de  ; Return value
	pop bc  ; B
	pop af  ; A
	push de  ; Put the return value back
	
	ld c, 0
	
ByteModLoop:  ; Jumped loop.
	cp b
	
	jr nc, ByteModNotDoneYet
	
	; Als we hier zijn, zijn we klaar! Register a bevat nu de uitkomst.
	ret
	
ByteModNotDoneYet:  ; Jumped, part of a loop.
	sub b
	inc c
	
	jr ByteModLoop

;==========================================
; D A T A
;==========================================



;==========================================
; E N D
;==========================================

ENDINCLUDE_bytemod: