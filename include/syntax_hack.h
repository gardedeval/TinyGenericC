#pragma once

#ifndef unless
#define unless(expr) if (!(expr))
#endif

#ifndef repeat
#define repeat do
#endif

#ifndef until
#define until(expr) while (!(expr))
#endif

#ifndef typecast
#define typecast(T, v) ((T) (v))
#endif

#ifndef array_sizeof
#define array_type_sizeof(arr) (sizeof((arr)[0]))
#define array_sizeof(arr) (sizeof((arr)) / array_type_sizeof(arr))
#endif
