
#include "new.h"
#include "object.h"
#include "vm.h"
#include <stdlib.h>
#include <string.h>

object_t *_new_object(vm_t *vm) {
    object_t *obj = calloc(1, sizeof(object_t));
    if (obj == NULL) {
        return NULL;
    }

    obj->is_marked = false;

    vm_track_object(vm, obj);

    return obj;
}

object_t *new_array(vm_t *vm, size_t size) {
    object_t *obj = _new_object(vm);
    if (obj == NULL) {
        return NULL;
    }

    object_t **elements = calloc(size, sizeof(object_t *));
    if (elements == NULL) {
        free(obj);
        return NULL;
    }

    obj->kind = ARRAY;
    obj->data.v_array = (array_t){.size = size, .elements = elements};

    return obj;
}

object_t *new_vector3(vm_t *vm, object_t *x, object_t *y,
                                object_t *z) {
    if (x == NULL || y == NULL || z == NULL) {
        return NULL;
    }

    object_t *obj = _new_object(vm);
    if (obj == NULL) {
        return NULL;
    }

    obj->kind = VECTOR3;
    obj->data.v_vector3 = (vector_t){.x = x, .y = y, .z = z};

    return obj;
}

object_t *new_integer(vm_t *vm, int value) {
    object_t *obj = _new_object(vm);
    if (obj == NULL) {
        return NULL;
    }

    obj->kind = INTEGER;
    obj->data.v_int = value;

    return obj;
}

object_t *new_float(vm_t *vm, float value) {
    object_t *obj = _new_object(vm);
    if (obj == NULL) {
        return NULL;
    }

    obj->kind = FLOAT;
    obj->data.v_float = value;
    return obj;
}

object_t *new_string(vm_t *vm, char *value) {
    object_t *obj = _new_object(vm);
    if (obj == NULL) {
        return NULL;
    }

    int len = strlen(value);
    char *dst = malloc(len + 1);
    if (dst == NULL) {
        free(obj);
        return NULL;
    }

    strcpy(dst, value);

    obj->kind = STRING;
    obj->data.v_string = dst;
    return obj;
}
