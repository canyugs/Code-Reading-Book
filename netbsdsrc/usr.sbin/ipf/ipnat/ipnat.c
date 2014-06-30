/*	$NetBSD: ipnat.c,v 1.9.2.2 1997/11/17 16:27:10 mrg Exp $	*/

/*
 * Copyright (C) 1993-1997 by Darren Reed.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and due credit is given
 * to the original author and the contributors.
 *
 * Added redirect stuff and a variety of bug fixes. (mcn@EnGarde.com)
 *
 * Broken still:
 * Displaying the nat with redirect entries is way confusing
 *
 * Example redirection line:
 * rdr le1 0.0.0.0/0 port 79 -> 199.165.219.129 port 9901
 * 
 * Will redirect all incoming packets on le1 to any machine, port 79 to
 * host 199.165.219.129, port 9901
 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#if !defined(__SVR4) && !defined(__svr4__)
#include <strings.h>
#else
#include <sys/byteorder.h>
#endif
#include <sys/time.h>
#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#if defined(sun) && (defined(__svr4__) || defined(__SVR4))
# include <sys/ioccom.h>
# include <sys/sysmacros.h>
#endif
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <ctype.h>
#include "netinet/ip_compat.h"
#include "netinet/ip_fil.h"
#include "netinet/ip_proxy.h"
#include "netinet/ip_nat.h"
#include "kmem.h"

#if !defined(lint)
static const char sccsid[] ="@(#)ipnat.c	1.9 6/5/96 (C) 1993 Darren Reed";
static const char rcsid[] = "@(#)Id: ipnat.c,v 2.0.2.21.2.1 1997/11/08 04:55:55 darrenr Exp ";
#endif


#if	SOLARIS
#define	bzero(a,b)	memset(a,0,b)
#endif

extern	char	*optarg;

ipnat_t	*parse __P((char *));
u_long	hostnum __P((char *, int *));
u_long	hostmask __P((char *));
u_short	portnum __P((char *, char *));
void	dostats __P((int, int)), flushtable __P((int, int));
void	printnat __P((ipnat_t *, int, void *));
void	parsefile __P((int, char *, int));
void	usage __P((char *));
int	countbits __P((u_long));
char	*getnattype __P((ipnat_t *));
int	main __P((int, char*[]));

#define	OPT_REM		1
#define	OPT_NODO	2
#define	OPT_STAT	4
#define	OPT_LIST	8
#define	OPT_VERBOSE	16
#define	OPT_FLUSH	32
#define	OPT_CLEAR	64


void usage(name)
char *name;
{
	fprintf(stderr, "%s: [-CFlnrsv] [-f filename]\n", name);
	exit(1);
}


int main(argc, argv)
int argc;
char *argv[];
{
	char	*file = NULL;
	int	fd = -1, opts = 1, c;

	while ((c = getopt(argc, argv, "CFf:lnrsv")) != -1)
		switch (c)
		{
		case 'C' :
			opts |= OPT_CLEAR;
			break;
		case 'f' :
			file = optarg;
			break;
		case 'F' :
			opts |= OPT_FLUSH;
			break;
		case 'l' :
			opts |= OPT_LIST;
			break;
		case 'n' :
			opts |= OPT_NODO;
			break;
		case 'r' :
			opts &= ~OPT_REM;
			break;
		case 's' :
			opts |= OPT_STAT;
			break;
		case 'v' :
			opts |= OPT_VERBOSE;
			break;
		default :
			usage(argv[0]);
		}

	if (!(opts & OPT_NODO) && ((fd = open(IPL_NAT, O_RDWR)) == -1) &&
	    ((fd = open(IPL_NAT, O_RDONLY)) == -1)) {
		perror("open");
		exit(-1);
	}

	if (opts & (OPT_FLUSH|OPT_CLEAR))
		flushtable(fd, opts);
	if (file)
		parsefile(fd, file, opts);
	if (opts & (OPT_LIST|OPT_STAT))
		dostats(fd, opts);
	return 0;
}


/*
 * count consecutive 1's in bit mask.  If the mask generated by counting
 * consecutive 1's is different to that passed, return -1, else return #
 * of bits.
 */
int	countbits(ip)
u_long	ip;
{
	u_long	ipn;
	int	cnt = 0, i, j;

	ip = ipn = ntohl(ip);
	for (i = 32; i; i--, ipn *= 2)
		if (ipn & 0x80000000)
			cnt++;
		else
			break;
	ipn = 0;
	for (i = 32, j = cnt; i; i--, j--) {
		ipn *= 2;
		if (j > 0)
			ipn++;
	}
	if (ipn == ip)
		return cnt;
	return -1;
}


void printnat(np, verbose, ptr)
ipnat_t *np;
int verbose;
void *ptr;
{
	int	bits;
	struct	protoent	*pr;

	switch (np->in_redir)
	{
	case NAT_REDIRECT :
		printf("rdr ");
		break;
	case NAT_MAP :
		printf("map ");
		break;
	case NAT_BIMAP :
		printf("bimap ");
		break;
	default :
		fprintf(stderr, "unknown value for in_redir: %#x\n",
			np->in_redir);
		break;
	}

	if (np->in_redir == NAT_REDIRECT) {
		printf("%s %s", np->in_ifname, inet_ntoa(np->in_out[0]));
		bits = countbits(np->in_out[1].s_addr);
		if (bits != -1)
			printf("/%d ", bits);
		else
			printf("/%s ", inet_ntoa(np->in_out[1]));
		if (np->in_pmin)
			printf("port %d ", ntohs(np->in_pmin));
		printf("-> %s", inet_ntoa(np->in_in[0]));
		if (np->in_pnext)
			printf(" port %d", ntohs(np->in_pnext));
		if ((np->in_flags & IPN_TCPUDP) == IPN_TCPUDP)
			printf(" tcp/udp");
		else if ((np->in_flags & IPN_TCP) == IPN_TCP)
			printf(" tcp");
		else if ((np->in_flags & IPN_UDP) == IPN_UDP)
			printf(" udp");
		printf("\n");
		if (verbose)
			printf("\t%p %u %x %u %p %d\n", np->in_ifp,
			       np->in_space, np->in_flags, np->in_pnext, np,
			       np->in_use);
	} else {
		np->in_nextip.s_addr = htonl(np->in_nextip.s_addr);
		printf("%s %s/", np->in_ifname, inet_ntoa(np->in_in[0]));
		bits = countbits(np->in_in[1].s_addr);
		if (bits != -1)
			printf("%d ", bits);
		else
			printf("%s", inet_ntoa(np->in_in[1]));
		printf(" -> %s/", inet_ntoa(np->in_out[0]));
		bits = countbits(ntohl(np->in_out[1].s_addr));
		if (bits != -1)
			printf("%d ", bits);
		else
			printf("%s", inet_ntoa(np->in_out[1]));
		if (*np->in_plabel) {
			printf(" proxy port");
			if (np->in_dport)
				printf(" %hu", ntohs(np->in_dport));
			printf(" %.*s/", (int)sizeof(np->in_plabel),
				np->in_plabel);
			if ((pr = getprotobynumber(np->in_p)))
				fputs(pr->p_name, stdout);
			else
				printf("%d", np->in_p);
		} else if (np->in_pmin || np->in_pmax) {
			printf(" portmap");
			if ((np->in_flags & IPN_TCPUDP) == IPN_TCPUDP)
				printf(" tcp/udp");
			else if (np->in_flags & IPN_TCP)
				printf(" tcp");
			else if (np->in_flags & IPN_UDP)
				printf(" udp");
			printf(" %d:%d", ntohs(np->in_pmin),
			       ntohs(np->in_pmax));
		}
		printf("\n");
		if (verbose)
			printf("\t%p %u %s %d %x\n", np->in_ifp,
			       np->in_space, inet_ntoa(np->in_nextip),
			       np->in_pnext, np->in_flags);
	}
}


/*
 * Get a nat filter type given its kernel address.
 */
char *getnattype(ipnat)
ipnat_t *ipnat;
{
	char *which;
	ipnat_t ipnatbuff;

	if (ipnat && kmemcpy((char *)&ipnatbuff, (long)ipnat,
			     sizeof(ipnatbuff)))
		return "???";

	switch (ipnatbuff.in_redir)
	{
	case NAT_MAP :
		which = "MAP";
		break;
	case NAT_REDIRECT :
		which = "RDR";
		break;
	case NAT_BIMAP :
		which = "BIMAP";
		break;
	default :
		which = "unknown";
		break;
	}
	return which;
}


void dostats(fd, opts)
int fd, opts;
{
	natstat_t ns;
	ipnat_t	ipn;
	nat_t	**nt[2], *np, nat;
	int	i = 0;

	bzero((char *)&ns, sizeof(ns));

	if (!(opts & OPT_NODO) && ioctl(fd, SIOCGNATS, &ns) == -1) {
		perror("ioctl(SIOCGNATS)");
		return;
	}

	if (opts & OPT_STAT) {
		printf("mapped\tin\t%lu\tout\t%lu\n",
			ns.ns_mapped[0], ns.ns_mapped[1]);
		printf("added\t%lu\texpired\t%lu\n",
			ns.ns_added, ns.ns_expire);
		printf("inuse\t%lu\nrules\t%lu\n", ns.ns_inuse, ns.ns_rules);
		if (opts & OPT_VERBOSE)
			printf("table %p list %p\n", ns.ns_table, ns.ns_list);
	}
	if (opts & OPT_LIST) {
		printf("List of active MAP/Redirect filters:\n");
		while (ns.ns_list) {
			if (kmemcpy((char *)&ipn, (long)ns.ns_list,
				    sizeof(ipn))) {
				perror("kmemcpy");
				break;
			}
			printnat(&ipn, opts & OPT_VERBOSE, (void *)ns.ns_list);
			ns.ns_list = ipn.in_next;
		}

		nt[0] = (nat_t **)malloc(sizeof(*nt) * NAT_SIZE);
		if (kmemcpy((char *)nt[0], (long)ns.ns_table[0],
			    sizeof(**nt) * NAT_SIZE)) {
			perror("kmemcpy");
			return;
		}

		printf("\nList of active sessions:\n");

		for (i = 0; i < NAT_SIZE; i++)
			for (np = nt[0][i]; np; np = nat.nat_hnext[0]) {
				if (kmemcpy((char *)&nat, (long)np,
					    sizeof(nat)))
					break;

				printf("%s %-15s %-5hu <- ->",
					getnattype(nat.nat_ptr),
					inet_ntoa(nat.nat_inip),
					ntohs(nat.nat_inport));
				printf(" %-15s %-5hu",
					inet_ntoa(nat.nat_outip),
					ntohs(nat.nat_outport));
				printf(" [%s %hu]", inet_ntoa(nat.nat_oip),
					ntohs(nat.nat_oport));
				printf(" %ld %hu %lx", nat.nat_age,
					nat.nat_use, nat.nat_sumd);
#if SOLARIS
				printf(" %lx", nat.nat_ipsumd);
#endif
				putchar('\n');
			}
		free(nt[0]);
	}
}


u_short	portnum(name, proto)
char	*name, *proto;
{
	struct	servent *sp, *sp2;
	u_short	p1 = 0;

	if (isdigit(*name))
		return htons((u_short)atoi(name));
	if (!proto)
		proto = "tcp/udp";
	if (strcasecmp(proto, "tcp/udp")) {
		sp = getservbyname(name, proto);
		if (sp)
			return sp->s_port;
		(void) fprintf(stderr, "unknown service \"%s\".\n", name);
		return 0;
	}
	sp = getservbyname(name, "tcp");
	if (sp)
		p1 = sp->s_port;
	sp2 = getservbyname(name, "udp");
	if (!sp || !sp2) {
		(void) fprintf(stderr, "unknown tcp/udp service \"%s\".\n",
			name);
		return 0;
	}
	if (p1 != sp2->s_port) {
		(void) fprintf(stderr, "%s %d/tcp is a different port to ",
			name, p1);
		(void) fprintf(stderr, "%s %d/udp\n", name, sp->s_port);
		return 0;
	}
	return p1;
}


u_long	hostmask(msk)
char	*msk;
{
	int	bits = -1;
	u_long	mask;

	if (!isdigit(*msk))
		return (u_long)-1;
	if (strchr(msk, '.'))
		return inet_addr(msk);
	if (strchr(msk, 'x'))
		return (u_long)strtol(msk, NULL, 0);
	/*
	 * set x most significant bits
	 */
	for (mask = 0, bits = atoi(msk); bits; bits--) {
		mask /= 2;
		mask |= ntohl(inet_addr("128.0.0.0"));
	}
	mask = htonl(mask);
	return mask;
}


/*
 * returns an ip address as a long var as a result of either a DNS lookup or
 * straight inet_addr() call
 */
u_long	hostnum(host, resolved)
char	*host;
int	*resolved;
{
	struct	hostent	*hp;
	struct	netent	*np;

	*resolved = 0;
	if (!strcasecmp("any", host))
		return 0L;
	if (isdigit(*host))
		return inet_addr(host);

	if (!(hp = gethostbyname(host))) {
		if (!(np = getnetbyname(host))) {
			*resolved = -1;
			fprintf(stderr, "can't resolve hostname: %s\n", host);
			return 0;
		}
		return np->n_net;
	}
	return *(u_32_t *)hp->h_addr;
}


ipnat_t *parse(line)
char *line;
{
	struct protoent *pr;
	static ipnat_t ipn;
	char *s, *t;
	char *shost, *snetm, *dhost, *proto;
	char *dnetm = NULL, *dport = NULL, *tport = NULL;
	int resolved;

	bzero((char *)&ipn, sizeof(ipn));
	if ((s = strchr(line, '\n')))
		*s = '\0';
	if ((s = strchr(line, '#')))
		*s = '\0';
	if (!*line)
		return NULL;
	if (!(s = strtok(line, " \t")))
		return NULL;
	if (!strcasecmp(s, "map"))
		ipn.in_redir = NAT_MAP;
	else if (!strcasecmp(s, "rdr"))
		ipn.in_redir = NAT_REDIRECT;
	else if (!strcasecmp(s, "bimap"))
		ipn.in_redir = NAT_BIMAP;
	else {
		(void)fprintf(stderr,
			      "expected map/rdr/bimap, got \"%s\"\n", s);
		return NULL;
	}

	if (!(s = strtok(NULL, " \t"))) {
		fprintf(stderr, "missing fields (interface)\n");
		return NULL;
	}
	strncpy(ipn.in_ifname, s, sizeof(ipn.in_ifname) - 1);
	ipn.in_ifname[sizeof(ipn.in_ifname) - 1] = '\0';
	if (!(s = strtok(NULL, " \t"))) {
		fprintf(stderr, "missing fields (%s)\n", 
			ipn.in_redir ? "destination": "source");
		return NULL;
	}
	shost = s;

	if (ipn.in_redir == NAT_REDIRECT) {
		if (!(s = strtok(NULL, " \t"))) {
			fprintf(stderr, "missing fields (destination port)\n");
			return NULL;
		}

		if (strcasecmp(s, "port")) {
			fprintf(stderr, "missing fields (port)\n");
			return NULL;
		}

		if (!(s = strtok(NULL, " \t"))) {
			fprintf(stderr, "missing fields (destination port)\n");
			return NULL;
		}

		dport = s;
	}


	if (!(s = strtok(NULL, " \t"))) {
		fprintf(stderr, "missing fields (->)\n");
		return NULL;
	}
	if (!strcmp(s, "->")) {
		snetm = strrchr(shost, '/');
		if (!snetm) {
			fprintf(stderr, "missing fields (%s netmask)\n",
				ipn.in_redir ? "destination":"source");
			return NULL;
		}
	} else {
		if (strcasecmp(s, "netmask")) {
			fprintf(stderr, "missing fields (netmask)\n");
			return NULL;
		}
		if (!(s = strtok(NULL, " \t"))) {
			fprintf(stderr, "missing fields (%s netmask)\n",
				ipn.in_redir ? "destination":"source");
			return NULL;
		}
		snetm = s;
	}

        if (!(s = strtok(NULL, " \t"))) {
		fprintf(stderr, "missing fields (%s)\n",
			ipn.in_redir ? "destination":"target");
		return NULL;
	}
        dhost = s;

        if (ipn.in_redir & NAT_MAP) {
		if (!(s = strtok(NULL, " \t"))) {
			dnetm = strrchr(dhost, '/');
			if (!dnetm) {
				fprintf(stderr,
					"missing fields (dest netmask)\n");
				return NULL;
			}
		}
		if (!s || !strcasecmp(s, "portmap") ||
		    !strcasecmp(s, "proxy")) {
			dnetm = strrchr(dhost, '/');
			if (!dnetm) {
				fprintf(stderr,
					"missing fields (dest netmask)\n");
				return NULL;
			}
		} else {
			if (strcasecmp(s, "netmask")) {
				fprintf(stderr,
					"missing fields (dest netmask)\n");
				return NULL;
			}
			if (!(s = strtok(NULL, " \t"))) {
				fprintf(stderr,
					"missing fields (dest netmask)\n");
				return NULL;
			}
			dnetm = s;
		}
		if (*dnetm == '/')
			*dnetm++ = '\0';
	} else {
		/* If it's a in_redir, expect target port */
		if (!(s = strtok(NULL, " \t"))) {
			fprintf(stderr, "missing fields (destination port)\n");
			return NULL;
		}

		if (strcasecmp(s, "port")) {
			fprintf(stderr, "missing fields (port)\n");
			return NULL;
		}
	  
		if (!(s = strtok(NULL, " \t"))) {
			fprintf(stderr, "missing fields (destination port)\n");
			return NULL;
		}
		tport = s;
	} 
	    

	if (*snetm == '/')
		*snetm++ = '\0';

	if (ipn.in_redir & NAT_MAP) {
		ipn.in_inip = hostnum(shost, &resolved);
		if (resolved == -1)
			return NULL;
		ipn.in_inmsk = hostmask(snetm);
		ipn.in_outip = hostnum(dhost, &resolved);
		if (resolved == -1)
			return NULL;
		ipn.in_outmsk = hostmask(dnetm);
	} else {
		ipn.in_inip = hostnum(dhost, &resolved); /* Inside is target */
		if (resolved == -1)
			return NULL;
		ipn.in_inmsk = hostmask("255.255.255.255");
		ipn.in_outip = hostnum(shost, &resolved);
		if (resolved == -1)
			return NULL;
		ipn.in_outmsk = hostmask(snetm);
		if (!(s = strtok(NULL, " \t"))) {
			ipn.in_flags = IPN_TCP; /* XXX- TCP only by default */
			proto = "tcp";
		} else {
			if (!strcasecmp(s, "tcp"))
				ipn.in_flags = IPN_TCP;
			else if (!strcasecmp(s, "udp"))
				ipn.in_flags = IPN_UDP;
			else if (!strcasecmp(s, "tcp/udp"))
				ipn.in_flags = IPN_TCPUDP;
			else if (!strcasecmp(s, "tcpudp"))
				ipn.in_flags = IPN_TCPUDP;
			else {
				fprintf(stderr,
					"expected protocol - got \"%s\"\n", s);
				return NULL;
			}
			proto = s;
			if ((s = strtok(NULL, " \t"))) {
				fprintf(stderr,
					"extra junk at the end of rdr: %s\n",
					s);
				return NULL;
			}
		}
		ipn.in_pmin = portnum(dport, proto); /* dest port */
		ipn.in_pmax = ipn.in_pmin; /* NECESSARY of removing nats */
		ipn.in_pnext = portnum(tport, proto); /* target port */
		s = NULL; /* That's all she wrote! */
	}
	ipn.in_inip &= ipn.in_inmsk;
	ipn.in_outip &= ipn.in_outmsk;

	if (!s)
		return &ipn;

	if (ipn.in_redir == NAT_BIMAP) {
		fprintf(stderr, "extra words at the end of bimap line: %s\n",
			s);
		return NULL;
	}
	if (!strcasecmp(s, "proxy")) {
		if (!(s = strtok(NULL, " \t"))) {
			fprintf(stderr, "missing parameter for \"proxy\"\n");
			return NULL;
		}
		dport = NULL;

		if (!strcasecmp(s, "port")) {
			if (!(s = strtok(NULL, " \t"))) {
				fprintf(stderr,
					"missing parameter for \"port\"\n");
				return NULL;
			}

			dport = s;

			if (!(s = strtok(NULL, " \t"))) {
				fprintf(stderr,
					"missing parameter for \"proxy\"\n");
				return NULL;
			}
		}
		if ((proto = index(s, '/'))) {
			*proto++ = '\0';
			if ((pr = getprotobyname(proto)))
				ipn.in_p = pr->p_proto;
			else
				ipn.in_p = atoi(proto);
			if (dport)
				ipn.in_dport = portnum(dport, proto);
		} else {
			ipn.in_p = 0;
			if (dport)
				ipn.in_dport = portnum(dport, NULL);
		}

		(void) strncpy(ipn.in_plabel, s, sizeof(ipn.in_plabel));
		if ((s = strtok(NULL, " \t"))) {
			fprintf(stderr, "too many parameters for \"proxy\"\n");
			return NULL;
		}
		return &ipn;
		
	}
	if (strcasecmp(s, "portmap")) {
		fprintf(stderr, "expected \"portmap\" - got \"%s\"\n", s);
		return NULL;
	}
	if (!(s = strtok(NULL, " \t")))
		return NULL;
	if (!strcasecmp(s, "tcp"))
		ipn.in_flags = IPN_TCP;
	else if (!strcasecmp(s, "udp"))
		ipn.in_flags = IPN_UDP;
	else if (!strcasecmp(s, "tcpudp"))
		ipn.in_flags = IPN_TCPUDP;
	else if (!strcasecmp(s, "tcp/udp"))
		ipn.in_flags = IPN_TCPUDP;
	else {
		fprintf(stderr, "expected protocol name - got \"%s\"\n", s);
		return NULL;
	}
	proto = s;
	if (!(s = strtok(NULL, " \t"))) {
		fprintf(stderr, "no port range found\n");
		return NULL;
	}
	if (!(t = strchr(s, ':'))) {
		fprintf(stderr, "no port range in \"%s\"\n", s);
		return NULL;
	}
	*t++ = '\0';
	ipn.in_pmin = portnum(s, proto);
	ipn.in_pmax = portnum(t, proto);
	return &ipn;
}


void parsefile(fd, file, opts)
int fd;
char *file;
int opts;
{
	char	line[512], *s;
	ipnat_t	*np;
	FILE	*fp;
	int	linenum = 1;

	if (strcmp(file, "-")) {
		if (!(fp = fopen(file, "r"))) {
			perror(file);
			exit(1);
		}
	} else
		fp = stdin;

	while (fgets(line, sizeof(line) - 1, fp)) {
		line[sizeof(line) - 1] = '\0';
		if ((s = strchr(line, '\n')))
			*s = '\0';
		if (!(np = parse(line))) {
			if (*line)
				fprintf(stderr, "%d: syntax error in \"%s\"\n",
					linenum, line);
		} else if (!(opts & OPT_NODO)) {
			if ((opts & OPT_VERBOSE) && np)
				printnat(np, opts &  OPT_VERBOSE, NULL);
			if (opts & OPT_REM) {
				if (ioctl(fd, SIOCADNAT, np) == -1)
					perror("ioctl(SIOCADNAT)");
			} else if (ioctl(fd, SIOCRMNAT, np) == -1)
				perror("ioctl(SIOCRMNAT)");
		}
		linenum++;
	}
	if (fp != stdin)
		fclose(fp);
}


void flushtable(fd, opts)
int fd, opts;
{
	int n = 0;

	if (opts & OPT_FLUSH) {
		n = 0;
		if (!(opts & OPT_NODO) && ioctl(fd, SIOCFLNAT, &n) == -1)
			perror("ioctl(SIOCFLNAT)");
		else
			printf("%d entries flushed from NAT table\n", n);
	}

	if (opts & OPT_CLEAR) {
		n = 0;
		if (!(opts & OPT_NODO) && ioctl(fd, SIOCCNATL, &n) == -1)
			perror("ioctl(SIOCCNATL)");
		else
			printf("%d entries flushed from NAT list\n", n);
	}
}