.model flat, c

heaven segment execute

hg_enter macro
push 033h ; swap to long mode
call $+5
add dword ptr [esp], 5
retf
endm

hg_end macro
call $+5
db 0C7h, 44h, 24h, 04h, 23h, 0, 0, 0 ; mov dword [rsp+4], 0x23
db 83h, 04h, 24h, 0Dh, 0CBh ; add dword [rsp], 0xD. RETF
endm

check_x64_peb proc

hg_enter

db 065h, 067h, 048h, 0A1h, 060h, 00, 00, 00 ; mov rax, gs:[0x60]
db 048h, 00Fh, 0B6h, 040h, 002h ; movzx rax, [rax+2] (PEB64->IsBeingDebugged)

hg_end
ret

check_x64_peb endp

end