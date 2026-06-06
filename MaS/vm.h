
#pragma once

#include "object.h"
#include "stack.h"

typedef struct VirtualMachine {
  stack_t *frames;
  stack_t *objects;
} vm_t;

typedef struct StackFrame {
  stack_t *references;
} frame_t;

void mark(vm_t *vm);
void trace(vm_t *vm);
void sweep(vm_t *vm);
void vm_collect_garbage(vm_t *vm);
void trace_blacken_object(stack_t *gray_objects, object_t *ref);
void trace_mark_object(stack_t *gray_objects, object_t *ref);
void vm_collect_garbage(vm_t *vm);
vm_t *vm_new();
void vm_free(vm_t *vm);
void vm_track_object(vm_t *vm, object_t *obj);
frame_t *vm_new_frame(vm_t *vm);
void vm_frame_push(vm_t *vm, frame_t *frame);
frame_t *vm_frame_pop(vm_t *vm);
void frame_free(frame_t *frame);
void frame_reference_object(frame_t *frame, object_t *obj);
