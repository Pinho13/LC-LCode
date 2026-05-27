#include <lcom/lcf.h>

#include "fw/common/utils.h"

#include "controller/ih/ih.h"


int(proj_main_loop)(int argc, char *argv[]) {
  int ipc_status, r;
  message msg;
  
  if (subscribe_interrupts() != OK)
    return fail(ERR, "proj_main_loop: unable to subscribe interrupts");

  while (1)
  {
    if ((r = driver_receive(ANY, &msg, &ipc_status))!= OK) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          printf("a");
          break;
        default:
          break;
      }
    }
  }

  if (unsubscribe_interrupts() != OK)
    return fail(ERR, "proj_main_loop: unable to unsubscribe interrupts");
  
  return 0;
}

int(main)(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/framework/trace.txt");
  lcf_log_output("/home/lcom/labs/framework/output.txt");
  
  if (lcf_start(argc, argv) != OK) {
    lcf_cleanup();
    return 1;
  }

  lcf_cleanup();
  return 0;
}
