#pragma once

#define iden_concat_5_gen(a, b, c, d, e) a##b##c##d##e
#define iden_concat_5(a, b, c, d, e) iden_concat_5_gen(a, b, c, d, e)

#define iden_concat_4_gen(a, b, c, d) a##b##c##d
#define iden_concat_4(a, b, c, d) iden_concat_4_gen(a, b, c, d)

#define iden_concat_3_gen(a, b, c) a##b##c
#define iden_concat_3(a, b, c) iden_concat_3_gen(a, b, c)

#define iden_concat_2_gen(a, b) a##b
#define iden_concat_2(a, b) iden_concat_2_gen(a, b)
