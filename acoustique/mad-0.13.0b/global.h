/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

# ifndef GLOBAL_H
# define GLOBAL_H

/* conditional debugging */

# if defined(DEBUG) && defined(NDEBUG)
#  error "cannot define both DEBUG and NDEBUG"
# endif

# if defined(DEBUG)
#  include <stdio.h>
# endif

/* conditional features */

# if defined(HAVE_MMAP) && !defined(USE_MMAP)
#  undef HAVE_MMAP
# endif

/* conditional replacement function prototypes */

# if !defined(HAVE_STRCASECMP)
int strcasecmp(char const *, char const *);
# endif

# if !defined(HAVE_STRNCASECMP)
int strncasecmp(char const *, char const *, unsigned long);
# endif

/* conditional natural language support */

# define  _(text)	gettext(text)
# define N_(text)	text

# if defined(ENABLE_NLS)
#  include <libintl.h>
# else
#  define gettext(text)				text
# endif

# endif
