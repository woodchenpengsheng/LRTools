#include "stdafx.h"

fast_string util_int_as_string(int value)
{
	char buf[64];

	SPRINTF_S(buf, "%d", value);

	return fast_string(buf);
}