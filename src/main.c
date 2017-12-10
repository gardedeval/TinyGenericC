#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector.h>
#include <linked_list.h>
#include <math.h>

#include <vector_view.h>
#include <hash_table.h>
#include <json.h>
#include <ref_count.h>

#include <stack.h>

void test_vector() {
    size_t i;
    vec(float) v;
    vec_make(&v);

    for (i = 0; i <= 1024; i++) {
        vec_at_expand(&v, i) = (float)sqrt(i);
    }

    for (i = 0; i <= 1024; i++) {
        common_ensure(vec_at(&v, i) == (float)sqrt(i));
    }

    {
        vec(float) v2;
        size_t i;
        float *value;

        vec_clone(&v2, &v);

        vec_for_each(&v2, i, value) {
            common_ensure(*value == vec_at(&v, i));
        }

        vec_destroy(&v2);
    }

    {
        common_ensure(vec_pop(&v) == 32);

        vec_enqueue(&v, 4);
        vec_enqueue(&v, 3);
        vec_enqueue(&v, 2);
        vec_enqueue(&v, 1);

        common_ensure(vec_bottom(&v) == 1);
        common_ensure(vec_get(&v, 1) == 2);
        common_ensure(vec_get(&v, 2) == 3);
        common_ensure(vec_get(&v, 3) == 4);

        common_ensure(vec_dequeue(&v) == 1);
        common_ensure(vec_dequeue(&v) == 2);
        common_ensure(vec_dequeue(&v) == 3);
        common_ensure(vec_dequeue(&v) == 4);
    }

    vec_destroy(&v);
}

void test_vector_view() {
    char buf[1024];

    vec_view(char) view;
    vec_view_make_arr(&view, buf, 0);

    vec_view_push(&view, 'h');
    vec_view_push(&view, 'e');
    vec_view_push(&view, 'l');
    vec_view_push(&view, 'l');
    vec_view_push(&view, 'o');
    vec_view_push(&view, '\0');

    common_ensure(common_memcmp("hello", view.mem, 5) == 0);

    vec_view_push(&view, 'w');
    vec_view_push(&view, 'o');
    vec_view_push(&view, 'r');
    vec_view_push(&view, 'l');
    vec_view_push(&view, 'd');

    common_ensure(common_memcmp("hello\0world", view.mem, 11) == 0);
}

void test_vector_view_1() {
    char buf[1024];
    int buf2[16];

    vec_view(char) bufView;
    vec_view(int) buf2View;

    vec_view_make_arr(&bufView, buf, 0);
    vec_view_make_arr(&buf2View, buf2, 0);

    // TODO...
}

void test_stack_0() {
    stk(char) s;
    stk_make(&s);
    
    for (size_t i = 0; i < 16; i++) {
        stk_push(&s, i);
    }

    common_ensure(stk_cap(&s) == 16);
    stk_push(&s, 16);
    common_ensure(stk_cap(&s) == 32);

    common_ensure(stk_top(&s) == stk_pop(&s));

    stk_destroy(&s);
}

void test_stack() {
    const char precedences[] = {
        ['+'] = 1,
        ['-'] = 1,
        ['*'] = 2,
        ['/'] = 2,
    };

    vec_view(char) input;
    const char *src = "7 + ((1 + 2) * 3 / (4 - 1))";
    vec_view_make_arr(&input, src, strlen(src));

    stk(char) opStack, tokens;

    

    stk_make(&opStack);
    stk_make(&tokens);
    {
        size_t i;
        char *ref;
        vec_view_for_each(&input, i, ref) {
            if (*ref >= '0' && *ref <= '9') stk_push(&tokens, *ref);
            else if (precedences[*ref] > 0) {
                while (stk_idx(&opStack) > 0 && precedences[stk_top(&opStack)] > precedences[*ref] && stk_top(&opStack) != '(') {
                    stk_push(&tokens, stk_pop(&opStack));
                }
                stk_push(&opStack, *ref);
            } else if (*ref == '(') stk_push(&opStack, *ref);
            else if (*ref == ')') {
                while (stk_idx(&opStack) > 0) {
                    const char op = stk_pop(&opStack);
                    if (op != '(') stk_push(&tokens, op);
                    else break;
                }
            }
        }

        while (stk_idx(&opStack) > 0) {
            stk_push(&tokens, stk_pop(&opStack));
        }

        common_ensure(common_strcmp(stk_mem(&tokens), "712+341-/*+") == 0);
    }
    stk_destroy(&opStack);
    stk_destroy(&tokens);
}


void test_linked_list() {
    typedef ll_t(int) int_ll;
    
    int_ll one;
    int_ll two;
    int_ll three;
    int_ll four;
    int_ll five;
    int_ll six;

    int i = 1;
    ll_make(&one) = i++;
    ll_make(&two) = i++;
    ll_make(&three) = i++;
    ll_make(&four) = i++;
    ll_make(&five) = i++;
    ll_make(&six) = i++;

    /* 1 -> 2 */
    ll_mergel(&one, &two);
    /* (1 -> 2) <- 3 */
    ll_merger(&two, &three);
    /* (1 -> 2 -> 3) -> 4 */
    ll_mergel(&three, &four);
    /* 6 -> (1 -> 2 -> 3 -> 4) */
    ll_mergel(&six, &one);
    /* (6 -> 1 -> 2) <- 5 -> 3 -> 4 */
    ll_merger(&two, &five);

    int_ll *it;
    it = &six;
    common_ensure(it->value == 6);
    it = it->next;
    common_ensure(it->value == 1);
    it = it->next;
    common_ensure(it->value == 2);
    it = it->next;
    common_ensure(it->value == 5);
    it = it->next;
    common_ensure(it->value == 3);
    it = it->next;
    common_ensure(it->value == 4);
    
}

void test_singly_linked_list() {
    int i = 1;
    sll_t(int) *one = sll_alloc(one) != NULL ? one->value = i++, one : NULL;
    sll_t(int) *two = sll_alloc(two) != NULL ? two->value = i++, two : NULL;
    sll_t(int) *three = sll_alloc(three) != NULL ? three->value = i++, three : NULL;
    sll_t(int) *four = sll_alloc(four) != NULL ? four->value = i++, four : NULL;
    sll_t(int) *five = sll_alloc(five) != NULL ? five->value = i++, five : NULL;

    /* cons! */
    sll_push(one, sll_push(two, sll_push(three, sll_push(four, five))));

    {
        sll_t(int) *it = ptr_rtol(one);

        common_ensure(it->value == 1);
        it = it->next;
        common_ensure(it->value == 2);
        it = it->next;
        common_ensure(it->value == 3);
        it = it->next;
        common_ensure(it->value == 4);
        it = it->next;
        common_ensure(it->value == 5);
        it = it->next;
        common_ensure(it == NULL);
    }

    sll_destroy_all(one);
}

void test_singly_linked_list_2() {
    sll_t(int) two, zero, six, three, one, *it; 
    sll_make(&two) = 2, sll_make(&zero) = 0;
    sll_make(&six) = 6, sll_make(&three) = 3;
    sll_make(&one) = 1;

    ptr_rtol(it) = sll_find_loop(&two);
    common_ensure(it == NULL);

    // 2 -> 0 -> 6 -> 3 -> 1 -> |X|
    sll_push(&two, sll_push(&zero, sll_push(&six, sll_push(&three, &one))));

    ptr_rtol(it) = &two;
    common_ensure(it->value == 2);
    it = it->next;
    common_ensure(it->value == 0);
    it = it->next;
    common_ensure(it->value == 6);
    it = it->next;
    common_ensure(it->value == 3);
    it = it->next;
    common_ensure(it->value == 1);
    it = it->next;
    common_ensure(it == NULL);

    ptr_rtol(it) = sll_find_loop(&two);
    common_ensure(it == NULL);

    // 2 -> 0 -> 6 -> 3 -> 1 -> 6 - > 3 -> 1 -> ...
    // or
    //           |--------------| 
    //           V              |
    // 2 -> 0 -> 6 -> 3 -> 1 -> |
    sll_push(&one, &six);

    ptr_rtol(it) = sll_find_loop(&two);
    common_ensure(it != NULL);
    common_ensure(it == &six);

    ptr_rtol(it) = sll_find_loop(&zero);
    common_ensure(it != NULL);
    common_ensure(it == &six);

    ptr_rtol(it) = sll_find_loop(&six);
    common_ensure(it != NULL);
    common_ensure(it == &six);

    // 3 -> 1 -> 6 -> 3 -> 1 -> 6 -> ...
    // or
    // |--------------|
    // V              |
    // 3 -> 1 -> 6 -> |
    ptr_rtol(it) = sll_find_loop(&three);
    common_ensure(it != NULL);
    common_ensure(it == &three);
}

void test_hash_table() {
    size_t i;
    ht_t(double, int) ht;

    ht_make(&ht);
    {
        for (i = 0; i < 1024; i++) {
            ht_put_val(&ht, (double)i, (int)i);
        }

        for (i = 0; i < 1024; i++) {
            common_ensure(ht_get_val(&ht, (double)i) != NULL);
        }

        {
            ht_link_entry_t(double, int) *it0, *head;
            size_t k = 0;
            ht_for_each(&ht, i, head, it0) k++;
            common_ensure(k == 1024);
        }

        {
            ht_entry_t(double, int) *it;
            
            ptr_rtol(it) = ht_get_val(&ht, 45.0);

            common_ensure(it != NULL);
            common_ensure(it->value == 45);
        }
    }
    ht_destroy(&ht);
}

void test_hash_table_2() {
    size_t i;
    ht_t(const char *, int) ht;
    ht_entry_t(const char *, int) *it;
    vec(char *) str;
    vec_make(&str);

    ht_make(&ht);
    {
        for (i = 0; i < 16; i++) {
#define alloc_string(str) memcpy(calloc(1, 16), str, sizeof(str))
            char *test = alloc_string("test");
            vec_push(&str, test);
            char *abcd = alloc_string("abcd");
            vec_push(&str, abcd);
            char *hw = alloc_string("hello world");
            vec_push(&str, hw);
    
            char buf[16];
            sprintf(buf, "%i", i);
    
            strcat(test, buf);
            strcat(abcd, buf);
            strcat(hw, buf);
    
            ht_put_str(&ht, test, (int)i);
            ht_put_str(&ht, abcd, (int)i);
            ht_put_str(&ht, hw, (int)i);
        }
        
        {
            ptr_rtol(it) = ht_get_str(&ht, "abcd1");
            common_ensure(it != NULL);
            common_ensure(it->value == 1);
        }

        {
            ptr_rtol(it) = ht_get_str(&ht, "test0");
            common_ensure(it != NULL);
            common_ensure(it->value == 0);
        }

        {
            ptr_rtol(it) = ht_get_str(&ht, "hello world7");
            common_ensure(it != NULL);
            common_ensure(it->value == 7);
        }

        {
            ptr_rtol(it) = ht_get_str_auto(&ht, "test1234");
            common_ensure(it != NULL);
            common_ensure(it->value == 0);
            common_ensure(ht_get_str(&ht, "test1234") != NULL);
        }

        {
            ht_del_str(&ht, "test1234");

            common_ensure(ht_get_str(&ht, "test1234") == NULL);
        }
    }

    ht_destroy(&ht);
    char **s;
    vec_for_each(&str, i, s) common_free((void *) *s);
    vec_destroy(&str);
}

void test_hash_table_3() {
    ht_t(int, int) table; /* create the table definition */
    ht_t(const char *, double) kv; /* string-number table */
    ht_make(&table); /* do this to initialize the fields */
    ht_make(&kv);

    ht_put_val(&table, 1234, 5678); /* drop in some value */
    ht_put_val(&table, 12345, 1234);
    ht_put_val(&table, 1561, 1734);
    
    common_ensure(ht_get_val(&table, 1234)->value == 5678);
    common_ensure(ht_get_val(&table, 12345)->value == 1234);
    common_ensure(ht_get_val(&table, 1561)->value == 1734);

    ht_del_val(&table, 12345);
    common_ensure(ht_get_val(&table, 12345) == NULL); /* NULL */

    ht_put_str(&kv, "the answer to life, the universe and everything", (double)42); /* add a string-int pair */
    ht_put_str(&kv, "e", 2.71828);
    ht_put_str(&kv, "pi", 3.14159);
    ht_put_str(&kv, "phi", 1.61803);

    common_ensure(ht_get_str(&kv, "the answer to life, the universe and everything")->value == 42.0); /* 42.0 (double representation) */
    common_ensure(ht_get_str(&kv, "e")->value == 2.71828); /* 2.71828 */
    common_ensure(ht_get_str(&kv, "pi")->value == 3.14159); /* 3.14159 */
    common_ensure(ht_get_str(&kv, "phi")->value == 1.61803); /* 1.61803 */

    ht_get_str_auto(&kv, "g")->value = 9.80665; /* if the field does not exist, automatically create one */
    common_ensure(ht_get_str(&kv, "g")->value == 9.80665); /* 9.80665 */

    ht_del_str(&kv, "e");
    common_ensure(ht_get_str(&kv, "e") == NULL); /* NULL */

    ht_destroy(&table); /* clean up the resources used */
    ht_destroy(&kv);
}

void test_hash_table_4() {
    size_t i;
    ht_t(double, int) ht, ht2;

    ht_make(&ht);
    {
        for (i = 0; i < 1024; i++) {
            ht_put_val(&ht, (double)i, (int)i);
        }

        ht_clone(&ht2, &ht);

        for (i = 0; i < 1024; i++) {
            common_ensure(ht_get_val(&ht2, (double)i) != NULL);
        }

    }
    ht_destroy(&ht);
    ht_destroy(&ht2);
}

void test_json_serialize() {

    json j, j5, j6;


    json_make_number(&j5, 123.456);

    json_make_object(&j6);
    json_object_put(&j6, "test", &j5);

    json_make_object(&j);

    json_object_put(&j, "test1", &j6);


    size_t written;
    char buf[65535] = { 0 };
    json_serialize_ret ret = json_serialize(&j, buf, 65535, &written);
    if (ret == SERIAL_OK) {
        printf("%.*s\n", (int)written, buf);
    } else {
        printf("serialization failed\n");
    }

    json_destroy(&j);
}

void test_reference_counting() {
    rc_t(int) i;
    rc_make(&i);

    rc_inflate(&i);
    common_ensure(rc_count(&i) == 2);

    rc_deflate(&i);
    common_ensure(rc_count(&i) == 1);

    rc_inflate(&i);
    rc_inflate(&i);
    rc_inflate(&i);
    rc_enter(&i) {
        common_ensure(rc_count(&i) == 4);
    }
    common_ensure(rc_count(&i) == 3);

    rc_deflate(&i);
    rc_deflate(&i);
    rc_deflate(&i);
    // rc_deflate(&i);
    common_ensure(rc_count(&i) == 0);
    common_ensure(rc_valid(&i) == 0);
}

int test_rc_2_signal = 0;
void test_reference_counting_2_fn(int *ptr) {
    test_rc_2_signal = 1;
}

void test_reference_counting_2() {
    char buf;

    rc_t(int) i;
    rc_pin_mem(&i, &buf, test_reference_counting_2_fn);

    rc_inflate(&i);
    rc_deflate(&i);
    rc_deflate(&i);

    common_ensure(rc_count(&i) == 0);
    common_ensure(rc_valid(&i) == 0);
    common_ensure(test_rc_2_signal == 1);
}

int main(void) {
    test_vector();
    test_vector_view();
    test_vector_view_1();

    test_stack_0();
    test_stack();

    test_linked_list();
    
    test_singly_linked_list();
    test_singly_linked_list_2();

    test_hash_table();
    test_hash_table_2();
    test_hash_table_3();
    test_hash_table_4();

    test_reference_counting();
    test_reference_counting_2();

    /*test_json_serialize();*/
#ifdef MSVC
    printf("done\n");
    getchar();
#endif
    return 0;
}
