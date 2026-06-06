#include <criterion/criterion.h>
#include <stddef.h>

#include "new.h"
#include "object.h"
#include "stack.h"
#include "vm.h"

Test(stack, push_pop_resize_and_remove_nulls)
{
    stack_t *stack = stack_new(2);
    int first = 1;
    int second = 2;
    int third = 3;

    cr_assert_not_null(stack);
    cr_assert_eq(stack->count, 0);
    cr_assert_eq(stack->capacity, 2);

    stack_push(stack, &first);
    stack_push(stack, NULL);
    stack_push(stack, &second);
    stack_push(stack, &third);

    cr_assert_eq(stack->count, 4);
    cr_assert_geq(stack->capacity, 4);

    stack_remove_nulls(stack);

    cr_assert_eq(stack->count, 3);
    cr_assert_eq(stack->data[0], &first);
    cr_assert_eq(stack->data[1], &second);
    cr_assert_eq(stack->data[2], &third);

    cr_assert_eq(stack_pop(stack), &third);
    cr_assert_eq(stack_pop(stack), &second);
    cr_assert_eq(stack_pop(stack), &first);
    cr_assert_null(stack_pop(stack));

    stack_free(stack);
}

Test(objects, constructors_track_objects_in_vm)
{
    vm_t *vm = vm_new();

    object_t *integer = new_integer(vm, 42);
    object_t *floating = new_float(vm, 2.5f);
    object_t *string = new_string(vm, "hello");
    object_t *array = new_array(vm, 2);

    cr_assert_not_null(vm);
    cr_assert_not_null(integer);
    cr_assert_not_null(floating);
    cr_assert_not_null(string);
    cr_assert_not_null(array);

    cr_assert_eq(integer->kind, INTEGER);
    cr_assert_eq(integer->data.v_int, 42);
    cr_assert_eq(floating->kind, FLOAT);
    cr_assert_float_eq(floating->data.v_float, 2.5f, 0.0001f);
    cr_assert_eq(string->kind, STRING);
    cr_assert_str_eq(string->data.v_string, "hello");
    cr_assert_eq(array->kind, ARRAY);
    cr_assert_eq(array->data.v_array.size, 2);
    cr_assert_eq(vm->objects->count, 4);

    vm_free(vm);
}

Test(array, set_get_and_validate_elements)
{
    vm_t *vm = vm_new();
    object_t *array = new_array(vm, 2);
    object_t *first = new_integer(vm, 10);
    object_t *second = new_integer(vm, 20);
    object_t *not_array = new_integer(vm, 99);

    cr_assert_not_null(vm);
    cr_assert_not_null(array);
    cr_assert_not_null(first);
    cr_assert_not_null(second);
    cr_assert_not_null(not_array);

    cr_assert(array_set(array, 0, first));
    cr_assert(array_set(array, 1, second));
    cr_assert_eq(array_get(array, 0), first);
    cr_assert_eq(array_get(array, 1), second);

    cr_assert_not(array_set(array, 2, first));
    cr_assert_not(array_set(array, 0, NULL));
    cr_assert_not(array_set(not_array, 0, first));
    cr_assert_null(array_get(array, 2));
    cr_assert_null(array_get(not_array, 0));
    cr_assert_null(array_get(NULL, 0));

    vm_free(vm);
}

Test(gc, keeps_reachable_graph_and_sweeps_unreachable_objects)
{
    vm_t *vm = vm_new();
    frame_t *frame = vm_new_frame(vm);

    object_t *x = new_integer(vm, 1);
    object_t *y = new_integer(vm, 2);
    object_t *z = new_integer(vm, 3);
    object_t *vector = new_vector3(vm, x, y, z);
    object_t *unreachable = new_string(vm, "sweep me");

    cr_assert_not_null(vm);
    cr_assert_not_null(frame);
    cr_assert_not_null(vector);
    cr_assert_not_null(unreachable);
    cr_assert_eq(vm->objects->count, 5);

    frame_reference_object(frame, vector);
    vm_collect_garbage(vm);

    cr_assert_eq(vm->objects->count, 4);
    cr_assert_eq(vector->is_marked, false);
    cr_assert_eq(x->is_marked, false);
    cr_assert_eq(y->is_marked, false);
    cr_assert_eq(z->is_marked, false);

    frame_t *popped = vm_frame_pop(vm);
    frame_free(popped);

    vm_collect_garbage(vm);
    cr_assert_eq(vm->objects->count, 0);

    vm_free(vm);
}
