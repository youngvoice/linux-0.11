# 1 "io.h"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "io.h"
# 79 "io.h"

#define SLOW_IO_BY_JUMPING
#ifdef SLOW_IO_BY_JUMPING
#define __SLOW_DOWN_IO __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:")
#else
#define __SLOW_DOWN_IO __asm__ __volatile__("outb %al,$0x80")
#endif

#ifdef REALLY_SLOW_IO
#define SLOW_DOWN_IO { __SLOW_DOWN_IO; __SLOW_DOWN_IO; __SLOW_DOWN_IO; __SLOW_DOWN_IO; }
#else
#define SLOW_DOWN_IO __SLOW_DOWN_IO
#endif



extern inline unsigned int __inb(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "b" " %" "w" "1,%" "b" "0" : "=a" (_v) : "d" (port) ,"0" (0) ); return _v; } extern inline unsigned int __inbc(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "b" " %" "" "1,%" "b" "0" : "=a" (_v) : "i" (port) ,"0" (0) ); return _v; } extern inline unsigned int __inb_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "b" " %" "w" "1,%" "b" "0" : "=a" (_v) : "d" (port) ,"0" (0) ); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); return _v; } extern inline unsigned int __inbc_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "b" " %" "" "1,%" "b" "0" : "=a" (_v) : "i" (port) ,"0" (0) ); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); return _v; }
extern inline unsigned int __inw(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "w" " %" "w" "1,%" "w" "0" : "=a" (_v) : "d" (port) ,"0" (0) ); return _v; } extern inline unsigned int __inwc(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "w" " %" "" "1,%" "w" "0" : "=a" (_v) : "i" (port) ,"0" (0) ); return _v; } extern inline unsigned int __inw_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "w" " %" "w" "1,%" "w" "0" : "=a" (_v) : "d" (port) ,"0" (0) ); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); return _v; } extern inline unsigned int __inwc_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "w" " %" "" "1,%" "w" "0" : "=a" (_v) : "i" (port) ,"0" (0) ); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); return _v; }
extern inline unsigned int __inl(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0" : "=a" (_v) : "d" (port) ); return _v; } extern inline unsigned int __inlc(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "l" " %" "" "1,%" "" "0" : "=a" (_v) : "i" (port) ); return _v; } extern inline unsigned int __inl_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0" : "=a" (_v) : "d" (port) ); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); return _v; } extern inline unsigned int __inlc_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "l" " %" "" "1,%" "" "0" : "=a" (_v) : "i" (port) ); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); return _v; }

extern inline void __outb(unsigned char value, unsigned short port) { __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1" : : "a" (value), "d" (port)); } extern inline void __outbc(unsigned char value, unsigned short port) { __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "" "1" : : "a" (value), "i" (port)); } extern inline void __outb_p(unsigned char value, unsigned short port) { __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1" : : "a" (value), "d" (port)); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); } extern inline void __outbc_p(unsigned char value, unsigned short port) { __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "" "1" : : "a" (value), "i" (port)); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); }
extern inline void __outw(unsigned short value, unsigned short port) { __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "w" "1" : : "a" (value), "d" (port)); } extern inline void __outwc(unsigned short value, unsigned short port) { __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "" "1" : : "a" (value), "i" (port)); } extern inline void __outw_p(unsigned short value, unsigned short port) { __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "w" "1" : : "a" (value), "d" (port)); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); } extern inline void __outwc_p(unsigned short value, unsigned short port) { __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "" "1" : : "a" (value), "i" (port)); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); }
extern inline void __outl(unsigned int value, unsigned short port) { __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1" : : "a" (value), "d" (port)); } extern inline void __outlc(unsigned int value, unsigned short port) { __asm__ __volatile__ ("out" "l" " %" "0,%" "" "1" : : "a" (value), "i" (port)); } extern inline void __outl_p(unsigned int value, unsigned short port) { __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1" : : "a" (value), "d" (port)); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); } extern inline void __outlc_p(unsigned int value, unsigned short port) { __asm__ __volatile__ ("out" "l" " %" "0,%" "" "1" : : "a" (value), "i" (port)); __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:"); }

extern inline void insb(unsigned short port, void * addr, unsigned long count) { __asm__ __volatile__ ("cld ; rep ; ins" "b" : "=D" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }
extern inline void insw(unsigned short port, void * addr, unsigned long count) { __asm__ __volatile__ ("cld ; rep ; ins" "w" : "=D" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }
extern inline void insl(unsigned short port, void * addr, unsigned long count) { __asm__ __volatile__ ("cld ; rep ; ins" "l" : "=D" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }

extern inline void outsb(unsigned short port, const void * addr, unsigned long count) { __asm__ __volatile__ ("cld ; rep ; outs" "b" : "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }
extern inline void outsw(unsigned short port, const void * addr, unsigned long count) { __asm__ __volatile__ ("cld ; rep ; outs" "w" : "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }
extern inline void outsl(unsigned short port, const void * addr, unsigned long count) { __asm__ __volatile__ ("cld ; rep ; outs" "l" : "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }

/*
 * Note that due to the way __builtin_constant_p() works, you
 *  - can't use it inside a inline function (it will never be true)
 *  - you don't have to worry about side effects within the __builtin..
 */

#define outb(val,port) \
		__outb((val),(port))
#define inb(port) \
		__inb(port)
#define outb_p(val,port) \
		__outb_p((val),(port))
#define inb_p(port) \
		__inb_p(port)
