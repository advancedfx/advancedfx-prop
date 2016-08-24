; Remember: We cannot simply relay on SetDllDirectoryW to work,
; since this won't work well with UCRT DLL function forwarders.

CPU 586 ; PENTIUM like
BITS 32 ; 32 BIT mode

jmp labelBoot

align 32

labelArgs:
	argGetModuleHandleW: dd 0
	argGetProcAddress: dd 0
	argDllFilePath: dd 0
	datKernel32Dll: dw __utf16__('kernel32.dll'), 0
	datLoadLibraryExW: db 'LoadLibraryExW', 0

labelOfs:
	mov ebx, [esp]
	sub ebx, labelGotOfs -labelArgs
	ret
	
labelBoot:
	push ebx
	
	call labelOfs
labelGotOfs:

	push ebp
	mov ebp, esp
	
	sub esp, 0x8
	; stack variables (pointed by ebp):
	; -0x4 : hKernel32Dll
	; -0x8 : pLoadLibraryExW
	
	; get hKernel32Dll:
	mov eax, ebx
	add eax, datKernel32Dll -labelArgs
	push eax
	call [ebx +argGetModuleHandleW -labelArgs]
	cmp eax, 0
	jnz labelCont1
	mov eax, 1 ; error
	mov esp, ebp
	pop ebp
	pop ebx
	ret 4
labelCont1:
	mov [ebp -0x4], eax
	
	; get pLoadLibraryExW:
	mov eax, ebx
	add eax, datLoadLibraryExW -labelArgs
	push eax
	mov eax, [ebp -0x4]
	push eax
	call [ebx +argGetProcAddress -labelArgs]
	cmp eax, 0
	jnz labelCont2
	mov eax, 2 ; error
	mov esp, ebp
	pop ebp
	pop ebx
	ret 4
labelCont2:
	mov [ebp -0x8], eax
	
	; LoadLibraryExW:
	push 0x00000008 ; LOAD_WITH_ALTERED_SEARCH_PATH
	push 0
	mov eax, [ebx +argDllFilePath -labelArgs]
	push eax
	call [ebp -0x8]
	cmp eax, 0
	jnz labelCont3
	mov eax, 3 ; error
	mov esp, ebp
	pop ebp
	pop ebx
	ret 4
labelCont3:
	
	; We are done:
	mov eax, 0 ; success
	mov esp, ebp
	pop ebp
	pop ebx
	ret 4
