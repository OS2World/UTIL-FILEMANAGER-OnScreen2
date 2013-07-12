/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <getopt.hpp>
#include <stdio.h>

#pragma warn -use
static char rcsID[]="$Id: getopt.cpp 1.6 1995/02/10 09:32:16 jallen Exp $";
#pragma warn +use

static CString  _getoptArg;
static int      _getoptError;

/**
 * 
 */
CString getoptArg()
{
	return _getoptArg;
}

/**
 * 
 */
void getoptErr(BOOL state)
{
	_getoptError = state;
}

/**
 * 
 */
static const char* switchChars()
{
#if defined(__UNIX__)
	char*  switchCh = "-";
#elif defined(__MSDOS__) || defined(__OS2__) || defined(__WIN32__)
	char*  switchCh = "/-";
#else
#pragma error Undefined environment
#endif

	if (getenv("SWITCHCHAR"))
		switchCh = getenv("SWITCHCHAR");

	return switchCh;
}

/**
 * 
 */
int isSwitch(char Ch)
{
	return (strchr(switchChars(), Ch) != 0);
}

/**
 * 
 */
void printOptions(option opts[])
{
   int  i;

   fprintf(stderr, "options are:\n");

	for (i=0; opts[i].text; i++)
		{
		fprintf(stderr, " %c%-30s%s\n", *switchChars(), opts[i].desc, opts[i].example);
      }
}

/**
 * 
 */
static int getoptNo(CString optname, option options[])
{
   int actopt = EOO;

	for (int optno=0; options[optno].text; optno++)
		{
		CString s1 = upperCase(optname), s2 = upperCase(options[optno].text);

		if (strncmp(s1, s2, s1.length()) == 0)
			{
			if (actopt != EOO && _getoptError)
				{
				fprintf(stderr, "ambiguous option %s\n", (const char*)optname);

				printOptions(options);

   			exit(3);
				}

			actopt=optno;
			}
		}

   if (actopt == EOO)
      {
      if (_getoptError)
         {
         fprintf(stderr, "invalid option %s\n", (const char*)optname);
			printOptions(options);
         exit(3);
         }
      else
		   {
			return EOO;
			}
      }

   return actopt;
}

/**
 * 
 */
int getopt(int& argno, char *argv[], option options[])
{
   static char* cp = 0;

   if (cp == 0 || *cp == '\0')
      {
      cp = argv[argno];
      }

   // we may have an option
   if (cp && isSwitch(*cp))
      {
      char  optName[128];
      char* pOptName = optName;

      _getoptArg = "";
      cp++;

      while (*cp && strchr("+-:/", *cp ) == 0)
         {
         *pOptName++ = *cp++;
         }

      *pOptName = '\0';

      int optno = getoptNo(optName, options);

      switch (*cp)
         {
         case ':':
            {
            if (options[optno].argtype == option::NO_ARGS)
               {
   				if (_getoptError)
	   				{
		   			fprintf(stderr,
      					   	"option %s cannot take an argument\n",
		      				   options[optno].text
   			      			);

		   			printOptions(options);
			   		exit(3);
				   	}
               }
            else
               {
               _getoptArg = ++cp;
               cp = 0;
               argno++;

					// the argument was blank, this means that the
					// next argv[] must be the argument
					if (_getoptArg == "")
						{
						if (!argv[argno])
							{
		   				if (_getoptError)
	   						{
		   					fprintf(stderr,
      							   	"missing argument for option %s\n",
		      						   options[optno].text
		   			      			);

				   			printOptions(options);
			   				exit(3);
						   	}

							return EOO;
							}

						_getoptArg = argv[argno];
						argno++;
						}

               return options[optno].optno;
               }
            }
         break;

         case '+':
            {
            _getoptArg = "+";

            if (options[optno].argtype == option::NO_ARGS)
               {
   				if (_getoptError)
	   				{
		   			fprintf(stderr,
      					   	"option %s cannot take an argument\n",
		      				   options[optno].text
   			      			);

		   			printOptions(options);
			   		exit(3);
				   	}
               }
            else
               {
               if (*++cp == 0)
                  argno++;

               return options[optno].optno;
               }
            }
         break;

         case '-':
            {
            _getoptArg = "+";

            if (options[optno].argtype == option::NO_ARGS)
               {
   				if (_getoptError)
	   				{
		   			fprintf(stderr,
      					   	"option %s cannot take an argument\n",
		      				   options[optno].text
   			      			);

		   			printOptions(options);
			   		exit(3);
				   	}
               }
            else
               {
               if (*++cp == 0)
                  argno++;

               return options[optno].optno;
               }
            }
         break;

         case '\0':
            argno++;
         case '/':
            {
            if (options[optno].argtype == option::REQUIRES_ARGS)
               {
               if (argv[argno] != 0 && !isSwitch(*argv[argno]))
                  {
				  _getoptArg = argv[argno];

                  argno++;

                  return options[optno].optno;
                  }

			   if (_getoptError)
                  {
            		fprintf(stderr,
                        "option %s requires an argument\n",
		      				options[optno].text
				      		);

   					printOptions(options);
                  exit(3);
                  }
               else
                  {
                  return EOO;
                  }
               }
            else
               {
               return options[optno].optno;
               }
            }
         break;
         }
      }

   cp = 0;

   return EOO;
}
