/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <sysdep.hpp>
#include <conditio.hpp>
#include <array.hpp>
#include <slist.hpp>
#include <dlist.hpp>
#include <cstring.hpp>
#include <date.hpp>
#include <dir.hpp>
#include <getopt.hpp>
#include <refptr.hpp>
#include <regexp.hpp>
#include <time.hpp>
#include <tracer.hpp>
#include <types.hpp>
#pragma hdrstop
#include <jac.h>

/**
 * 
 */
const char* EXP_FUNC getRCSVersion(const char* rcsID)
{
   static char buf[128];

	*buf = '\0';

   if (*rcsID == '$') // have we got and RCS id string
      {
      // probably
      char* ep = 0;

      strcpy(buf, strchr(rcsID, ':')+2);

      if ((ep = strchr(buf, ' ')) != 0)
         *ep = '\0';
      else
      if ((ep = strchr(buf, '$')) != 0)
         *ep = '\0';

		int major=0, minor=0, patch=0, level=0;
		int count;

		switch (count = sscanf(buf, "%d.%d.%d.%d", &major, &minor, &patch, &level))
			{
			case -1:
			case 0:
				strcpy(buf, "x.xx");
			break;

			case 1:
			case 2:
				sprintf(buf, "%i.%02i", major, minor);
			break;
			
			case 3:
			case 4:
				sprintf(buf, "%i.%02i.%02i.%02i", major, minor, patch, level);
			break;
			}

		MTrace(("RCSID scan count %i", count));
      }

   return buf;
}
