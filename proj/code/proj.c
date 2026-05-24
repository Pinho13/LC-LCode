#include <lcom/lcf.h>

int(proj_main_loop)(int argc, char *argv[]) {
  
  return 0;
}

int(main)(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/framework/trace.txt");
  lcf_log_output("/home/lcom/labs/framework/output.txt");
  
  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}
