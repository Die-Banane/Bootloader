org 0x7C00
BITS 16

start:
    jmp main


clr_screen:
;save state
    push ax
    push bx
    push cx
    push dx

    mov ah, 0x06
    mov al, 0			;0 = blank the Window
    mov bh, 0x07
    mov ch, 0			;top left corner
    mov cl, 0
    mov dh, 24			;bottom right corner
    mov dl, 79
    int 0x10

;restore state
    pop dx
    pop cx
    pop bx
    pop ax
    ret


;prints a null terminated String to the cursor position
puts:
    pushf
    push si
    push ax
    push bx
    xor bh, bh

.loop:
    lodsb
    test al, al
    jz .done
    
    mov ah, 0x0e
    int 0x10

    jmp .loop

.done:
    pop bx
    pop ax
    pop si
    popf
    ret

;check if the a20 line is enabled, store result in ax. 1 - enabled, 0 - disabled
check_a20:
    pushf
    push es
    push ds
    push di
    push si
    
    cli

    xor ax, ax
    mov ds, ax			;ds:si = 0x0000:0x0500 = 0x500
    mov si, 0x500

    not ax
    mov es, ax			;es:di = 0xffff:0x0510 = 0x100500
    mov di, 0x510
;save previous values
    push [ds:si]
    push [es:di]

    mov ah, 1
    mov byte [ds:si], 1
    mov byte [es:di], 0
    mov al, [ds:si]
    cmp al, [es:di]
    jne .exit
    dec ah

.exit:

    sti

    shr ax, 8
    pop [es:di]
    pop [ds:si]
    pop si
    pop di
    pop ds
    pop es
    popf
    ret


main:
;initialisation
    xor ax, ax
    mov es, ax
    mov ds, ax

    mov ss, ax
    mov sp, 0x7C00
    
    call clr_screen

;put msg string to the screen
    mov si, a20_msg
    call puts

    call check_a20
    test ax, ax
    jnz .a20_done

    mov ax, 0x2401
    int 0x15
    jc .a20_fail

.a20_done:
    mov si, done_msg
    call puts
    jmp .halt

.a20_fail:
    mov si, fail_msg
    call puts

.halt:
    hlt
    jmp .halt

a20_msg: db "activating A20-Line...", 0x0d, 0x0a, 0
done_msg: db "done", 0x0d, 0x0a, 0
fail_msg: db "failed", 0xd, 0x0a, 0

times 510 - ($ - $$) db 0	;pad the file with 0
db 0x55				;place the boot signature at byte 510 and 511
db 0xAA
