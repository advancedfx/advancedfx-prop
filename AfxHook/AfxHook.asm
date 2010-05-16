CPU 586 ; PENTIUM like
BITS 32 ; 32 BIT mode

jmp labelBoot

align 32

labelArgs:
	argLoadLibrary: dd 0
	argSetDllDirectory: dd 0
	argDllDirectory: dd 0
	argDllName: dd 0
	
labelOfs:
	mov ebx, [esp]
	sub ebx, labelGotOfs -labelArgs
	ret
	
labelBoot:
	push ebx
	
	call labelOfs

labelGotOfs:

	mov eax, [ebx +argDllDirectory -labelArgs]
	push eax
	call [ebx +argSetDllDirectory -labelArgs]
	cmp eax, 0
	jz labelAbort
	
	mov eax, [ebx +argDllName -labelArgs]
	push eax
	call [ebx +argLoadLibrary -labelArgs]
	cmp eax, 0
	jz labelAbort
	
	mov eax, 0
	
	pop ebx
	ret 4

labelAbort:
	mov eax, 1
	
	pop ebx
	ret 4
