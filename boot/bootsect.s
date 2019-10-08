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
ROOT_DEV = 0x306

entry _start
_start:
! Print some inane message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#36
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
! add by xjk
! add process to es
! because process to es is done before print in origin source file
	mov ax,#0x07c0
	mov es,ax
	mov	ax,#0x1301		! write string, move cursor
	int	0x10
! ok, we've written the message, now

load_setup:
	mov dx,#0x0000
	mov cx,#0x0002
	mov bx,#0x0200
	mov ax,#0x0200+SETUPLEN
	int 0x13
	jnc ok_load_setup
	mov dx,#0x0000
	mov ax,#0x0000
	int 0x13
	jmp load_setup
ok_load_setup:
	jmpi 0,SETUPSEG

inf_loop:
	jmp inf_loop

msg1:
	.byte 13,10
	.ascii "Hello os world, my name is xjk"
	.byte 13,10,13,10
.org 508
root_dev:
	.word ROOT_DEV
boot_flag:
	.word 0xAA55

.text
endtext:
.data
enddata:
.bss
endbss:
