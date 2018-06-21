#include <iostream>
#include <string>
#include <list>

#include "source/external/pcre/pcre.h"

#include "source/dialg_common.h"
#include "source/qgroup.h"

#include <stdlib.h> //for system()

using namespace std;
using namespace Dialg;

static string history;

string read_str(QGroup & qgr)
{
	string buffer;
	char bf;

	system("clear");
	cout << history << ">" << "<" << endl;
	for(bf = cin.get(); bf != 10; bf = cin.get())
	{
		if (bf == 127)
		{
			if (buffer.length() > 0)
			{
				buffer.erase(buffer.length() - 1);

				system("clear");
				cout << history << ">" << buffer << "< " << endl;
			}
		}
		else
		{
			buffer += bf;

			system("clear");
			cout << history << ">" << buffer << "<" << endl;
		}

		for(QPhraseID id = 1; id <= qgr.GetPhrasesCount(); id++)
		{
			if (qgr[id].IsPartial(buffer))
			{
				cout << qgr[id].GetHelp() << endl;
			}
		}
	}

	return buffer;
}

static void input_mode();
int main()
{
	QGroup qgr = QGroup("json/q1.json");
	string buffer;

	system("tput civis");
	input_mode();
	while (buffer != "exit")
	{
		buffer = read_str(qgr);
		history += "User: ";
		history += buffer;
		history += "\n";
	}
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
