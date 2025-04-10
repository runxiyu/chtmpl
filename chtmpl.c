/*
 * Copyright (c) 2025 Runxi Yu <https://runxiyu.org>
 * Copyright (c) 2022 Omar Polo <op@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	 parse(FILE *, const char *);

int	 nodebug;

void usage(char *progname)
{
	fprintf(stderr, "usage: %s [-G] [-o out] [file ...]\n", progname);
	exit(1);
}

int
main(int argc, char **argv)
{
	FILE		*fp = stdout;
	const char	*out = NULL;
	int		 ch, i;

	while ((ch = getopt(argc, argv, "Go:")) != -1) {
		switch (ch) {
		case 'G':
			nodebug = 1;
			break;
		case 'o':
			out = optarg;
			break;
		default:
			if (argc == 0)
				usage("chtmpl");
			else
				usage(argv[0]);
		}
	}
	argc -= optind;
	argv += optind;

	if (out && (fp = fopen(out, "w")) == NULL)
		err(1, "can't open %s", out);

	if (argc == 0) {
		if (parse(fp, "/dev/stdin") == -1)
			goto err;
	} else {
		for (i = 0; i < argc; ++i)
			if (parse(fp, argv[i]) == -1)
				goto err;
	}

	if (ferror(fp))
		goto err;

	if (fclose(fp) == -1) {
		fp = NULL;
		goto err;
	}

	return (0);

err:
	if (fp)
		fclose(fp);
	if (out && unlink(out) == -1)
		err(1, "unlink %s", out);
	return (1);
}
