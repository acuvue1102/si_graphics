// dlmallocを使うためのファイル.

#ifndef DLMALLOC_H
#define DLMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mspace;

mspace create_mspace_with_base(void* base , size_t capacity , int locked );
size_t destroy_mspace(mspace msp);

void*  mspace_malloc(mspace msp, size_t bytes);
void*  mspace_memalign(mspace msp, size_t alignment, size_t bytes);
void   mspace_free(mspace msp, void* mem);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif // DLMALLOC_H
