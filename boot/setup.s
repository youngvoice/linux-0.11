!
! SYS_SIZE is the number of clicks (16 bytes) to be loaded.
! 0x3000 is 0x30000 bytes = 196kB, more than enough for current
! versions of linux
!
SYSSIZE = 0x3000
!
!	bootsect.s		(C) 1991 Linus Torvalds
!
! bootsect.s is loaded at 0x7c00 by the bios-startup routines, and moves
! iself out of the way to address 0x90000, and jumps there.
!
! It then loads 'setup' directly after itself (0x90200), and the system
! at 0x10000, using BIOS interrupts. 
!
! NOTE! currently system is at most 8*65536 bytes long. This should be no
! problem, even in the future. I want to keep it simple. This 512 kB
! kernel size should be enough, especially as this doesn't contain the
! buffer cache as in minix
!
! The loader has been made as simple as possible, and continuos
! read errors will result in a unbreakable loop. Reboot by hand. It
! loads pretty fast by getting whole sectors at a time whenever possible.

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

SETUPLEN = 2				! nr of setup-sectors
BOOTSEG  = 0x07c0			! original address of boot-sector
INITSEG  = 0x9000			! we move boot here - out of the way
SETUPSEG = 0x07e0			! setup starts here
SYSSEG   = 0x1000			! system loaded at 0x10000 (65536).
ENDSEG   = SYSSEG + SYSSIZE		! where to stop loading

! ROOT_DEV:	0x000 - same type of floppy as boot.
!		0x301 - first partition on first drive etc

entry start
start:
! Print some inane message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#36
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg2
! add by xjk
! add process to es
! because process to es is done before print in origin source file
	mov ax,cs
	mov es,ax
	mov	ax,#0x1301		! write string, move cursor
	int	0x10


	mov ax,cs
	mov es,ax
! init ss:sp
	mov ax,#INITSEG
	mov ss,ax
	mov sp,0xff00
! get params
	mov ax,#INITSEG
	mov ds,ax
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov [0],dx
	mov ah,#0x88
	int 0x15
	mov [2],ax
	mov ax,#0x0000
	mov ds,ax
	lds si,[4*0x41]
	mov ax,#INITSEG
	mov es,ax
	mov di,#0x0004
	mov cx,#0x10
	rep
	movsb
! be ready to print
	mov ax,cs
	mov es,ax
	mov ax,#INITSEG
	mov ds,ax
! Cursor Position
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#18
	mov bx,#0x0007
	mov bp,#msg_cursor
	mov ax,#0x1301
	int 0x10
	mov dx,[0]
	call print_hex
! Memory Size
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#14
	mov bx,#0x0007
	mov bp,#msg_memory
	mov ax,#0x1301
	int 0x10
	mov dx,[2]
	call print_hex

inf_loop:
	jmp inf_loop


print_hex:
	mov cx,#4
!???????????????????
!	mov dx,(bp)
print_digit:
	rol dx,#4
	mov ax,#0xe0f
	and al,dl
	add al,#0x30
	cmp al,#0x3a
	jl outp
	add al,#0x07
outp:
	int 0x10
	loop print_digit
	ret
print_nl:
! CR
	mov ax,#0xe0d
	int 0x10
! LF
	mov al,#0xa
	int 0x10
	ret

msg_cursor:
	.byte 13,10
	.ascii "Cursor position:"

msg_memory:
	.byte 13,10
	.ascii "Memory Size:"
msg2:
	.byte 13,10
	.ascii "Now we are in SETUP"
	.byte 13,10,13,10
.text
endtext:
.data
enddata:
.bss
endbss:

