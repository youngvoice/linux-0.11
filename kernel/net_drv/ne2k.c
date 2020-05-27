//#include <asm/io.h>
#include <linux/kernel.h>
#include "io.h"
#include <asm/system.h>
#include "8390.h"

#define NE_BASE (ioaddr)
#define NE_CMD 0x00
#define NE_DATAPORT	0x10	/* NatSemi-defined port window offset. */

#define NE1SM_START_PG	0x20	/* First page of TX buffer */
#define NE1SM_STOP_PG 	0x40	/* Last page +1 of RX ring */
#define NESM_START_PG	0x40	/* First page of TX buffer */
#define NESM_STOP_PG	0x80	/* Last page +1 of RX ring */

//ne_probe()
/*
*/

//ne2k_init()
int ioaddr = 0xc020;
int word16;
unsigned char tx_start_page;
unsigned char stop_page;
unsigned char rx_start_page;
unsigned char current_page;
char *name;
int ne2k_init()
{

	
	int i;
	unsigned char SA_prom[32];
	int wordlength = 2;
	//char *name;
	int start_page;//stop_page;
	int neX000, ctron, dlink, dfi;
	int reg0 = inb(ioaddr);
	if (reg0 == 0xFF)
			return 0;

    /* Do a quick preliminary check that we have a 8390. */
    {	int regd;
	outb_p(E8390_NODMA+E8390_PAGE1+E8390_STOP, ioaddr + E8390_CMD);
	regd = inb_p(ioaddr + 0x0d);
	outb_p(0xff, ioaddr + 0x0d);
	outb_p(E8390_NODMA+E8390_PAGE0, ioaddr + E8390_CMD);
	inb_p(ioaddr + EN0_COUNTER0); /* Clear the counter by reading. */
	if (inb_p(ioaddr + EN0_COUNTER0) != 0) {
	    outb_p(reg0, ioaddr);
	    outb(regd, ioaddr + 0x0d);	/* Restore the old values. */
	    return 0;
	}
    }

    printk("NE*000 ethercard probe at %#3x:", ioaddr);

    /* Read the 16 bytes of station address prom, returning 1 for
       an eight-bit interface and 2 for a 16-bit interface.
       We must first initialize registers, similar to NS8390_init(eifdev, 0).
       We can't reliably read the SAPROM address without this.
       (I learned the hard way!). */
    {
	struct {unsigned char value, offset; } program_seq[] = {
	    {E8390_NODMA+E8390_PAGE0+E8390_STOP, E8390_CMD}, /* Select page 0*/
	    {0x48,	EN0_DCFG},	/* Set byte-wide (0x48) access. */
	    {0x00,	EN0_RCNTLO},	/* Clear the count regs. */
	    {0x00,	EN0_RCNTHI},
	    {0x00,	EN0_IMR},	/* Mask completion irq. */
	    {0xFF,	EN0_ISR},
	    {E8390_RXOFF, EN0_RXCR},	/* 0x20  Set to monitor */
	    {E8390_TXOFF, EN0_TXCR},	/* 0x02  and loopback mode. */
	    {32,	EN0_RCNTLO},
	    {0x00,	EN0_RCNTHI},
	    {0x00,	EN0_RSARLO},	/* DMA starting at 0x0000. */
	    {0x00,	EN0_RSARHI},
	    {E8390_RREAD+E8390_START, E8390_CMD},
	};
	for (i = 0; i < sizeof(program_seq)/sizeof(program_seq[0]); i++)
	    outb_p(program_seq[i].value, ioaddr + program_seq[i].offset);
    }
    for(i = 0; i < 32 /*sizeof(SA_prom)*/; i+=2) {
	SA_prom[i] = inb(ioaddr + NE_DATAPORT);
	SA_prom[i+1] = inb(ioaddr + NE_DATAPORT);
	if (SA_prom[i] != SA_prom[i+1])
	    wordlength = 1;
    }

    if (wordlength == 2) {
	/* We must set the 8390 for word mode. */
	outb_p(0x49, ioaddr + EN0_DCFG);
	/* We used to reset the ethercard here, but it doesn't seem
	   to be necessary. */
	/* Un-double the SA_prom values. */
	for (i = 0; i < 16; i++)
	    SA_prom[i] = SA_prom[i+i];
    }
	printk("wordlength %d\n", wordlength);

#define show_all_SAPROM
#if defined(show_all_SAPROM)
    /* If your ethercard isn't detected define this to see the SA_PROM. */
    for(i = 0; i < sizeof(SA_prom); i++)
	printk(" %2.2x", SA_prom[i]);
#else
    for(i = 0; i < ETHER_ADDR_LEN; i++) {
	//dev->dev_addr[i] = SA_prom[i];
	printk(" %2.2x", SA_prom[i]);
    }
#endif

    neX000 = (SA_prom[14] == 0x57  &&  SA_prom[15] == 0x57);
    ctron =  (SA_prom[0] == 0x00 && SA_prom[1] == 0x00 && SA_prom[2] == 0x1d);
    dlink =  (SA_prom[0] == 0x00 && SA_prom[1] == 0xDE && SA_prom[2] == 0x01);
    dfi   =  (SA_prom[0] == 'D' && SA_prom[1] == 'F' && SA_prom[2] == 'I');

    /* Set up the rest of the parameters. */
    if (neX000 || dlink || dfi) {
	if (wordlength == 2) {
	    name = dlink ? "DE200" : "NE2000";
	    start_page = NESM_START_PG;
	    stop_page = NESM_STOP_PG;
	} else {
	    name = dlink ? "DE100" : "NE1000";
	    start_page = NE1SM_START_PG;
	    stop_page = NE1SM_STOP_PG;
	}
    } else if (ctron) {
	name = "Cabletron";
	start_page = 0x01;
	stop_page = (wordlength == 2) ? 0x40 : 0x20;
    } else {
	printk(" not found.\n");
	return 0;
    }



	tx_start_page = start_page;
	//unsigned char stop_page = stop_page;
	word16 = (wordlength == 2);
	rx_start_page = start_page + TX_PAGES;



/*################### NS8390_init() #########################################*/
	
	
    int startp = 1;
    int e8390_base = ioaddr;
    //int i;
    int endcfg = word16 ? (0x48 | ENDCFG_WTS) : 0x48;
    
    /* Follow National Semi's recommendations for initing the DP83902. */
    outb_p(E8390_NODMA+E8390_PAGE0+E8390_STOP, e8390_base); /* 0x21 */
    outb_p(endcfg, e8390_base + EN0_DCFG);	/* 0x48 or 0x49 */
    /* Clear the remote byte count registers. */
    outb_p(0x00,  e8390_base + EN0_RCNTLO);
    outb_p(0x00,  e8390_base + EN0_RCNTHI);
    /* Set to monitor and loopback mode -- this is vital!. */
    outb_p(E8390_RXOFF, e8390_base + EN0_RXCR); /* 0x20 */
    outb_p(E8390_TXOFF, e8390_base + EN0_TXCR); /* 0x02 */
    /* Set the transmit page and receive ring. */
    outb_p(tx_start_page,	 e8390_base + EN0_TPSR);
    outb_p(rx_start_page,	 e8390_base + EN0_STARTPG);
    outb_p(stop_page-1, e8390_base + EN0_BOUNDARY); /* 3c503 says 0x3f,NS0x26*/
    current_page = rx_start_page;		/* assert boundary+1 */
    outb_p(stop_page,	  e8390_base + EN0_STOPPG);
    /* Clear the pending interrupts and mask. */
    outb_p(0xFF, e8390_base + EN0_ISR);
    outb_p(0x00,  e8390_base + EN0_IMR);
    
    /* Copy the station address into the DS8390 registers,
       and set the multicast hash bitmap to receive all multicasts. */
    cli();
    outb_p(E8390_NODMA + E8390_PAGE1 + E8390_STOP, e8390_base); /* 0x61 */
    for(i = 0; i < 6; i++) {
		outb_p(SA_prom[i], e8390_base + EN1_PHYS + i);
    }
    /* Initialize the multicast list to accept-all.  If we enable multicast
       the higher levels can do the filtering. */
    for(i = 0; i < 8; i++)
		outb_p(0xff, e8390_base + EN1_MULT + i);
    
    outb_p(rx_start_page,	 e8390_base + EN1_CURPAG);
    outb_p(E8390_NODMA+E8390_PAGE0+E8390_STOP, e8390_base);
    sti();
    if (startp) {
		outb_p(0xff,  e8390_base + EN0_ISR);
		outb_p(ENISR_ALL,  e8390_base + EN0_IMR);
		outb_p(E8390_NODMA+E8390_PAGE0+E8390_START, e8390_base);
		outb_p(E8390_TXCONFIG, e8390_base + EN0_TXCR); /* xmit on. */
		/* 3c503 TechMan says rxconfig only after the NIC is started. */
		outb_p(E8390_RXCONFIG,	e8390_base + EN0_RXCR); /* rx on,  */
    }
    //return;




	return 0;
}


/* ##############################################################*/
static void
ne_block_output(int count,
		const unsigned char *buf, const int start_page)
{
    int retries = 0;
    int nic_base = NE_BASE;

    /* Round the count up for word writes.  Do we need to do this?
       What effect will an odd byte count have on the 8390?
       I should check someday. */
    if (word16 && (count & 0x01))
      count++;
    /* We should already be in page 0, but to be safe... */
    outb_p(E8390_PAGE0+E8390_START+E8390_NODMA, nic_base + NE_CMD);

 retry:
#define rw_bugfix
#if defined(rw_bugfix)
    /* Handle the read-before-write bug the same way as the
       Crynwr packet driver -- the NatSemi method doesn't work.
       Actually this doesn't aways work either, but if you have
       problems with your NEx000 this is better than nothing! */
    outb_p(0x42, nic_base + EN0_RCNTLO);
    outb_p(0x00,   nic_base + EN0_RCNTHI);
    outb_p(0x42, nic_base + EN0_RSARLO);
    outb_p(0x00, nic_base + EN0_RSARHI);
    outb_p(E8390_RREAD+E8390_START, nic_base + NE_CMD);
    /* Make certain that the dummy read has occured. */
    SLOW_DOWN_IO;
    SLOW_DOWN_IO;
    SLOW_DOWN_IO;
#endif  /* rw_bugfix */

    /* Now the normal output. */
    outb_p(count & 0xff, nic_base + EN0_RCNTLO);
    outb_p(count >> 8,   nic_base + EN0_RCNTHI);
    outb_p(0x00, nic_base + EN0_RSARLO);
    outb_p(start_page, nic_base + EN0_RSARHI);

    outb_p(E8390_RWRITE+E8390_START, nic_base + NE_CMD);
    if (word16) {
	outsw(NE_BASE + NE_DATAPORT, buf, count>>1);
    } else {
	outsb(NE_BASE + NE_DATAPORT, buf, count);
    }

    /* This was for the ALPHA version only, but enough people have
       encountering problems that it is still here. */
	int ei_debug = 1; /*xjk*/
    if (ei_debug > 1) {		/* DMA termination address check... */
	int addr, tries = 20;
	do {
	    /* DON'T check for 'inb_p(EN0_ISR) & ENISR_RDC' here
	       -- it's broken! Check the "DMA" address instead. */
	    int high = inb_p(nic_base + EN0_RSARHI);
	    int low = inb_p(nic_base + EN0_RSARLO);
	    addr = (high << 8) + low;
	    if ((start_page << 8) + count == addr)
		break;
	} while (--tries > 0);
	if (tries <= 0) {
	    printk("%s: Tx packet transfer address mismatch,"
		   "%#4.4x (expected) vs. %#4.4x (actual).\n",
		   name, (start_page << 8) + count, addr);
	    if (retries++ == 0)
		goto retry;
	}
    }
    return;
}


/* Trigger a transmit start, assuming the length is valid. */
static void NS8390_trigger_send(unsigned int length,
								int start_page)
{
    int e8390_base = ioaddr;
    
    outb_p(E8390_NODMA+E8390_PAGE0, e8390_base);
    
    if (inb_p(e8390_base) & E8390_TRANS) {
		printk("%s: trigger_send() called with the transmitter busy.\n",
			   name);
		return;
    }
    outb_p(length & 0xff, e8390_base + EN0_TCNTLO);
    outb_p(length >> 8, e8390_base + EN0_TCNTHI);
    outb_p(start_page, e8390_base + EN0_TPSR);
    outb_p(E8390_NODMA+E8390_TRANS+E8390_START, e8390_base);
    return;
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
struct pbuf {
		etherhdr_t etherhdr;
		arphdr_t arphdr;
};

#define ETH_ZLEN 60
static int ei_start_xmit(struct pbuf *data)
{
    int e8390_base = ioaddr;
    int length, send_length;
    
    /* Fill in the ethernet header. */
	length = sizeof(struct pbuf);


    /* Mask interrupts from the ethercard. */
    outb(0x00,	e8390_base + EN0_IMR);

    send_length = ETH_ZLEN < length ? length : ETH_ZLEN;

    {  /* No pingpong, just a single Tx buffer. */
		ne_block_output(length, data, tx_start_page);
		NS8390_trigger_send(send_length, tx_start_page);
    }
    
    /* Turn 8390 interrupts back on. */
    outb_p(ENISR_ALL, e8390_base + EN0_IMR);

    return 0;
}

int test_transmit()
{
		struct pbuf arp_package;
		int i = 0;

		//ne2k_init();
		for (i = 0; i < ETHER_ADDR_LEN; i++)
				arp_package.etherhdr.dhost[i] = 0xFF;
		arp_package.etherhdr.shost[0] = 0xB0;
		arp_package.etherhdr.shost[1] = 0xC4;
		arp_package.etherhdr.shost[2] = 0x20;
		arp_package.etherhdr.shost[3] = 0x00;
		arp_package.etherhdr.shost[4] = 0x00;
		arp_package.etherhdr.shost[5] = 0x01;

		//arp_package.etherhdr.ethertype = 0x0806;
		arp_package.etherhdr.ethertype = 0x0608;

		arp_package.arphdr.htype = 0x0100;
		//arp_package.arphdr.ptype = 0x0800;
		arp_package.arphdr.ptype = 0x0008;
		arp_package.arphdr.hlen = 6;
		arp_package.arphdr.plen = 4;
		arp_package.arphdr.oper = 0x0100;
		arp_package.arphdr.sha[0] = 0xB0;
		arp_package.arphdr.sha[1] = 0xC4;
		arp_package.arphdr.sha[2] = 0x20;
		arp_package.arphdr.sha[3] = 0x00;
		arp_package.arphdr.sha[4] = 0x00;
		arp_package.arphdr.sha[5] = 0x01;
		
		arp_package.arphdr.spa[0] = 192;
		arp_package.arphdr.spa[1] = 168;
		arp_package.arphdr.spa[2] = 1;
		arp_package.arphdr.spa[3] = 12;

		for (i = 0; i < ETHER_ADDR_LEN; i++)
				arp_package.arphdr.tha[i] = 0xFF;
		arp_package.arphdr.tpa[0] = 192;
		arp_package.arphdr.tpa[1] = 168;
		arp_package.arphdr.tpa[2] = 1;
		arp_package.arphdr.tpa[3] = 11;

		ei_start_xmit(&arp_package);

		return 0;
}

