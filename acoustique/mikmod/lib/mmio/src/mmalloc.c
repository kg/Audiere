/*

 Mikmod Portable System Management Facilities (the MMIO)

  By Jake Stine of Divine Entertainment (1996-2000) and

 Support:
  If you find problems with this code, send mail to:
    air@divent.org

 Distribution / Code rights:
  Use this source code in any fashion you see fit.  Giving me credit where
  credit is due is optional, depending on your own levels of integrity and
  honesty.

 ------------------------------------------------------
 module: mmalloc.c  (that's mm - alloc!)
 
 The Mikmod Portable Memory Allocation Layer

 Generally speaking, Mikmod only allocates nice and simple blocks of
 memory.  None-the-less, I allow the user the option to assign their own
 memory manage routines, for whatever reason it may be that they want them
 used.
 
 Notes:
  - All default allocation procedures can now ensure a fixed byte alignment on
    any allocated memory block.  The alignment is adjustable via the _MM_ALIGN
    compiler define.

*/

#include "mmio.h"
#include <string.h>

#ifdef _MSC_VER
#include <assert.h>
#endif

#define NUM_BLOCK_TYPES         16

#ifdef _DEBUG

#ifndef _MM_OVERWRITE_BUFFER
#define _MM_OVERWRITE_BUFFER    16
#endif

#ifndef _MM_OVERWRITE_FILL
#define _MM_OVERWRITE_FILL      0x99999999
#endif

#else

#ifndef _MM_OVERWRITE_BUFFER
#define _MM_OVERWRITE_BUFFER     0
#endif

#endif

static CHAR *msg_fail = "%s > Failure allocating block of size: %d";
static CHAR *msg_set  = "Out of memory!  Please check the logfile for more information.";


// Add the buffer threshold value and the size of the 'pointer storage space'
// onto the buffer size. Then, align the buffer size to the set alignment.

#define BLOCKHEAP_THRESHOLD     3072

static MM_ALLOC      *globalalloc = NULL;
static MM_BLOCKINFO  *blockheap = NULL; 
static uint           heapsize;               // number of blocks in the heap
static uint           lastblock;              // last block free'd (or first free block)

#ifdef _DEBUG

// =====================================================================================
    static BOOL checkblock(MM_ALLOC *type, MM_BLOCKINFO *block, BOOL doassert)
// =====================================================================================
{
    ULONG   *ptr = block->block;
    uint     i;

    if(!block->block) return FALSE;

    // Check for Underwrites
    // ---------------------

    for(i=0; i<_MM_OVERWRITE_BUFFER; i++, ptr++)
    {   if(*ptr != _MM_OVERWRITE_FILL)
        {   if(doassert)
            {   _mmlog("** _mm_checkblock > Memory underwrite on block type '%d'", type->name);
                assert(FALSE);
            }
            return FALSE;
        }
    }

    // Check for Overwrites
    // --------------------

    ptr = (ULONG *)block->block;
    ptr = &ptr[block->size + _MM_OVERWRITE_BUFFER];

    for(i=0; i<_MM_OVERWRITE_BUFFER; i++, ptr++)
    {   if(*ptr != _MM_OVERWRITE_FILL)
        {   if(doassert)
            {   _mmlog("** _mm_checkblock > Memory overwrite on block type '%d'", type->name);
                assert(FALSE);
            }
            return FALSE;
        }
    }
    return TRUE;
}


// =====================================================================================
    void _mm_checkallblocks(MM_ALLOC *type)
// =====================================================================================
// Does a routine check on all blocks in the specified allocation type for possible mem-
// ory corruption (via the overflow and underflow buffers or, as some people call them,
// 'No Mans Land Buffers').
{
    if(!type) type = globalalloc;

    if(type)
    {   int            cruise = type->blocklist;
        MM_ALLOC      *typec  = type->children;

        while(typec)
        {   MM_ALLOC  *cnext = typec->next;
            _mm_checkallblocks(typec);
            typec = cnext;
        }

        while(cruise != -1)
        {   MM_BLOCKINFO   *block = &blockheap[cruise];
            checkblock(type, block, TRUE);
            cruise = block->next;
        }
    }

}


// =====================================================================================
    void _mm_allocreportall(void)
// =====================================================================================
// Dumps a memory report to logfile on all memory types.
{

}

#else
#define checkblock(x,y,z)  TRUE
#endif


// =====================================================================================
    static void tallyblocks(MM_ALLOC *type, uint *runsum, uint *blkcnt)
// =====================================================================================
{
    int cruise = type->blocklist;

    if(type->children)
    {   MM_ALLOC *child = type->children;
        while(child)
        {   tallyblocks(child, runsum, blkcnt);
            child = child->next;
        }
    }

    while(cruise != -1)
    {   MM_BLOCKINFO   *block = &blockheap[cruise];
        *runsum += block->size;
        (*blkcnt)++;
        cruise = block->next;
    }
}


// =====================================================================================
    void _mmalloc_reportchild(MM_ALLOC *type, uint tabsize)
// =====================================================================================
{
    CHAR   tab[60];
    uint   i;

    for(i=0; i<tabsize; i++) tab[i] = 32;
    tab[i] = 0;

    if(!type) return;


    if(type)
    {   int  cruise = type->blocklist;

        uint    runsum = 0;
        uint    blkcnt = 0;

        _mmlog("\n%sChild AllocReport for %s", tab, type->name);

        if(type->children)
        {   tallyblocks(type, &runsum, &blkcnt);
            _mmlog("%sTotal Blocks    : %d\n"
                   "%sTotal Memory    : %dK", tab, blkcnt, tab, runsum/256);
        }

        runsum = 0;
        blkcnt = 0;

        while(cruise != -1)
        {   MM_BLOCKINFO   *block = &blockheap[cruise];
            runsum += block->size;
            blkcnt++;
            cruise = block->next;
        }

        _mmlog("%sLocal Blocks    : %d\n"
               "%sLocal Memory    : %dK", tab, blkcnt, tab, runsum/256);

        blkcnt = 0;
        while(cruise != -1)
        {
            MM_BLOCKINFO   *block = &blockheap[cruise];

            if(type->flags & MMAF_CHECK_OVERWRITES)
            {   if(!checkblock(type, block, FALSE))
                    blkcnt++;
            }

            cruise = block->next;
        }
        if(blkcnt) _mmlog("%sBad Blocks          : %d", tab,blkcnt);


        if(type->children)
        {
            MM_ALLOC *child = type->children;
            _mmlog("%sChildren types:", tab);

            while(child)
            {   _mmlog("%s - %s", tab, child->name);
                child = child->next;
            }

            child = type->children;
            while(child)
            {   _mmalloc_reportchild(child, tabsize+4);
                child = child->next;
            }
        }
    }
}


// =====================================================================================
    void _mmalloc_report(MM_ALLOC *type)
// =====================================================================================
// Dumps a memory report to logfile on the specified block type.
{
    if(!type)
        type = globalalloc;

    if(type)
    {   uint    runsum = 0;
        uint    blkcnt = 0;
        int     cruise = type->blocklist;

        _mmlog("\nAllocReport for %s\n--------------------------------------\n", type->name);

        if(type->children)
        {   tallyblocks(type, &runsum, &blkcnt);
            _mmlog("Total Blocks    : %d\n"
                   "Total Memory    : %dK", blkcnt, runsum/256);
        }

        runsum = 0;
        blkcnt = 0;

        while(cruise != -1)
        {   MM_BLOCKINFO   *block = &blockheap[cruise];
            runsum += block->size;
            blkcnt++;
            cruise = block->next;
        }

        _mmlog("Local Blocks    : %d\n"
               "Local Memory    : %dK", blkcnt, runsum/256);

        blkcnt = 0;
        cruise = type->blocklist;
        while(cruise != -1)
        {
            MM_BLOCKINFO   *block = &blockheap[cruise];

            if(type->flags & MMAF_CHECK_OVERWRITES)
            {   if(!checkblock(type, block, FALSE))
                    blkcnt++;
            }

            cruise = block->next;
        }
        if(blkcnt) _mmlog("Bad Blocks          : %d", blkcnt);

        if(type->children)
        {
            MM_ALLOC *child = type->children;
            _mmlog("Children types:");

            while(child)
            {   _mmlog(" - %s", child->name);
                child = child->next;
            }

            child = type->children;
            while(child)
            {   _mmalloc_reportchild(child, 4);
                child = child->next;
            }
        }

        _mmlog("--------------------------------------\n", type->name);
    }
}


// =====================================================================================
    static MM_ALLOC *createglobal(void)
// =====================================================================================
// Create an mmio allocation handle for allocating memory in groups/pages.
{
    MM_ALLOC   *type;

    if(!blockheap)
    {   blockheap = malloc(sizeof(MM_BLOCKINFO) * BLOCKHEAP_THRESHOLD);
        heapsize  = BLOCKHEAP_THRESHOLD;
        {   uint  i;
            MM_BLOCKINFO  *block = blockheap;
            for(i=0; i<heapsize; i++, block++) block->block = NULL;
        }
    }

    type = (MM_ALLOC *)calloc(1, sizeof(MM_ALLOC));
    strcpy(type->name, "globalalloc");
    type->flags  = MMAF_CHECK_OVERWRITES;

    type->blocklist = -1;

    return type;
}

#define myretval(b)    return &(b)->block[_MM_OVERWRITE_BUFFER]
#define myblockval(b)  &(b)->block[_MM_OVERWRITE_BUFFER]

// =====================================================================================
    static ULONG __inline *mymalloc(MM_ALLOC *type, uint size)
// =====================================================================================
{
#ifdef _DEBUG
    if(type->flags & MMAF_CHECK_OVERWRITES) _mm_checkallblocks(NULL);
#endif
    return (ULONG *)malloc((size + (_MM_OVERWRITE_BUFFER*2)) * 4);
}


// =====================================================================================
    static ULONG __inline *myrealloc(MM_ALLOC *type, ULONG *old_blk, uint size)
// =====================================================================================
{
#ifdef _DEBUG
    if(type->flags & MMAF_CHECK_OVERWRITES) _mm_checkallblocks(NULL);
#endif
    return (ULONG *)realloc(old_blk, (size + (_MM_OVERWRITE_BUFFER*2)) * 4);
}


// =====================================================================================
    static MM_BLOCKINFO *addblock(MM_ALLOC *type, ULONG *ptr, uint size, void (*deallocate)(void *block))
// =====================================================================================
{
    MM_BLOCKINFO *block;
    uint          i;

    for(i=lastblock, block = &blockheap[lastblock]; i<heapsize; i++, block++)
        if(!block->block) break;

    if(!block)
    {   for(i=0, block = blockheap; i<lastblock; i++, block++)
            if(!block->block) break;
    }

    lastblock = i+1;

    if(!block || (lastblock >= heapsize))
    {   
        // Woops! No open blocks.
        // ----------------------
        // Lets just make some more room, shall we?

        blockheap = realloc(blockheap, (heapsize+BLOCKHEAP_THRESHOLD) * sizeof(MM_BLOCKINFO));
        block     = &blockheap[i = heapsize];
        lastblock = heapsize+1;

        heapsize += BLOCKHEAP_THRESHOLD;

        {   uint  t;
            MM_BLOCKINFO  *cruise = &blockheap[lastblock];
            for(t=lastblock; t<heapsize; t++, cruise++) cruise->block = NULL;
        }

    }

    block->size       = size;
    block->deallocate = deallocate;
    block->block      = ptr;
    block->next       = type->blocklist;
    block->prev       = -1;

    if(type->blocklist != -1)
        blockheap[type->blocklist].prev = i;

    type->blocklist = i;

    return block;
}


// =====================================================================================
    static void removeblock(MM_ALLOC *type, MM_BLOCKINFO *block)
// =====================================================================================
// Deallocates the given block and removes it from the allocation list.
{
#ifdef _DEBUG
    if(type->flags & MMAF_CHECK_OVERWRITES) _mm_checkallblocks(type);
#endif

    if(block->prev != -1)
        blockheap[block->prev].next = block->next;
    else
        type->blocklist = block->next;

    if(block->next != -1) blockheap[block->next].prev = block->prev;

    // Should I call deallocate before or after it is removed from the list?
    // Does it matter?  Do you care?  Are you reading this?  AM I ALONE IN THIS WORLD!?
    // Will some woman... ANY WOMAN.. (who doesn't suck, anyway)... please!

    if(block->deallocate) block->deallocate(myblockval(block));

    free(block->block);
    //free(block);
}


// =====================================================================================
    static void fillblock(MM_BLOCKINFO *block, BOOL wipe)
// =====================================================================================
// Fills the underrun and overrun buffers with the magic databyte!
// if 'wipe' is set TRUE, then it works like calloc,a nd clears out the dataspace.
{
    ULONG *tptr = block->block;

#ifdef _DEBUG
    uint  i;
    for(i=0; i<_MM_OVERWRITE_BUFFER; i++, tptr++) *tptr = _MM_OVERWRITE_FILL;
#endif

    if(wipe) memset(tptr, 0, block->size*4);
    tptr+=block->size;

#ifdef _DEBUG
    for(i=0; i<_MM_OVERWRITE_BUFFER; i++, tptr++) *tptr = _MM_OVERWRITE_FILL;
#endif
}


// =====================================================================================
    MM_ALLOC *_mmalloc_create(const CHAR *name, MM_ALLOC *parent)
// =====================================================================================
// Create an mmio allocation handle for allocating memory in groups/pages.
{
    MM_ALLOC   *type;

    type = (MM_ALLOC *)calloc(1, sizeof(MM_ALLOC));
    strcpy(type->name, name);

#ifdef _DEBUG
    if(parent) type->flags = parent->flags | MMAF_CHECK_OVERWRITES;
#else
    if(parent) type->flags = parent->flags;
#endif

    if(!parent)
    {   if(!globalalloc) globalalloc = createglobal();
        parent = globalalloc;
    }

    type->next = parent->children;
    if(parent->children) parent->children->prev = type;
    parent->children = type;
    type->parent     = parent;
    type->blocklist = -1;

    return type;
}


// =====================================================================================
    void _mmalloc_setshutdown(MM_ALLOC *type, void (*shutdown)(void *block), void *block)
// =====================================================================================
{
    if(type)
    {   type->shutdown = shutdown;
        type->shutdata = block;
    }
}


// =====================================================================================
    void _mmalloc_freeall(MM_ALLOC *type)
// =====================================================================================
// deallocates all memory bound to the given allocation type, but does not close the
// handle - memory can be re-allocated to it at any time.
// Notes to self:
//  - should this deallocate children as well?
{
    if(type)
    {
        MM_ALLOC     *typec;
        int           cruise;

        //if(type->shutdown) type->shutdown(type->shutdata);

        _mmlogd1("_mmalloc_freeall > Unloading bunches of memory : \'%d\'",type->name);

        typec  = type->children;
        cruise = type->blocklist;

        /* Child Deallocation, Uncomment if I deem this more appropriate behavior
           at some point in the distant yet vaugely forseeable future.

        while(typec)
        {   MM_ALLOC  *cnext = typec->next;
            typec->parent = NULL;
            _mmalloc_close(typec);
            typec = cnext;
        }
        */

        while(cruise != -1)
        {   MM_BLOCKINFO *block = &blockheap[cruise];

#if (_MM_OVERWRITE_BUFFER != 0)
            if(type->flags & MMAF_CHECK_OVERWRITES) checkblock(type, block, TRUE);
#endif
            if(block->deallocate) block->deallocate(myblockval(block));

            if(block->block) free(block->block);
            lastblock = cruise;
            cruise = block->next;
        }

        type->blocklist = -1;

        //_mmlogd("               > Done!");
    }
}


// =====================================================================================
    void _mmalloc_closeall(void)
// =====================================================================================
// Pretty self-explainitory, as usual: closes *ALL* allocated memory.  For use during a
// complete shutdown or restart of the application.
{
    _mmalloc_close(globalalloc);
    globalalloc = NULL;
}


// =====================================================================================
    void _mmalloc_close(MM_ALLOC *type)
// =====================================================================================
// Shuts down the given alloc handle/type.  Frees all memory associated with the handle
// and optionally checks for buffer under/overruns (if they have been flagged).
{
    if(type)
    {
        MM_ALLOC     *typec;
        int           cruise;

        if(type->shutdown) type->shutdown(type->shutdata);

        _mmlogd1("_mmalloc_close > Unloading lotsa memory : '%s'",type->name);

        typec  = type->children;
        cruise = type->blocklist;

        while(typec)
        {   MM_ALLOC  *cnext = typec->next;
            typec->parent = NULL;
            _mmalloc_close(typec);
            typec = cnext;
        }

        while(cruise != -1)
        {   MM_BLOCKINFO *block = &blockheap[cruise];

#if (_MM_OVERWRITE_BUFFER != 0)
            if(type->flags & MMAF_CHECK_OVERWRITES) checkblock(type, block, TRUE);
#endif
            if(block->deallocate) block->deallocate(myblockval(block));

            if(block->block) free(block->block);
            lastblock = cruise;
            cruise = block->next;
        }

        if(type->parent)
        {
            if(type->prev)
                type->prev->next = type->next;
            else
                type->parent->children = type->next;

            if(type->next)
                type->next->prev = type->prev;

        }
        free(type);
        //_mmlogd("               > Done!");
    }
}


// =====================================================================================
    void *_mm_mallocx(MM_ALLOC *type, size_t size, void (*deinitialize)(void *block))
// =====================================================================================
{
    ULONG   *d;

    if(!type)
    {   if(!globalalloc) globalalloc = createglobal();
        type = globalalloc;
    }

    if(type->fptr_malloc)
        d = type->fptr_malloc(size);
    else
    {   size = (size/4)+1;
        d = mymalloc(type,size);

        if(!d)
        {   _mmlog(msg_fail, "_mm_malloc", size);
            _mmerr_set(MMERR_OUT_OF_MEMORY, msg_set);
        } else
        {   MM_BLOCKINFO *block = addblock(type, d, size, deinitialize);
            fillblock(block, FALSE);
            myretval(block);
        }
    }

    return d;
}


// =====================================================================================
    static MM_ALLOC *slowfree(MM_ALLOC *type, ULONG *d)
// =====================================================================================
// this is used internally only for recursive searching for a deallocated block in the
// event that the initial basic search for the block fails.  Removes the block in the event
// that it is found, and returns the alloctype it was found in (so we can report it to the
// logfile).  If not found, returns NULL!
{
    MM_ALLOC   *found = NULL;

    while(type && !found)
    {   int   cruise = type->blocklist;
        while((cruise != -1) && (blockheap[cruise].block != d)) cruise = blockheap[cruise].next;
        
        if(cruise != -1)
        {   removeblock(type, &blockheap[cruise]);
            lastblock = cruise;
            break;
        }

        found = slowfree(type->children, d);
        type = type->next;
    }

    return found ? found : type;
}


// =====================================================================================
    void _mm_a_free(MM_ALLOC *type, void **d)
// =====================================================================================
// Free the memory specified by the given pointer.  Checks the given type first and if
// not found there checks the entire hierarchy of memory allocations.  If the type is
// NULL, the all memory is checked reguardless.
{
    if(d && *d)
    {
        if(!type)
        {   if(!globalalloc) return;
            type = globalalloc;
        }

        if(type->fptr_free)
            type->fptr_free(*d);
        else
        {
            int    cruise;
            ULONG *dd = *d;

            dd -= _MM_OVERWRITE_BUFFER;

            // Note to Self / TODO
            // -------------------
            // I may want to change it so that free checks all children of the specified
            // blocktype without warning or error before resorting to a full search.
            // [...]

            cruise = type->blocklist;
            while((cruise != -1) && (blockheap[cruise].block != dd))
                cruise = blockheap[cruise].next;

            if(cruise != -1)
            {   removeblock(type, &blockheap[cruise]);
                lastblock = cruise;
            } else
            {
                MM_ALLOC    *ctype;

                // Check All Memory Regions
                // ------------------------
                // This is a very stimple process thanks to some skilled use of recursion!

                ctype = slowfree(globalalloc->children, dd);

                if(ctype)
                {   _mmlog("_mm_free > (Warning) Freed block was outside specified alloctype!\n"
                           "         > (Warning) Type specified: \'%s\'\n"
                           "         > (Warning) Found in      : \'%s\'", type->name, ctype->name);

                    assert(FALSE);
                } else
                {   _mmlog("** _mm_free > Specified block not found! (type %s, address = %d)", type->name, dd);
                    assert(FALSE);
                }
            }

            *d = NULL;
        }
    }
}


// =====================================================================================
    void *_mm_callocx(MM_ALLOC *type, size_t nitems, size_t size, void (*deallocate)(void *block))
// =====================================================================================
{
    ULONG *d;

    if(!type)
    {   if(!globalalloc) globalalloc = createglobal();
        type = globalalloc;
    }

    if(type->fptr_calloc)
    {   d = type->fptr_calloc(nitems, size);
    } else
    {   
        // The purpose of calloc : to save others the work of the multiply!

        size = nitems * size;
        size = (size/4)+1;

        d = mymalloc(type,size);
        if(d)
        {   MM_BLOCKINFO *block = addblock(type, d, size, deallocate);
            fillblock(block, TRUE);
            myretval(block);
        } else
        {   _mmlog(msg_fail, "_mm_calloc", size);
            _mmerr_set(MMERR_OUT_OF_MEMORY, msg_set);
        }
    }

    return d;
}


// =====================================================================================
    void *_mm_reallocx(MM_ALLOC *type, void *old_blk, size_t size, void (*deallocate)(void *block))
// =====================================================================================
{
    ULONG *d;    

    if(!type)
    {   if(!globalalloc) globalalloc = createglobal();
        type = globalalloc;
    }

    if(type->fptr_realloc)
        d = type->fptr_realloc(old_blk, size);
    else
    {
        MM_BLOCKINFO   *block;

        size = (size/4)+1;

        if(old_blk)
        {
            int             cruise;
            ULONG          *dd = old_blk;

            dd -= _MM_OVERWRITE_BUFFER;

            cruise = type->blocklist;
            while((cruise != -1) && (blockheap[cruise].block != dd))
                cruise = blockheap[cruise].next;

            if(cruise != -1)
            {   d = myrealloc(type, dd, size);

                block = &blockheap[cruise];

                block->size       = size;
                block->block      = d;
                block->deallocate = deallocate;
            } else
            {   _mmlog("** _mm_realloc > Block not found...");
                assert(FALSE);
            }
        } else
        {   d = mymalloc(type,size);
            block = addblock(type, d, size, deallocate);
        }

        if(d)
        {   fillblock(block, FALSE);
            myretval(block);
        } else
        {   _mmlog(msg_fail, "_mm_realloc", size);
            _mmerr_set(MMERR_OUT_OF_MEMORY, msg_set);
        }
    }

    return d;
}


// =====================================================================================
    void *_mm_recallocx(MM_ALLOC *type, void *old_blk, size_t nitems, size_t size, void (*deallocate)(void *block))
// =====================================================================================
// My own special reallocator, which works like calloc by allocating memory by both block-
// size and numitems.  However, due to the reason that I do not internally track the size
// of allocations, I cannot clear the new portion like I'd like to.
// Oh well.. Maybe later! ;)
{
    ULONG   *d;

    if(!type)
    {   if(!globalalloc) globalalloc = createglobal();
        type = globalalloc;
    }

    if(type->fptr_recalloc)
    {   d = type->fptr_recalloc(old_blk, nitems, size);
    } else
    {
        MM_BLOCKINFO   *block;
        // The purpose of calloc : to save others the work of the multiply!

        size = nitems * size;
        size = (size/4)+1;

        if(old_blk)
        {
            int             cruise;
            ULONG          *dd = old_blk;

            dd -= _MM_OVERWRITE_BUFFER;

            cruise = type->blocklist;
            while((cruise != -1) && (blockheap[cruise].block != dd))
                cruise = blockheap[cruise].next;

            if(cruise != -1)
            {   d = myrealloc(type, dd, size);

                block = &blockheap[cruise];

                if(block->size < size)
                {   ULONG   *dd = (ULONG *)d;
                    memset(&dd[block->size], 0, (size - block->size)*4);
                } else
                    memset(d, 0, size*4);

                block->size       = size;
                block->block      = d;
                block->deallocate = deallocate;
            } else
            {   _mmlog("** _mm_recalloc > Block not found...");
                assert(FALSE);
            }
        } else
        {   d = mymalloc(type,size);
            block = addblock(type, d, size, deallocate);
        }

        if(d)
        {   fillblock(block, FALSE);
            myretval(block);
        } else
        {   _mmlog(msg_fail, "_mm_recalloc", size);
            _mmerr_set(MMERR_OUT_OF_MEMORY, msg_set);
        }
    }

    return d;
}
