#include <criterion/criterion.h>
#include <stddef.h>

#include "object.h"

Test(object_creation, creates_integer_float_string_and_array)
{
    object_t *integer = new_integer(42);
    object_t *floating = new_float(3.5f);
    object_t *string = new_string("hello");
    object_t *array = new_array(2);

    cr_assert_not_null(integer);
    cr_assert_eq(integer->refcount, 1);
    cr_assert_eq(integer->kind, INTEGER);
    cr_assert_eq(integer->data.v_int, 42);

    cr_assert_not_null(floating);
    cr_assert_eq(floating->refcount, 1);
    cr_assert_eq(floating->kind, FLOAT);
    cr_assert_float_eq(floating->data.v_float, 3.5f, 0.0001f);

    cr_assert_not_null(string);
    cr_assert_eq(string->refcount, 1);
    cr_assert_eq(string->kind, STRING);
    cr_assert_str_eq(string->data.v_string, "hello");

    cr_assert_not_null(array);
    cr_assert_eq(array->refcount, 1);
    cr_assert_eq(array->kind, ARRAY);
    cr_assert_eq(array->data.v_array.size, 2);
    cr_assert_null(array_get(array, 0));
    cr_assert_null(array_get(array, 1));

    refcount_dec(integer);
    refcount_dec(floating);
    refcount_dec(string);
    refcount_dec(array);
}

Test(reference_counting, increments_and_decrements_reference_count)
{
    object_t *obj = new_integer(7);

    cr_assert_not_null(obj);
    cr_assert_eq(obj->refcount, 1);

    refcount_inc(obj);
    cr_assert_eq(obj->refcount, 2);

    refcount_dec(obj);
    cr_assert_eq(obj->refcount, 1);

    refcount_dec(obj);
}

Test(vector3, owns_and_releases_component_references)
{
    object_t *x = new_integer(1);
    object_t *y = new_integer(2);
    object_t *z = new_integer(3);

    object_t *vector = new_vector3(x, y, z);

    cr_assert_not_null(vector);
    cr_assert_eq(vector->kind, VECTOR3);
    cr_assert_eq(vector->refcount, 1);
    cr_assert_eq(x->refcount, 2);
    cr_assert_eq(y->refcount, 2);
    cr_assert_eq(z->refcount, 2);
    cr_assert_eq(vector->data.v_vector3.x, x);
    cr_assert_eq(vector->data.v_vector3.y, y);
    cr_assert_eq(vector->data.v_vector3.z, z);

    refcount_dec(vector);

    cr_assert_eq(x->refcount, 1);
    cr_assert_eq(y->refcount, 1);
    cr_assert_eq(z->refcount, 1);

    refcount_dec(x);
    refcount_dec(y);
    refcount_dec(z);
}

Test(array, set_get_replace_and_validate_elements)
{
    object_t *array = new_array(2);
    object_t *first = new_integer(10);
    object_t *second = new_integer(20);
    object_t *not_array = new_integer(99);

    cr_assert_not_null(array);
    cr_assert_not_null(first);
    cr_assert_not_null(second);
    cr_assert_not_null(not_array);

    cr_assert(array_set(array, 0, first));
    cr_assert_eq(array_get(array, 0), first);
    cr_assert_eq(first->refcount, 2);

    cr_assert(array_set(array, 0, second));
    cr_assert_eq(array_get(array, 0), second);
    cr_assert_eq(first->refcount, 1);
    cr_assert_eq(second->refcount, 2);

    cr_assert_not(array_set(array, 2, first));
    cr_assert_not(array_set(not_array, 0, first));
    cr_assert_not(array_set(array, 1, NULL));
    cr_assert_null(array_get(array, 2));
    cr_assert_null(array_get(not_array, 0));
    cr_assert_null(array_get(NULL, 0));

    refcount_dec(array);

    cr_assert_eq(first->refcount, 1);
    cr_assert_eq(second->refcount, 1);

    refcount_dec(first);
    refcount_dec(second);
    refcount_dec(not_array);
}
