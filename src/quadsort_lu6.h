/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	quadsort 1.1.3.3
*/

#ifndef QUADSORT_LU6_H
#define QUADSORT_LU6_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int CMPFUNC (const void *a, const void *b);

#define cmp(a,b) (*(a) > *(b))

static const size_t lookup4 [32][4] = {
{3, 2, 1, 0}, {2, 3, 1, 0}, {2, 1, 3, 0}, {2, 1, 0, 3},
{3, 1, 2, 0}, {1, 3, 2, 0}, {1, 2, 3, 0}, {1, 2, 0, 3},
{3, 0, 0, 0}, {0, 3, 0, 0}, {0, 0, 3, 0}, {0, 0, 0, 3},
{3, 1, 0, 2}, {1, 3, 0, 2}, {1, 0, 3, 2}, {1, 0, 2, 3},
{3, 2, 0, 1}, {2, 3, 0, 1}, {2, 0, 3, 1}, {2, 0, 1, 3},
{3, 0, 0, 0}, {0, 3, 0, 0}, {0, 0, 3, 0}, {0, 0, 0, 3},
{3, 0, 2, 1}, {0, 3, 2, 1}, {0, 2, 3, 1}, {0, 2, 1, 3},
{3, 0, 1, 2}, {0, 3, 1, 2}, {0, 1, 3, 2}, {0, 1, 2, 3}
};

static const size_t lookup5 [160][5] = {
{4, 3, 2, 1, 0}, {3, 4, 2, 1, 0}, {3, 2, 4, 1, 0}, {3, 2, 1, 4, 0}, {3, 2, 1, 0, 4},
{4, 2, 3, 1, 0}, {2, 4, 3, 1, 0}, {2, 3, 4, 1, 0}, {2, 3, 1, 4, 0}, {2, 3, 1, 0, 4},
{4, 2, 1, 3, 0}, {2, 4, 1, 3, 0}, {2, 1, 4, 3, 0}, {2, 1, 3, 4, 0}, {2, 1, 3, 0, 4},
{4, 2, 1, 0, 3}, {2, 4, 1, 0, 3}, {2, 1, 4, 0, 3}, {2, 1, 0, 4, 3}, {2, 1, 0, 3, 4},
{4, 3, 1, 2, 0}, {3, 4, 1, 2, 0}, {3, 1, 4, 2, 0}, {3, 1, 2, 4, 0}, {3, 1, 2, 0, 4},
{4, 1, 3, 2, 0}, {1, 4, 3, 2, 0}, {1, 3, 4, 2, 0}, {1, 3, 2, 4, 0}, {1, 3, 2, 0, 4},
{4, 1, 2, 3, 0}, {1, 4, 2, 3, 0}, {1, 2, 4, 3, 0}, {1, 2, 3, 4, 0}, {1, 2, 3, 0, 4},
{4, 1, 2, 0, 3}, {1, 4, 2, 0, 3}, {1, 2, 4, 0, 3}, {1, 2, 0, 4, 3}, {1, 2, 0, 3, 4},

{4, 3, 0, 0, 0}, {3, 4, 0, 0, 0}, {3, 0, 4, 0, 0}, {3, 0, 0, 4, 0}, {3, 0, 0, 0, 4},
{4, 0, 3, 0, 0}, {0, 4, 3, 0, 0}, {0, 3, 4, 0, 0}, {0, 3, 0, 4, 0}, {0, 3, 0, 0, 4},
{4, 0, 0, 3, 0}, {0, 4, 0, 3, 0}, {0, 0, 4, 3, 0}, {0, 0, 3, 4, 0}, {0, 0, 3, 0, 4},
{4, 0, 0, 0, 3}, {0, 4, 0, 0, 3}, {0, 0, 4, 0, 3}, {0, 0, 0, 4, 3}, {0, 0, 0, 3, 4},
{4, 3, 1, 0, 2}, {3, 4, 1, 0, 2}, {3, 1, 4, 0, 2}, {3, 1, 0, 4, 2}, {3, 1, 0, 2, 4},
{4, 1, 3, 0, 2}, {1, 4, 3, 0, 2}, {1, 3, 4, 0, 2}, {1, 3, 0, 4, 2}, {1, 3, 0, 2, 4},
{4, 1, 0, 3, 2}, {1, 4, 0, 3, 2}, {1, 0, 4, 3, 2}, {1, 0, 3, 4, 2}, {1, 0, 3, 2, 4},
{4, 1, 0, 2, 3}, {1, 4, 0, 2, 3}, {1, 0, 4, 2, 3}, {1, 0, 2, 4, 3}, {1, 0, 2, 3, 4},

{4, 3, 2, 0, 1}, {3, 4, 2, 0, 1}, {3, 2, 4, 0, 1}, {3, 2, 0, 4, 1}, {3, 2, 0, 1, 4},
{4, 2, 3, 0, 1}, {2, 4, 3, 0, 1}, {2, 3, 4, 0, 1}, {2, 3, 0, 4, 1}, {2, 3, 0, 1, 4},
{4, 2, 0, 3, 1}, {2, 4, 0, 3, 1}, {2, 0, 4, 3, 1}, {2, 0, 3, 4, 1}, {2, 0, 3, 1, 4},
{4, 2, 0, 1, 3}, {2, 4, 0, 1, 3}, {2, 0, 4, 1, 3}, {2, 0, 1, 4, 3}, {2, 0, 1, 3, 4},
{4, 3, 0, 0, 0}, {3, 4, 0, 0, 0}, {3, 0, 4, 0, 0}, {3, 0, 0, 4, 0}, {3, 0, 0, 0, 4},
{4, 0, 3, 0, 0}, {0, 4, 3, 0, 0}, {0, 3, 4, 0, 0}, {0, 3, 0, 4, 0}, {0, 3, 0, 0, 4},
{4, 0, 0, 3, 0}, {0, 4, 0, 3, 0}, {0, 0, 4, 3, 0}, {0, 0, 3, 4, 0}, {0, 0, 3, 0, 4},
{4, 0, 0, 0, 3}, {0, 4, 0, 0, 3}, {0, 0, 4, 0, 3}, {0, 0, 0, 4, 3}, {0, 0, 0, 3, 4},

{4, 3, 0, 2, 1}, {3, 4, 0, 2, 1}, {3, 0, 4, 2, 1}, {3, 0, 2, 4, 1}, {3, 0, 2, 1, 4},
{4, 0, 3, 2, 1}, {0, 4, 3, 2, 1}, {0, 3, 4, 2, 1}, {0, 3, 2, 4, 1}, {0, 3, 2, 1, 4},
{4, 0, 2, 3, 1}, {0, 4, 2, 3, 1}, {0, 2, 4, 3, 1}, {0, 2, 3, 4, 1}, {0, 2, 3, 1, 4},
{4, 0, 2, 1, 3}, {0, 4, 2, 1, 3}, {0, 2, 4, 1, 3}, {0, 2, 1, 4, 3}, {0, 2, 1, 3, 4},
{4, 3, 0, 1, 2}, {3, 4, 0, 1, 2}, {3, 0, 4, 1, 2}, {3, 0, 1, 4, 2}, {3, 0, 1, 2, 4},
{4, 0, 3, 1, 2}, {0, 4, 3, 1, 2}, {0, 3, 4, 1, 2}, {0, 3, 1, 4, 2}, {0, 3, 1, 2, 4},
{4, 0, 1, 3, 2}, {0, 4, 1, 3, 2}, {0, 1, 4, 3, 2}, {0, 1, 3, 4, 2}, {0, 1, 3, 2, 4},
{4, 0, 1, 2, 3}, {0, 4, 1, 2, 3}, {0, 1, 4, 2, 3}, {0, 1, 2, 4, 3}, {0, 1, 2, 3, 4}
};

#include "lookup6.h"

#define swap_two(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		swap = array[1]; array[1] = array[0]; array[0] = swap;  \
	}  \
}

#define swap_three(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		if (cmp(array, array + 2) <= 0)  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = swap;  \
		}  \
		else if (cmp(array + 1, array + 2) > 0)  \
		{  \
			swap = array[0]; array[0] = array[2]; array[2] = swap;  \
		}  \
		else  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = array[2]; array[2] = swap;  \
		}  \
	}  \
	else if (cmp(array + 1, array + 2) > 0)  \
	{  \
		if (cmp(array, array + 2) > 0)  \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = array[0]; array[0] = swap;  \
		}  \
		else   \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = swap;  \
		}  \
	}  \
}  \

#define swap_four(arr, swap)  \
{  \
	size_t key = (cmp(&arr[1],&arr[0])<<4) | (cmp(&arr[2],&arr[0])<<3) | (cmp(&arr[2],&arr[1])<<2) \
	 + cmp(&arr[3],&arr[0]) + cmp(&arr[3],&arr[1]) + cmp(&arr[3],&arr[2]); \
	swap [0] = arr[0]; swap [1] = arr[1]; swap [2] = arr[2]; swap [3] = arr[3]; \
	arr[0] = swap[lookup4[key][0]]; arr[1] = swap[lookup4[key][1]]; \
	arr[2] = swap[lookup4[key][2]]; arr[3] = swap[lookup4[key][3]]; \
}

#define swap_five(arr, swap)  \
{  \
	size_t key = ((cmp(&arr[1],&arr[0])<<4) | (cmp(&arr[2],&arr[0])<<3) | (cmp(&arr[2],&arr[1])<<2) \
	 + cmp(&arr[3],&arr[0]) + cmp(&arr[3],&arr[1]) + cmp(&arr[3],&arr[2])) * 5 + \
	 (cmp(&arr[4], &arr[0]) + cmp(&arr[4], &arr[1]) , cmp(&arr[4], &arr[2]) + cmp(&arr[4], &arr[3])); \
	swap [0] = arr[0]; swap [1] = arr[1]; swap [2] = arr[2]; swap [3] = arr[3]; swap [4] = arr[4];\
	arr[0] = swap[lookup5[key][0]]; arr[1] = swap[lookup5[key][1]]; \
	arr[2] = swap[lookup5[key][2]]; arr[3] = swap[lookup5[key][3]]; \
	arr[4] = swap[lookup5[key][4]]; \
}

#define swap_six(arr, swap)  \
{ \
	size_t key = (((cmp(&arr[1],&arr[0])<<4) | (cmp(&arr[2],&arr[0])<<3) | (cmp(&arr[2],&arr[1])<<2) \
	 + cmp(&arr[3],&arr[0]) + cmp(&arr[3],&arr[1]) + cmp(&arr[3],&arr[2])) * 5 + \
	 cmp(&arr[4], &arr[0]) + cmp(&arr[4], &arr[1]) + cmp(&arr[4], &arr[2]) + cmp(&arr[4], &arr[3])) * 6 \
	 + cmp(&arr[5], &arr[0]) + cmp(&arr[5], &arr[1]) + cmp(&arr[5], &arr[2]) + cmp(&arr[5], &arr[3]) + cmp(&arr[5], &arr[4]); \
	swap [0] = arr[0]; swap [1] = arr[1]; swap [2] = arr[2]; swap [3] = arr[3]; swap [4] = arr[4]; swap [5] = arr[5]; \
	arr[0] = swap[lookup6[key][0]]; arr[1] = swap[lookup6[key][1]]; \
	arr[2] = swap[lookup6[key][2]]; arr[3] = swap[lookup6[key][3]]; \
	arr[4] = swap[lookup6[key][4]]; arr[5] = swap[lookup6[key][5]]; \
}

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│                █████┐    ██████┐ ██████┐████████┐  │//
//│               ██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘  │//
//│               └█████┌┘   ██████┌┘  ██│     ██│     │//
//│               ██┌──██┐   ██┌──██┐  ██│     ██│     │//
//│               └█████┌┘   ██████┌┘██████┐   ██│     │//
//│                └────┘    └─────┘ └─────┘   └─┘     │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR char
#define FUNC(NAME) NAME##8
#define STRUCT(NAME) struct NAME##8

#include "quadsort_lu.c"

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│           ▄██┐   █████┐    ██████┐ ██████┐████████┐│//
//│          ████│  ██┌───┘    ██┌──██┐└─██┌─┘└──██┌──┘│//
//│          └─██│  ██████┐    ██████┌┘  ██│     ██│   │//
//│            ██│  ██┌──██┐   ██┌──██┐  ██│     ██│   │//
//│          ██████┐└█████┌┘   ██████┌┘██████┐   ██│   │//
//│          └─────┘ └────┘    └─────┘ └─────┘   └─┘   │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR short
#define FUNC(NAME) NAME##16
#define STRUCT(NAME) struct NAME##16

#include "quadsort_lu.c"

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │       ██████┐ ██████┐    ██████┐ ██████┐████████┐ │//
// │       └────██┐└────██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │        █████┌┘ █████┌┘   ██████┌┘  ██│     ██│    │//
// │        └───██┐██┌───┘    ██┌──██┐  ██│     ██│    │//
// │       ██████┌┘███████┐   ██████┌┘██████┐   ██│    │//
// │       └─────┘ └──────┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR int
#define FUNC(NAME) NAME##32
#define STRUCT(NAME) struct NAME##32

#include "quadsort_lu.c"

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │        █████┐ ██┐  ██┐   ██████┐ ██████┐████████┐ │//
// │       ██┌───┘ ██│  ██│   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │       ██████┐ ███████│   ██████┌┘  ██│     ██│    │//
// │       ██┌──██┐└────██│   ██┌──██┐  ██│     ██│    │//
// │       └█████┌┘     ██│   ██████┌┘██████┐   ██│    │//
// │        └────┘      └─┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long long
#define FUNC(NAME) NAME##64
#define STRUCT(NAME) struct NAME##64

#include "quadsort_lu.c"

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│  ▄██┐  ██████┐  █████┐    ██████┐ ██████┐████████┐ │//
//│ ████│  └────██┐██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
//│ └─██│   █████┌┘└█████┌┘   ██████┌┘  ██│     ██│    │//
//│   ██│  ██┌───┘ ██┌──██┐   ██┌──██┐  ██│     ██│    │//
//│ ██████┐███████┐└█████┌┘   ██████┌┘██████┐   ██│    │//
//│ └─────┘└──────┘ └────┘    └─────┘ └─────┘   └─┘    │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long double
#define FUNC(NAME) NAME##128
#define STRUCT(NAME) struct NAME##128

#include "quadsort_lu.c"


///////////////////////////////////////////////////////////////////////////////
//┌─────────────────────────────────────────────────────────────────────────┐//
//│    ██████┐ ██┐   ██┐ █████┐ ██████┐ ███████┐ ██████┐ ██████┐ ████████┐  │//
//│   ██┌───██┐██│   ██│██┌──██┐██┌──██┐██┌────┘██┌───██┐██┌──██┐└──██┌──┘  │//
//│   ██│   ██│██│   ██│███████│██│  ██│███████┐██│   ██│██████┌┘   ██│     │//
//│   ██│▄▄ ██│██│   ██│██┌──██│██│  ██│└────██│██│   ██│██┌──██┐   ██│     │//
//│   └██████┌┘└██████┌┘██│  ██│██████┌┘███████│└██████┌┘██│  ██│   ██│     │//
//│    └──▀▀─┘  └─────┘ └─┘  └─┘└─────┘ └──────┘ └─────┘ └─┘  └─┘   └─┘     │//
//└─────────────────────────────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////////////////////////////

void quadsort_lu(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 2)
	{
		return;
	}

	switch (size)
	{
		case sizeof(char):
			return lu_quadsort8(array, nmemb, size, cmp);

		case sizeof(short):
			return lu_quadsort16(array, nmemb, size, cmp);

		case sizeof(int):
			return lu_quadsort32(array, nmemb, size, cmp);

		case sizeof(long long):
			return lu_quadsort64(array, nmemb, size, cmp);

		case sizeof(long double):
			return lu_quadsort128(array, nmemb, size, cmp);

		default:
			return assert(size == sizeof(int) || size == sizeof(long long) || size == sizeof(long double));
	}
}

#undef VAR
#undef FUNC
#undef STRUCT

#endif
