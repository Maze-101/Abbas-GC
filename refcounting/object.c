#include "object.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool array_set(object_t *obj, size_t index,
                    object_t *value) {
    if (obj == NULL || value == NULL) {
        return false;
    }
    if (obj->kind != ARRAY) {
        return false;
    }
    if (index >= obj->data.v_array.size) {
        return false;
    }
    refcount_dec(obj->data.v_array.elements[index]);
    obj->data.v_array.elements[index] = value;
    refcount_inc(value);
    return true;
}

void refcount_free(object_t *obj) {
    switch (obj->kind) {
        case INTEGER:
        case FLOAT:
            break;
        case STRING:
            free(obj->data.v_string);
            break;
        case VECTOR3: {
            vector_t vec = obj->data.v_vector3;
            refcount_dec(vec.x);
            refcount_dec(vec.y);
            refcount_dec(vec.z);
            break;
        }
        case ARRAY: {
            for(size_t i = 0; i < obj->data.v_array.size ; i++){
                refcount_dec(obj->data.v_array.elements[i]);
            }
            free(obj->data.v_array.elements);
            break;
        }
        default:
            assert(false);
    }
    free(obj);
}

object_t *array_get(object_t *obj, size_t index) {
    if (obj == NULL) {
        return NULL;
    }

    if (obj->kind != ARRAY) {
        return NULL;
    }

    if (index >= obj->data.v_array.size) {
        return NULL;
    }

    return obj->data.v_array.elements[index];
}

void refcount_inc(object_t *obj) {
    if (obj == NULL) {
        return;
    }

    obj->refcount++;
    return;
}

void refcount_dec(object_t *obj) {
    if (obj == NULL) {
        return;
    }
    obj->refcount--;
    if (obj->refcount == 0) {
        refcount_free(obj);
        return;
    }
    return;
}

object_t *_new_object() {
    object_t *obj = calloc(1, sizeof(object_t));
    if (obj == NULL) {
        return NULL;
    }

    obj->refcount = 1;

    return obj;
}

object_t *new_array(size_t size) {
    object_t *obj = _new_object();
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

object_t *new_integer(int value) {
    object_t *obj = _new_object();
    if (obj == NULL) {
        return NULL;
    }

    obj->kind = INTEGER;
    obj->data.v_int = value;
    return obj;
}

object_t *new_float(float value) {
    object_t *obj = _new_object();
    if (obj == NULL) {
        return NULL;
    }

    obj->kind = FLOAT;
    obj->data.v_float = value;
    return obj;
}

object_t *new_string(char *value) {
    object_t *obj = _new_object();
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

object_t *new_vector3(object_t *x, object_t *y,
                                object_t *z) {
    if (x == NULL || y == NULL || z == NULL) {
        return NULL;
    }
    object_t *obj = _new_object();
    if (obj == NULL) {
        return NULL;
    }
    obj->kind = VECTOR3;
    obj->data.v_vector3 = (vector_t){.x = x, .y = y, .z = z};
    refcount_inc(x);
    refcount_inc(y);
    refcount_inc(z);
    return obj;
}
