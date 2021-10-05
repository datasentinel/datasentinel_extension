#include "postgres.h"
#include "storage/ipc.h"
#include "storage/proc.h"
#include "miscadmin.h"
#include "access/twophase.h"
#include "parser/analyze.h"
#include "parser/scansup.h"
#include "access/hash.h"

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(datasentinel_queryid);

/* Entry point of library loading */
void _PG_init(void);
void _PG_fini(void);

/* Saved hook values in case of unload */
static shmem_startup_hook_type datasentinel_prev_shmem_startup_hook = NULL;
static post_parse_analyze_hook_type prev_post_parse_analyze_hook = NULL;

/* Our hooks */
static void datasentinel_shmem_startup(void);
#if PG_VERSION_NUM >= 140000
static void ash_post_parse_analyze(ParseState *pstate, Query *query, JumbleState *jstate);
#else
static void ash_post_parse_analyze(ParseState *pstate, Query *query)
#endif

typedef struct procEntry
{
	uint64 queryid;
} procEntry;

static procEntry *ProcEntryArray = NULL;
static int get_max_procs_count(void);

static int
get_max_procs_count(void)
{
	int count = 0;

	count += MaxBackends;
	count += NUM_AUXILIARY_PROCS;
	count += max_prepared_xacts;

	return count;
}

#if PG_VERSION_NUM >= 140000
static void ash_post_parse_analyze(ParseState *pstate, Query *query, JumbleState *jstate)
#else
static void ash_post_parse_analyze(ParseState *pstate, Query *query)
#endif
{

	if (prev_post_parse_analyze_hook)
	    #if PG_VERSION_NUM >= 140000
        prev_post_parse_analyze_hook(pstate, query, jstate);
        #else
		prev_post_parse_analyze_hook(pstate, query);
		#endif
	if (MyProc)
	{
		int i = MyProc - ProcGlobal->allProcs;
		#if PG_VERSION_NUM >= 110000
				if (query->queryId != UINT64CONST(0)) {
					ProcEntryArray[i].queryid = query->queryId;
		#else
				if (query->queryId != 0) {
					ProcEntryArray[i].queryid = query->queryId;
		#endif
				} else {
					ProcEntryArray[i].queryid = 0;
				}
	}
}

static void
datasentinel_shmem_startup(void)
{
	int size;
	bool   found;
	
	if (datasentinel_prev_shmem_startup_hook)
		datasentinel_prev_shmem_startup_hook();

	LWLockAcquire(AddinShmemInitLock, LW_EXCLUSIVE);

	size = mul_size(sizeof(procEntry), get_max_procs_count());
	ProcEntryArray = (procEntry *) ShmemInitStruct("Datasentinel Proc Entry Array", size, &found);
	if (!found)
	{
		MemSet(ProcEntryArray, 0, size);
	}

	LWLockRelease(AddinShmemInitLock);

	if (found)
		return;

}

void
_PG_init(void)
{

	if (!process_shared_preload_libraries_in_progress)
		return;

	datasentinel_prev_shmem_startup_hook = shmem_startup_hook;
	shmem_startup_hook = datasentinel_shmem_startup;
	prev_post_parse_analyze_hook = post_parse_analyze_hook;
	post_parse_analyze_hook = ash_post_parse_analyze;
}

Datum
datasentinel_queryid(PG_FUNCTION_ARGS)
{
	int i;

	for (i = 0; i < ProcGlobal->allProcCount; i++)
	{
		PGPROC  *proc = &ProcGlobal->allProcs[i];
		if (proc != NULL && proc->pid != 0 && proc->pid == PG_GETARG_INT32(0))
		{
			return ProcEntryArray[i].queryid;
		}
	}
	return 0;
}

void
_PG_fini(void)
{
	shmem_startup_hook = datasentinel_prev_shmem_startup_hook;
	post_parse_analyze_hook = prev_post_parse_analyze_hook;
}
