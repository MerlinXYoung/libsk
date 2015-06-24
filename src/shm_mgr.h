#ifndef SHM_MGR_H
#define SHM_MGR_H

namespace sk {

enum singleton_type {
    ST_MIN = 0,

    // ...

    ST_MAX
};

int register_singleton(int id, size_t size);

int shm_mgr_init(key_t main_key, key_t aux_key1, key_t aux_key2, key_t aux_key3,
                 bool resume, size_t max_block_size, int chunk_count, size_t heap_size);
int shm_mgr_fini();



namespace detail {

template<typename K>
size_t hashcode(const K& k);

template<typename K, typename V, bool D, size_t(*F)(const K& k)>
struct hash;

template<typename T>
struct stack;

struct mem_chunk;
struct buddy;

} // namespace detail

namespace detail {

// we should not use 0 here, we will leave
// 0 to the invalid pointer type
enum detail_ptr_type {
    PTR_TYPE_SINGLETON = 1,
    PTR_TYPE_CHUNK     = 2,
    PTR_TYPE_HEAP      = 3
};

static const int PTR_TYPE_MASK = 0x3;

struct singleton_ptr {
    int singleton_id: 32;
    int      padding: 30; // is only used to padding the struct to 64 bit
    u32     ptr_type: 2;
};

struct chunk_ptr {
    int chunk_index: 32;
    int block_index: 30;
    u32    ptr_type: 2;
};

struct heap_ptr {
    int unit_index: 32;
    int    padding: 30;
    u32   ptr_type: 2;
};

} // namespace detail


template<typename T>
struct shm_ptr;


/**
 * the layout of shm_mgr:
 *
 *     +---------+-----------+-------+------+
 *     |         | singleton | chunk |      |
 *     | shm_mgr |           |       | heap |
 *     |         |  objects  | pool  |      |
 *     +---------+-----------+-------+------+
 *                           ^       ^      ^
 *                           |       |      |
 *                 pool -----+       |      +----- pool_end_offset
 *                           |   heap_head
 *     pool_head_offset -----+
 */
struct shm_mgr {
    typedef size_t offset_t;
    typedef int index_t;
    typedef detail::hash<size_t, index_t, true, detail::hashcode> size_index_hash;
    typedef detail::stack<index_t> stack;
    typedef detail::mem_chunk mem_chunk;
    typedef detail::buddy heap_allocator;

    /*
     * id of the shm pool
     */
    int shmid;

    /*
     * a block is an area inside a chunk, so:
     *     [max block size] = [chunk size] - [chunk header size]
     */
    size_t chunk_size;
    size_t max_block_size;

    /*
     * according to the heap allocation algorithm,
     * the heap is also seperated into many chunks
     * like the chunk pool, however, we call it
     * "unit" here
     */
    size_t heap_total_size;
    size_t heap_unit_size;

    /*
     * the offsets to singleton objects
     */
    offset_t singletons[ST_MAX];

    /*
     * pool is the base address of entire mem pool
     * also, it is the head of chunk pool
     *
     * pool_head_offset is the head offset of pool:
     *     [pool head] = [pool addr] - [addr of this]
     *
     * pool_end_offset is the end offset of pool:
     *     [pool end] = [actual total shm size]
     *
     * NOTE: pool_end_offset is NOT in the pool!!!
     */
    char *pool;
    offset_t pool_head_offset;
    offset_t pool_end_offset;

    /*
     * the pool will be divided into two parts:
     *     1. chunk pool
     *     2. heap
     *
     * chunk_end is the "realtime" end of chunk pool
     * heap_head is the fixed head of heap
     *
     * chunk_end can grow, if it meets heap_head, then
     * there is no more available chunk, but, it does
     * NOT mean there is no more space on heap
     *
     * NOTE: a variable with type offset_t means an offset
     * measured from address of this ptr, however, the two
     * variables below are measured from pool ptr
     */
    size_t chunk_end;
    size_t heap_head;

    /*
     * free_chunk_hash stores the mapping from block size
     * to chunk index, it only stores those chunks can
     * allocate at leaset one block, full chunks will be
     * erased from the hash
     *
     * empty_chunk_stack stores those empty chunks, so that
     * we can reuse those chunks to store blocks with
     * different sizes
     */
    size_index_hash *free_chunk_hash;
    stack *empty_chunk_stack;

    /*
     * the heap allocator, uses buddy algorithm
     */
    heap_allocator *heap;

    static bool __power_of_two(size_t num);

    static size_t __fix_size(size_t size);

    static size_t __align_size(size_t size);

    static shm_mgr *create(key_t main_key, key_t aux_key1, key_t aux_key2, key_t aux_key3,
                           bool resume, size_t max_block_size, int chunk_count, size_t heap_size);

    static shm_mgr *get();

    void *offset2ptr(offset_t offset);

    offset_t ptr2offset(void *ptr);

    mem_chunk *__offset2chunk(offset_t offset);

    offset_t __chunk2offset(mem_chunk *chunk);

    mem_chunk *__index2chunk(index_t idx);

    int __malloc_from_chunk_pool(size_t mem_size, int& chunk_index, int& block_index);

    int __malloc_from_heap(size_t mem_size, int& unit_index);

    void __free_from_chunk_pool(int chunk_index, int block_index);

    void __free_from_heap(int unit_index);

    void *get_singleton(int id);

    template<typename T>
    shm_ptr<T> malloc() {
        size_t mem_size = __align_size(sizeof(T));

        do {
            if (mem_size > max_block_size)
                break;

            int chunk_index = IDX_NULL;
            int block_index = IDX_NULL;
            int ret = __malloc_from_chunk_pool(mem_size, chunk_index, block_index);

            // 1. the allocation succeeds
            if (ret == 0) {
                detail::chunk_ptr ptr = {0};
                ptr.ptr_type = detail::PTR_TYPE_CHUNK;
                ptr.chunk_index = chunk_index;
                ptr.block_index = block_index;

                return shm_ptr<T>(ptr);
            }

            // 2. fatal errors other than "no more memory", we return NULL
            if (ret != -ENOMEM)
                return shm_ptr<T>();

            // 3. the error is because chunk pool is used up, then we will
            //    allocate it from heap
            break;

        } while (0);

        // if the block should be allocated on heap, or the allocation fails in chunk pool
        // because it is used up, then we do the allocation on heap
        int unit_index = IDX_NULL;
        int ret = __malloc_from_heap(mem_size, unit_index);
        if (ret != 0)
            return shm_ptr<T>();

        detail::heap_ptr ptr = {0};
        ptr.ptr_type = detail::PTR_TYPE_HEAP;
        ptr.unit_index = unit_index;

        return shm_ptr<T>(ptr);
    }

    template<typename T>
    void free(shm_ptr<T> ptr) {
        if (!ptr)
            return;

        u32 ptr_type = ptr.mid & detail::PTR_TYPE_MASK;
        switch (ptr_type) {
        case detail::PTR_TYPE_SINGLETON: {
            detail::singleton_ptr *sptr = cast_ptr(detail::singleton_ptr, &ptr.mid);
            assert_retnone(sptr->ptr_type == detail::PTR_TYPE_SINGLETON);

            assert_noeffect(0);
            return;
        }
        case detail::PTR_TYPE_CHUNK: {
            detail::chunk_ptr *cptr = cast_ptr(detail::chunk_ptr, &ptr.mid);
            assert_retnone(cptr->ptr_type == detail::PTR_TYPE_CHUNK);
            assert_retnone(cptr->chunk_index >= 0 && cptr->block_index >= 0);

            __free_from_chunk_pool(cptr->chunk_index, cptr->block_index);
            return;
        }
        case detail::PTR_TYPE_HEAP: {
            detail::heap_ptr *hptr = cast_ptr(detail::heap_ptr, &ptr.mid);
            assert_retnone(hptr->ptr_type == detail::PTR_TYPE_HEAP);
            assert_retnone(hptr->unit_index >= 0);

            __free_from_heap(hptr->unit_index);
            return;
        }
        default:
            assert_retnone(0);
        }
    }
};



/*
 * the two functions below are used to convert offset pointer and raw pointers
 */
shm_ptr ptr2ptr(void *ptr);

template<typename T>
T *ptr2ptr(shm_ptr ptr) {
    shm_mgr *mgr = shm_mgr::get();
    assert_retnone(mgr);

    void *raw_ptr = mgr->ptr2ptr(ptr);
    return static_cast<T *>(raw_ptr);
}


/*
 * the group of functions below is used to manipulate shared memory allocation/deallocation.
 *
 * shm_malloc/shm_free is like malloc/free in C, only memories are allocated/deallocated,
 * no constructors/destructors are called
 *
 * shm_new/shm_del are like new/delete in C++, they will call constructors/destructors
 * when allocating/deallocating memories
 */
void *shm_malloc(size_t size, shm_ptr& ptr);
void shm_free(shm_ptr ptr);
void shm_free(void *ptr);
void *shm_singleton(int id);


template<typename T>
T *shm_new(shm_ptr& ptr) {
    void *raw_ptr = shm_malloc(sizeof(T), ptr);

    if (raw_ptr == NULL)
        return NULL;

    new (T)(raw_ptr);

    return static_cast<T *>(raw_ptr);
}

template<typename T>
void shm_del(shm_ptr ptr) {
    T *t = ptr2ptr<T>(ptr);
    if (!t)
        return;

    t->~T();

    shm_free(ptr);
}

template<typename T>
void shm_del(T *ptr) {
    if (!ptr)
        return;

    ptr->~T();

    shm_free(ptr);
}

} // namespace sk

#endif // SHM_MGR_H