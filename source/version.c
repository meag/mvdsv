/*
	version.c
 
	Build number and version strings
 
	Copyright (C) 1996-1997 Id Software, Inc.
 
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 
	See the GNU General Public License for more details.
 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:
 
		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA
 
	$Id: version.c,v 1.4 2005/12/04 05:37:45 disconn3ct Exp $
*/

#ifdef SERVERONLY
#include "qwsvdef.h"
#else
#include "quakedef.h"
#endif
#include "version.h"

// char *date = "Oct 24 1996";
static char *date = __DATE__ ;
static char *mon[12] =
    { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char mond[12] =
    { 31,    28,    31,    30,    31,    30,    31,    31,    30,    31,    30,    31 };

char full_version[SIZEOF_FULL_VERSION];

// returns days since Dec 21 1999
int build_number (void)
{
	int m = 0;
	int d = 0;
	int y = 0;
	static int b = 0;

	if (b != 0)
		return b;

	for (m = 0; m < 11; m++)
	{
		if (strncasecmp( &date[0], mon[m], 3 ) == 0)
			break;
		d += mond[m];
	}

	d += atoi( &date[4] ) - 1;
	y = atoi( &date[7] ) - 1900;
	b = d + (int)((y - 1) * 365.25);

	if (((y % 4) == 0) && m > 1)
		b += 1;

	b -= 36148; // Dec 21 1999

	return b;
}


/*
=======================
Version_f
======================
*/
void Version_f (void)
{
	Con_Printf ("QW version %4.2f\n", QW_VERSION);
#ifdef SERVERONLY
	Con_Printf ("%s\n", full_version);
	Con_Printf (PROJECT_NAME " Project home page: http://mvdsv.sourceforge.net\n\n");
#else
	Con_Printf ("QWExtended version %s (Build %04d)\n", QWE_VERSION, build_number());
	Con_Printf ("Exe: "__TIME__" "__DATE__"\n");
#endif
}