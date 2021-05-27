#include "cache.h"

#include "strbuf.h"
#include "trace2.h"

char *get_process_name(int pid)
{
#ifdef HAVE_PROCFS_LINUX
	struct strbuf procfs_path = STRBUF_INIT;
	struct strbuf out = STRBUF_INIT;
	/* try to use procfs if it's present. */
	strbuf_addf(&procfs_path, "/proc/%d/comm", pid);
	if (!strbuf_read_file(&out, procfs_path.buf, 0)) {
		/* All done with file reads, clean up early */
		strbuf_release(&procfs_path);
		return strbuf_detach(&out, NULL);
	}
#endif

	/* NEEDSWORK: add non-procfs implementations here. */
	return NULL;
}

void trace2_collect_process_info(enum trace2_process_info_reason reason)
{
	if (!trace2_is_enabled())
		return;

	/* someday we may want to write something extra here, but not today */
	if (reason == TRACE2_PROCESS_INFO_EXIT)
		return;

	if (reason == TRACE2_PROCESS_INFO_STARTUP) {
		/*
		 * NEEDSWORK: we could do the entire ptree in an array instead,
		 * see compat/win32/trace2_win32_process_info.c.
		 */
		char *names[2];
		names[0] = get_process_name(getppid());
		names[1] = NULL;

		if (!names[0])
			return;

		trace2_cmd_ancestry((const char**)names);

		free(names[0]);
	}

	return;
}
