#include "arena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static struct arena_chunk *alloc_chunk(size_t size) {
  struct arena_chunk *chunk = malloc(sizeof(struct arena_chunk));
  assert(chunk != NULL);

  chunk->region =
      mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  assert(chunk->region != MAP_FAILED);

  chunk->size = size;
  chunk->index = 0;
  chunk->next = NULL;

  return chunk;
}

struct arena *init_arena() {
  size_t size = sysconf(_SC_PAGESIZE);
  if (size == 0)
    return NULL;

  struct arena *a = malloc(sizeof(struct arena));
  assert(a != NULL);

  struct arena_chunk *chunk = alloc_chunk(size);
  a->head = chunk;
  a->curr = chunk;
  a->chunkSize = size;

  return a;
}

void clear_arena(struct arena *a) {
  if (!a)
    return;

  for (struct arena_chunk *chunk = a->head; chunk; chunk = chunk->next) {
    chunk->index = 0;
  }

  a->curr = a->head;
}

void free_arena(struct arena *a) {
  if (!a)
    return;

  struct arena_chunk *chunk = a->head;
  while (chunk) {
    struct arena_chunk *next = chunk->next;
    munmap(chunk->region, chunk->size); // free mmap region
    free(chunk);
    chunk = next;
  }

  free(a);
}

size_t copy_arena(struct arena *dst, const struct arena *src) {
  if (!dst || !src)
    return 0;

  struct arena_chunk *srcChunk = src->head;
  struct arena_chunk **dstLink = &dst->head;
  size_t totalCopied = 0;

  while (srcChunk) {
    struct arena_chunk *dstChunk = *dstLink;
    if (!dstChunk) {
      dstChunk = alloc_chunk(dst->chunkSize);
      *dstLink = dstChunk;
    }

    size_t toCopy = srcChunk->index;
    if (toCopy > dstChunk->size)
      toCopy = dstChunk->size;

    memcpy(dstChunk->region, srcChunk->region, toCopy);
    dstChunk->index = toCopy;
    totalCopied += toCopy;

    srcChunk = srcChunk->next;
    dstLink = &dstChunk->next;
  }

  dst->curr = dst->head;
  while (dst->curr->next && dst->curr->next->index > 0)
    dst->curr = dst->curr->next;

  return totalCopied;
}

void *arena_alloc(struct arena *a, size_t alignment, size_t size) {
  if (!a || size == 0)
    return NULL;

  if ((alignment & (alignment - 1)) != 0 && alignment != 0) {
    fprintf(stderr, "arena_alloc error: alignment %zu is not power-of-two\n",
            alignment);
    return NULL;
  }

  if (size > a->chunkSize) {
    fprintf(stderr, "arena_alloc error: allocation size %zu > chunk size %zu\n",
            size, a->chunkSize);
    return NULL;
  }

  struct arena_chunk *chunk = a->curr;

  while (chunk) {
    uintptr_t base = (uintptr_t)((char *)chunk->region + chunk->index);
    size_t offset =
        (alignment && (base % alignment)) ? alignment - (base % alignment) : 0;

    if (chunk->index + offset + size <= chunk->size) {
      void *ptr = (char *)chunk->region + chunk->index + offset;
      chunk->index += offset + size;
      a->curr = chunk;
      return ptr;
    }

    if (!chunk->next)
      chunk->next = alloc_chunk(a->chunkSize);

    chunk = chunk->next;
  }

  return NULL;
}
