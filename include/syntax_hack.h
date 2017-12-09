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
