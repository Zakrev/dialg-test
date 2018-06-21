#include <iostream>
#include <string>
#include <list>

#include "source/external/pcre/pcre.h"

#include "source/dialg_common.h"
#include "source/qgroup.h"

#include <stdlib.h> //for system()

using namespace std;
using namespace Dialg;

void read_str(QGroup * qgr)
{
	string buffer;
	char bf;

	list<QPhrase *> phrs_all = qgr->get_phrases();
	list<QPhrase *> phrs_tmp1 = phrs_all;
	list<QPhrase *> phrs_tmp2;

	system("clear");
	cout << ">" << "<" << endl;
	for(bf = cin.get(); bf != 10; bf = cin.get())
	{
		if (bf == 127)
		{
			if (buffer.length() > 0)
			{
				buffer.erase(buffer.length() - 1);

				system("clear");
				cout << ">" << buffer << "< " << endl;

				phrs_tmp1.clear();
				for (auto it = phrs_all.begin(); it != phrs_all.end(); it++)
				{
					QPhrase * phrase = (*it);

					if (phrase->is_partial(buffer))
						phrs_tmp1.push_back(phrase);
				}

			}
		}
		else
		{
			buffer += bf;

			system("clear");
			cout << ">" << buffer << "<" << endl;

			phrs_tmp2.clear();
			for (auto it = phrs_tmp1.begin(); it != phrs_tmp1.end(); it++)
			{
				QPhrase * phrase = (*it);

				if (phrase->is_partial(buffer))
					phrs_tmp2.push_back(phrase);
			}

			phrs_tmp1 = phrs_tmp2;
		}

		for (auto it = phrs_tmp1.begin(); it != phrs_tmp1.end(); it++)
		{
			cout << (*it)->help << endl;
		}
	}
}

static void input_mode();
int main()
{
	QGroup qgr = QGroup("json/group_1.json");

	system("tput civis");
	input_mode();
	read_str(&qgr);
	qgr.clear();
	system("tput cnorm");
}

#include <unistd.h>
#include <termios.h>
static void input_mode()
{
	static struct termios saved_attributes = {0};
	static char is_set = 0;

	if (is_set)
	{
		tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
		return;
	}

	struct termios tattr;

	/* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO))
	{
		fprintf(stderr, "Not a terminal.\n");
		exit(EXIT_FAILURE);
	}

	/* Save the terminal attributes so we can restore them later. */
	tcgetattr(STDIN_FILENO, &saved_attributes);
	atexit(input_mode);

	/* Set the funny terminal modes. */
	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);

	is_set = 1;
}
