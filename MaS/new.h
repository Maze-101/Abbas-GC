#pragma once

#include "object.h"
#include "vm.h"

object_t *new_integer(vm_t *vm, int value);
object_t *new_float(vm_t *vm, float value);
object_t *new_string(vm_t *vm, char *value);
object_t *new_vector3(vm_t *vm, object_t *x, object_t *y,
                                object_t *z);
object_t *new_array(vm_t *vm, size_t size);
