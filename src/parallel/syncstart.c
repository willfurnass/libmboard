/* $Id: syncstart.c 1930 2009-07-02 13:47:53Z lsc $ */
/*!
 * \file parallel/syncstart.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_SyncStart()
 * 
 */
#include "mb_parallel.h"
#include "mb_syncqueue.h"

/*!
 * \brief Synchronises the content of the board across all processes
 * \ingroup MB_API
 * \param[in] mb MessageBoard Handle
 * 
 * Lock the board and submit a Sync Request to the communication thread.
 * 
 * We do not capture \c syncLock since we assume that up to this point, only
 * the main thread will be accessing this board.
 * 
 * Synchronisation of a null board (::MB_NULL_MBOARD) is valid, and will 
 * return immediately with ::MB_SUCCESS
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID  (Invalid board)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 * 
 */
int MB_SyncStart(MBt_Board mb) {

    MBIt_Board *board;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) 
    {
        P_FUNCFAIL("Cannot sync null board (MB_NULL_MBOARD)");
        return MB_SUCCESS;
    }
      
    /* get object mapped to mb handle */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL) 
    {
        P_FUNCFAIL("Unknown board handle (%d)", (int)mb);
        return MB_ERR_INVALID;
    }
    
    /* Check if board is locked */
    if (board->locked == MB_TRUE) 
    {
        P_FUNCFAIL("Board is locked");
        return MB_ERR_LOCKED;
    }
    
    /* lock the board */
    board->locked = MB_TRUE;
    
    /* nothing to do if we're not running in parallel */
    if (MBI_CommSize == 1)
    {
        P_INFO("Nothing to sync (single proc run). Marked as completed");
        board->syncCompleted = MB_TRUE;
        return MB_SUCCESS;
    }
    
    board->syncCompleted = MB_FALSE;
     
    /* Submit sync request to comm thread */
    return MBI_SyncQueue_Push(mb);
}
