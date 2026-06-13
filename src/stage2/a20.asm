BITS 16

enable_a20:
    call check_a20
    cmp ax, 1
    je .done

    call enable_a20_bios
    call check_a20
    cmp ax, 1
    je .done

    call enable_a20_fast
    call check_a20
    cmp ax, 1
    je .done

    call enable_a20_kbc
    call check_a20
    cmp ax, 1
    je .done
   
.done:
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
    jne .done
    dec ah

.done:
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

;try enableing a20 with the BIOS
enable_a20_bios:
    mov ax, 0x2401
    int 0x15
    ret

enable_a20_kbc:
    cli
    push ax
    pushf

    call .wait_in_buf
    mov al, 0xAD
    out 0x64, al	;disable keyboard

    call .wait_in_buf
    mov al, 0xD0
    out 0x64, al

    call .wait_out_buf
    in al, 0x60
    push ax		;read the output port and save it

    call .wait_in_buf
    mov al, 0xD1
    out 0x64, al	;write next byte in the data port to the output port

    call .wait_in_buf
    pop ax
    or al, 2
    out 0x60, al	;set bit 1 (enable A20) in the output port
    jmp .done

;input buffer must be clear before sending commands to the controller
.wait_in_buf:
    in al, 0x64		
    test al, 2		;mask of bit 1, wich is set if the input buffer is occupied
    jnz .wait_in_buf
    ret

;output buffer must be clear before reading data from port 0x60
.wait_out_buf:
    in al, 0x64
    test al, 1		;mask of bit 0, wich is set if the output buffer is occupied
    jnz .wait_out_buf
    ret

.done:
    call .wait_in_buf
    mov al, 0xAE
    out 0x64, al

    call .wait_in_buf

    popf
    pop ax
    sti
    ret


enable_a20_fast:
    push ax
    in al, 0x92
    and al, 0xFE
    or al, 2
    out 0x92, al
    pop ax
    ret
