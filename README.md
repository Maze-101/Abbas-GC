
---

# Garbage Collector Implementation in C

- Garbage Collection is one of the most important features in modern programming languages, It’s the automated process of finding objects that are no longer needed by a program and freeing up their memory space so it can be reused.
- A garbage collector can be made with various techniques:
    - [Reference Counting](#reference-counting)
    - [Mark and Sweep](#mark-and-sweep)
    - Mark and Compact
    - Copying
    - Generational

- **Note** : Consider taking a look at the header files before diving into the source code so that you understand how every technique is implemented.

---

## Reference Counting

- This is one of the simplest and oldest techniques. Every time an object is referenced (e.g., assigned to a variable), its reference count increments by 1. When a reference goes out of scope or is set to null, the count decrements.
    - **The Trigger**: As soon as an object's reference count hits 0, it is immediately destroyed, and its memory is reclaimed.
    - **Pros**: Memory is reclaimed instantly; no long pauses in program execution.
    - **Cons**: It cannot handle cyclical references (e.g., Object A points to Object B, and Object B points to Object A. Their counts will never hit 0, even if the rest of the program loses access to both).

- Every object will now have an `int refcount` attribute

    ```c
    typedef struct Object {
        int refcount;
        object_kind_t kind;
        object_data_t data;
    } object_t;
    ```

- In `object.h` we have 3 main functions for reference counting implementation :
    - `void refcount_inc(object_t *obj)`  : increments the reference count for a specific object.

        ```c
        void refcount_inc(object_t *obj) {
            if (obj == NULL) {
                return;
            }

            obj->refcount++;
            return;
        }
        ```

    - `void refcount_dec(object_t *obj)`  : decrements the reference count for a specific object (if the reference count reaches 0, `refcount_free` is called).

        ```c
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
        ```

    - `void refcount_free(object_t *obj)` : frees the memory space occupied by the object (if the object holds a reference for other objects, thier reference count will be decremented right before the main object itself is freed).

        ```c
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
        ```

---

## Mark and Sweep

- This technique works by finding all objects that are still reachable by the program and freeing the rest. It is usually split into two main phases: first the collector marks every reachable object, then it sweeps through memory and frees every object that was not marked.
    - **The Trigger**: The garbage collector runs when `vm_collect_garbage` is called. It marks reachable objects from the VM stack frames, traces their child references, then sweeps unreachable objects.
    - **Pros**: It can handle cyclical references because it does not depend on reference counts.
    - **Cons**: Memory is not reclaimed immediately; the program may pause while the collector marks and sweeps objects.

- Every object will now have a `bool is_marked` attribute

    ```c
    typedef struct Object {
        bool is_marked;

        object_kind_t kind;
        object_data_t data;
    } object_t;
    ```

- In `vm.h` we have 3 main functions for mark and sweap implementation :
    - `void mark(vm_t *vm)`  : marks all objects that are directly referenced by the current VM stack frames.

        ```c
        void mark(vm_t *vm) {
        for (size_t i = 0; i < vm->frames->count; i++) {
            frame_t *frame = vm->frames->data[i];
            for (size_t j = 0; j < frame->references->count; j++) {
            object_t *obj = frame->references->data[j];
            obj->is_marked = true;
            }
        }
        }
        ```

    - `void trace(vm_t *vm)`  : traces every marked object and marks the objects referenced inside it.

        ```c
        void trace(vm_t *vm) {
        stack_t *gray_objects = stack_new(8);
        if (gray_objects == NULL) {
            return;
        }

        for (size_t i = 0; i < vm->objects->count; i++) {
            object_t *obj = vm->objects->data[i];
            if (obj->is_marked) {
            stack_push(gray_objects, obj);
            }
        }

        while (gray_objects->count > 0) {
            trace_blacken_object(gray_objects, stack_pop(gray_objects));
        }

        stack_free(gray_objects);
        }
        ```

    - `void sweep(vm_t *vm)` : goes through all objects tracked by the VM, frees every unmarked object, and resets the mark on reachable objects for the next collection.

        ```c
        void sweep(vm_t *vm) {
        for(size_t i = 0; i < vm->objects->count ; i++){
            object_t *obj = vm->objects->data[i];
            if(obj->is_marked) obj->is_marked = false;
            else {
            object_free(obj);
            vm->objects->data[i] = NULL;
            }
        }
        stack_remove_nulls(vm->objects);
        }
        ```

- The full garbage collection process is done by calling `vm_collect_garbage`, which runs the mark, trace, and sweep phases in order.

    ```c
    void vm_collect_garbage(vm_t *vm) {
      mark(vm);
      trace(vm);
      sweep(vm);
    }
    ```
