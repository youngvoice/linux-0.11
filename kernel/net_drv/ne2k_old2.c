#include <asm/io.h>
#define NE_DATAPORT 0x10
#define NE_IOBASE 0xc020
#define NE_IRQ 11
struct macaddr {
		unsigned char bytes[6];
};

struct ne {
		unsigned short iobase;
		unsigned short irq;
		unsigned short membase;
		unsigned short memsize;
		unsigned char rx_page_start;
		unsigned char rx_page_stop;
		unsigned char next_packet; /* the next unread data_packet */
		struct macaddr paddr;
};


struct ne ne2k;
#define NE_PAGE_SIZE 256
#define NE_TXBUF_SIZE 4
#define NE_TX_BUFERS 2


#define NE_P0_CR 0x00 /*p0 p1 p2 share this offset*/
#define NE_CR_STP 0x01
#define NE_CR_STA 0x02
#define NE_CR_RD2 0x20
#define NE_CR_PAGE0 0x00 /*add by xjk*/
#define NE_P0_PSTART 0x01
#define NE_P0_PSTOP 0x02
#define NE_P0_BNRY 0x03
#define NE_CR_PAGE1 0x40
#define NE_P1_CURR 0x07


#define NE_P1_PAR0 0x01
#define NE_P1_MAR0 0x08
#define NE_P0_RCR 0x0C
#define NE_RCR_AB 0x04


#define NE_P0_TCR 0x0D
#define NE_P0_DCR 0x0E
#define NE_TCR_LB_MODE3 0x06
#define NE_TCR_LB_MODE2 0x04
#define NE_TCR_LB_MODE1 0x02
#define NE_TCR_LB_MODE0 0x00
#define NE_DCR_WTS 0x01
#define NE_DCR_BOS 0x02
#define NE_DCR_LS 0x00

#define NE_P0_ISR 0x07
#define NE_ISR_CLEAR 0xFF
#define NE_P0_IMR 0x0F


#define NE_P0_RBCR0 0x0A
#define NE_P0_RBCR1 0x0B

void ne2k_init()
{
		int i = 0;
		ne2k.iobase = NE_IOBASE;
		ne2k.irq = NE_IRQ;
		ne2k.membase = 16*1024;
		ne2k.memsize = 16*1024;
		ne2k.rx_page_start = ne2k.membase/NE_PAGE_SIZE;
		ne2k.rx_page_stop = ne2k.rx_page_start + ne2k.memsize/NE_PAGE_SIZE - NE_TXBUF_SIZE*NE_TX_BUFERS;
		ne2k.next_packet = ne2k.rx_page_start + 1; /* BNDY+1*/
		for (i = 0; i < 12; i++)
		{
				if (i%2 == 0) {
						__asm__ volatile ("inb %%dx,%%al":"=a" (ne2k.paddr.bytes[i/2]):"d" (NE_IOBASE + NE_DATAPORT));
						printk("paddr %x ", ne2k.paddr.bytes[i/2]);
				}
				else {
						__asm__ volatile ("inb %%dx,%%al"::"d" (NE_IOBASE + NE_DATAPORT));
				}
		}

		outb(NE_CR_PAGE0|NE_CR_RD2|NE_CR_STP, ne2k.iobase+NE_P0_CR);

		outb(NE_DCR_LS|NE_DCR_WTS&(~NE_DCR_BOS), ne2k.iobase + NE_P0_DCR);
		outb(0, ne2k.iobase + NE_P0_RBCR0);
		outb(0, ne2k.iobase + NE_P0_RBCR1);
		outb(NE_RCR_AB, ne2k.iobase + NE_P0_RCR);
		//outb(NE_TCR_LB_MODE3, ne2k.iobase + NE_P0_TCR);
		outb(ne2k.rx_page_start, ne2k.iobase + NE_P0_PSTART);
		outb(ne2k.rx_page_stop, ne2k.iobase + NE_P0_PSTOP);
		outb(ne2k.rx_page_start, ne2k.iobase + NE_P0_BNRY);
		outb(NE_ISR_CLEAR, ne2k.iobase + NE_P0_ISR);
		//NE_P0_IMR 0x0F




		outb(NE_CR_PAGE1|NE_CR_RD2|NE_CR_STP, ne2k.iobase + NE_P0_CR);
		outb(ne2k.next_packet, ne2k.iobase + NE_P1_CURR);
		/* BUFFER RAM ready to receive or transmit*/



		for (i = 0; i < 6; i++)
				outb(ne2k.paddr.bytes[i], ne2k.iobase + NE_P1_PAR0 + i);
		for (i = 0; i < 8; i++)
				outb(0, ne2k.iobase + NE_P1_MAR0 + i);

		outb(NE_CR_PAGE0|NE_CR_RD2|NE_CR_STA, ne2k.iobase + NE_P0_CR);
		outb(NE_TCR_LB_MODE0, ne2k.iobase + NE_P0_TCR);


}
#define ETHER_ADDR_LEN 6
#define LOGIC_ADDR_LEN 4


/*################################ ARP ###########################################*/
typedef struct arphdr {
		unsigned short htype;
		unsigned short ptype;
		unsigned char hlen;
		unsigned char plen;
		unsigned short oper;
		unsigned char sha[ETHER_ADDR_LEN];
		unsigned char spa[LOGIC_ADDR_LEN];
		unsigned char tha[ETHER_ADDR_LEN];
		unsigned char tpa[LOGIC_ADDR_LEN];
}arphdr_t;

typedef struct etherhdr {
		unsigned char dhost[ETHER_ADDR_LEN];
		unsigned char shost[ETHER_ADDR_LEN];
		unsigned short ethertype;
}etherhdr_t;


#define NE_P0_RSAR0 0x08
#define NE_P0_RSAR1 0x09
#define NE_P0_RBCR0 0x0A
#define NE_P0_RBCR1 0x0B
#define NE_P0_ISR 0x07
#define NE_ISR_RDC 0x40
#define NE_CR_RD1 0x10


#define NE_P0_TPSR 0x04
#define NE_P0_TBCR0 0x05
#define NE_P0_TBCR1 0x06
#define NE_CR_TXP 0x04
#define NE_P0_TSR 0x04
#define NE_CR_RD0 0x08

struct pbuf {
		unsigned char pad[46];
		etherhdr_t etherhdr;
		//arphdr_t arphdr;

};
int ne2k_transmit(struct pbuf *p)
{
		//unsigned short packetlen = 48;
		unsigned short packetlen = sizeof(struct pbuf);
		unsigned char dst = ne2k.rx_page_stop;
		unsigned short value;
		int i = 0;
		unsigned char status;

		printk("packetlen %d\n", packetlen);

		//outb(NE_CR_PAGE0 | NE_CR_RD2 | NE_CR_STA, ne2k.iobase + NE_P0_CR);

		//dummy read
		outb((unsigned char) 1, ne2k.iobase + NE_P0_RBCR0);
		outb(NE_CR_PAGE0 | NE_CR_RD0 | NE_CR_STA, ne2k.iobase + NE_P0_CR);

		outb((unsigned char) packetlen, ne2k.iobase + NE_P0_RBCR0);
		outb((unsigned char) (packetlen >> 8), ne2k.iobase + NE_P0_RBCR1);

		outb((unsigned char)(dst * NE_PAGE_SIZE), ne2k.iobase + NE_P0_RSAR0);
		outb((unsigned char)((dst * NE_PAGE_SIZE) >> 8), ne2k.iobase + ne2k.iobase + NE_P0_RSAR1);
		
		outb(NE_CR_PAGE0 | NE_CR_RD1 | NE_CR_STA, ne2k.iobase + NE_P0_CR);
		//for (int i; i < 48/2; i++) outw(xx, ne2k.iobase + NE_DATAPORT);
		for (i = 0; i < packetlen/2; i++) {
				value = *((unsigned short *)p + i);
				__asm__("outw %%ax,%%dx"::"a" (value), "d" (ne2k.iobase + NE_DATAPORT));
		}

		while ((status = inb(ne2k.iobase + NE_P0_ISR)& 0x40) == 0);
		printk("to buffer ram successful\n");

		/*##################################################################*/

		/*
		outp(ne2k.iobase + NE_P0_TPSR, (unsigned char)dst);
		if (packetlen > 60) {
				outp(ne2k.iobase + NE_P0_TBCR0, packetlen);
				outp(ne2k.iobase + NE_P0_TBCR1, packetlen >> 8);
		}
		else {
				outp(ne2k.iobase + NE_P0_TBCR0, 60);
				outp(ne2k.iobase + NE_P0_TBCR1, 0);
		}
		outp(ne2k.iobase + NE_P0_CR, NE_CR_RD2 | NE_CR_TXP | NE_CR_STA);
		*/
		outb_p((unsigned char)dst,ne2k.iobase + NE_P0_TPSR);
		if (packetlen > 60) {
				outb_p(packetlen,ne2k.iobase + NE_P0_TBCR0);
				outb_p(packetlen >> 8,ne2k.iobase + NE_P0_TBCR1);
		}
		else {
				outb_p( 60, ne2k.iobase + NE_P0_TBCR0);
				outb_p( 0, ne2k.iobase + NE_P0_TBCR1);
		}
		outb_p( NE_CR_RD2 | NE_CR_TXP | NE_CR_STA,ne2k.iobase + NE_P0_CR);
		while ((status = inb(ne2k.iobase + NE_P0_TSR)) == 0);
		printk("transmit status %x\n",status); 



}



int test_transmit()
{
		struct pbuf arp_package;
		int i = 0;

		//ne2k_init();
		for (i = 0; i < ETHER_ADDR_LEN; i++)
				arp_package.etherhdr.dhost[i] = 0xFF;
		arp_package.etherhdr.shost[5] = 0xB0;
		arp_package.etherhdr.shost[4] = 0xC4;
		arp_package.etherhdr.shost[3] = 0x20;
		arp_package.etherhdr.shost[2] = 0x00;
		arp_package.etherhdr.shost[1] = 0x00;
		arp_package.etherhdr.shost[0] = 0x01;

		arp_package.etherhdr.ethertype = 0x0806;

		/*
		arp_package.arphdr.htype = 1;
		arp_package.arphdr.ptype = 0x0800;
		arp_package.arphdr.hlen = 6;
		arp_package.arphdr.plen = 4;
		arp_package.arphdr.oper = 1;
		arp_package.arphdr.sha[5] = 0xB0;
		arp_package.arphdr.sha[4] = 0xC4;
		arp_package.arphdr.sha[3] = 0x20;
		arp_package.arphdr.sha[2] = 0x00;
		arp_package.arphdr.sha[1] = 0x00;
		arp_package.arphdr.sha[0] = 0x01;
		
		arp_package.arphdr.spa[3] = 192;
		arp_package.arphdr.spa[2] = 168;
		arp_package.arphdr.spa[1] = 1;
		arp_package.arphdr.spa[0] = 12;

		for (i = 0; i < ETHER_ADDR_LEN; i++)
				arp_package.arphdr.tha[i] = 0xFF;
		arp_package.arphdr.tpa[3] = 192;
		arp_package.arphdr.tpa[2] = 168;
		arp_package.arphdr.tpa[1] = 1;
		arp_package.arphdr.tpa[0] = 11;
		*/

		ne2k_transmit(&arp_package);

		return 0;
}

