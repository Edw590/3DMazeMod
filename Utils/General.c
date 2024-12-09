#include <stdio.h>
#include "General.h"

int CDECL MessageBoxPrintf(TCHAR * szCaption, TCHAR * szFormat, ...) {
	TCHAR szBuffer [1024];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat) ;

	va_start (pArgList, szFormat);

	// The last argument to wvsprintf points to the arguments

	_vsntprintf (szBuffer, sizeof (szBuffer) / sizeof (TCHAR),
	szFormat, pArgList);

	// The va_end macro just zeroes out pArgList for no good reason

	va_end (pArgList);

	return MessageBox(NULL, szBuffer, szCaption, 0);
}
