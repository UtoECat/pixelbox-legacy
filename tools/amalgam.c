/*
 * amalgamate C sources stronger by removing ALL comments and 
 * not important spaces.
 * handwritten and can contain some bugs :)
 *
 * how to write parser ? :
 * 	- write code
 * 	- test on real source
 * 	- debug and fix issues
 * 	- repeat while result is not good enough
 * 	:D
 *
 * Copyright (C) 2023 UtoECat
 * MIT License
 */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define putchar(c) {putc(c, Fout); putchar(c);}
#define getchar() getc(Fin)
#define status(str) {fprintf(stdout, "\n--%s[%c, %i]--\n", str, ch, tt); fflush(stdout);}

#define IS_OPERATOR(c) \
	(c == '+' || c == '-' || c == '=' || c == '<' || c == '>' \
	|| c == '(' || c == ')' || c == '*' || c == '/' || c == '%' \
	|| c == '&' || c == '|' || c == '~' || c == '^' || c == '!' \
	|| c == '.' || c == ',' || c == ':' || c == '?' || c == ';' \
	|| c == '{' || c == '}' || c == '[' || c == ']' || c == '#')

#define HARDTEST() (feof(Fin) || ferror(Fin) || ferror(Fout)) ? (perror(":("), 1) : 0
int  ch = 0;	
FILE *Fin, *Fout;

#define ISALNUM(c) isalnum(c) || c == '_'

static int checkcont(int print) {
	if (ch != '\\') {
		return 0;
	} else if ((ch = getchar()) != '\n') {
		if (print) {
			putchar('\\');
			putchar(ch);
		}
		return 1;
	}
	return 2;
}

static int docomment() {
	if (ch == '/') {
		ch = getchar();
		if (ch == '/') { // C++
			while ((ch = getchar()) != EOF && ch != '\n') {
				if (HARDTEST()) return 0;
				checkcont(0);
			}
			//ch = getchar();
			return 1;
		} else if (ch == '*') { // C
			not_end:
			while ((ch = getchar()) != EOF && ch != '*') {
				if (HARDTEST()) return 0;
			}
			ch = getchar();
			if (ch != '/' && ch != EOF) goto not_end;
			//ch = getchar();
			return 1;
		} else {
			ungetc(ch, Fin);
			ch = '/';
			return 0;
		}
	}
	return 0; // fail
}

enum TokenType {
	TOK_START    = 0,
	TOK_NAME     = 1,
	TOK_OPERATOR = 2,
	TOK_STRING   = 3,
	TOK_DEFINE   = 4
};

enum TokenType tt = TOK_START;

static int doName() {
	//status("name");
	int ok = 0;
	while (ch != EOF && (ISALNUM(ch))) {
		tt = TOK_NAME;
		ok = 1;
		putchar(ch);
		ch = getchar();
	}
	return ok;
}

static int doString() {
	int ok = 0;
	status("str");
	if (ch == '"') {
		putchar(ch);
		ch = getchar();
		while (ch != EOF && ch != '"') {
			if (!checkcont(1)) putchar(ch);
			ch = getchar();
		}
		putchar('"');
		ok = 1;
	} else if (ch == '\'') {
		putchar(ch);
		ch = getchar();
		while (ch != EOF && ch != '\'') {
			if (!checkcont(1)) putchar(ch);
			ch = getchar();
		}
		putchar('\'');
		ok = 1;
	}
	ch = getchar(); // show char after the string
	if (ok) tt = TOK_STRING;
	return ok;
}

static int doOperator() {
	int ok = 0;
	while (ch != EOF && IS_OPERATOR(ch)) {
		tt = TOK_OPERATOR;
		if (!docomment()) {
			putchar(ch);
			//status("op");
			ok = 1;
		} else {
			//status("comment");
		}
		ch = getchar();
	}
	return ok;
}

static void skipSpaces() {
	//status("skip");
	while ((ch==' '|| ch=='\t') && ch != EOF) {
		ch = getchar();
	}
}

static int parseLine() {
	while (ch == '\n') ch = getchar();
	if (ch == ' ' || ch == '\t') skipSpaces();
	if (ch == '#') { // macro parsing
		status("macro");
		putchar('\n');
		putchar('#');
		ch = getchar();
		while (ch != EOF && ch != '\n') {
			if (ISALNUM(ch)) {
				doName(); 
			} else if (IS_OPERATOR(ch)) {
				doOperator();
			} else if (ch == '"' || ch == '\'' || ch == '<') {
				if (ch == '<') {
					putchar(ch);
					ch = getchar();
					while (ch != EOF && ch != '>' && ch != '\n') {
						putchar(ch);
						ch = getchar();
					}
					ch = getchar(); // show char after the string
					putchar('>');
				} else doString();
			} else if (ch == ' ' || ch == '\t') {
				skipSpaces(); putchar(' '); // keep spaces in macros
			} else if (checkcont(1)==2) {
				ch = getchar();
				putchar(' ');
			}
		}
		tt = TOK_DEFINE;
	} else {
		status("code");
		while (ch != EOF && ch != '\n') {
			if (ISALNUM(ch)) {
				if (tt == TOK_NAME || tt == TOK_STRING) 
					putchar(' '); // add space
				if (tt == TOK_DEFINE) 
					putchar('\n'); // on next line please
				doName(); 
			} else if (IS_OPERATOR(ch)) {
				if (tt == TOK_DEFINE) putchar('\n'); // on next line please
				doOperator();
			} else if (ch == '"' || ch == '\'') {
				if (tt == TOK_NAME)
					putchar(' ');
				doString();
			} else if (ch == ' ' || ch == '\t') {
				skipSpaces();
			} else if (checkcont(1) == 2) {
				ch = getchar();
				putchar(' ');
			}
		}
	}
	return ch != EOF;
}

#include <signal.h>
#include <setjmp.h>

jmp_buf throw;

static void fuck(int signal) {
	longjmp(throw, 1);
}

int main(int argc, char** argv) {
	signal(SIGTERM, fuck);
	signal(SIGINT, fuck);

	if (argc < 3) {
		fprintf(stderr, "usage : %s [input] [output]\n", argv[0]);
		return -1;
	}
	Fin = fopen(argv[1], "r");
	if (!Fin) {
		fprintf(stderr, "can't open %s (IN)\n", argv[1]);
		return -2;
	}
	Fout = fopen(argv[2], "w");
	if (!Fout) {
		fprintf(stderr, "can't open %s (OUT)\n", argv[2]);
		return -2;
	}

	ch = getchar();
	while (parseLine()) {
		if (setjmp(throw) > 0) {
			perror("error");
			break;
		}
		status("next line");
		if (HARDTEST()) break;
	}
	putc('\n', stdout);

	fclose(Fin);
	fclose(Fout);
	return 0;
}
