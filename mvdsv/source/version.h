/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	$Id: version.h,v 1.11 2005/10/12 12:10:50 danfe Exp $
*/
// version.h

#ifdef _WIN32
#define QW_PLATFORM			"Win32"
#define QW_PLATFORM_SHORT	"(w)"
#endif

#ifdef __FreeBSD__
#define QW_PLATFORM			"FreeBSD"
#define QW_PLATFORM_SHORT	"(f)"
#endif

/*Claymore added this stuff ---> */
#ifdef __OpenBSD__
#define QW_PLATFORM			"OpenBSD"
#define QW_PLATFORM_SHORT	"(o)"
#endif
/* <---  */

#ifdef __linux__
#define QW_PLATFORM			"Linux"
#define QW_PLATFORM_SHORT	"(l)"
#endif

#ifdef sun
#define QW_PLATFORM			"Sun"
#define QW_PLATFORM_SHORT	"(s)"
#endif

#ifdef __APPLE__
#define QW_PLATFORM			"Darwin"
#define QW_PLATFORM_SHORT	"(d)"
#endif

#define	QW_VERSION			2.40
#define QWE_VERSION			"0.19.01-CVS"
#define QWE_VERNUM			0.1901
#define SERVER_NAME			"MVDSV"
#define QWDTOOLS_NAME		"QWDtools"
#define PROJECT_NAME		SERVER_NAME
#define PROJECT_URL			"http://mvdsv.sorceforge.net"
/*"QWExtended"*/
#define FULL_VERSION		SERVER_NAME " " QWE_VERSION " " QW_PLATFORM_SHORT ", build %d"
#define BUILD_DATE			"Build date: " __DATE__ ", " __TIME__
#define SIZEOF_FULL_VERSION	(sizeof(FULL_VERSION) + sizeof(BUILD_DATE) + sizeof(int) * 3)

#define LINUX_VERSION		QWE_VERNUM
#define GLQUAKE_VERSION		QWE_VERNUM
#ifdef GLQUAKE
#define QW_RENDERER			"GL"
#else
#define QW_RENDERER			"Soft"
#endif

#define RELEASE_VERSION

extern char full_version[];

int build_number (void);
void Version_f (void);