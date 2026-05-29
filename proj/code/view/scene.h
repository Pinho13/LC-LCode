#pragma once

#include <lcom/lcf.h>

typedef enum {
  SCENE_EDITOR
} SceneID;

int scene_init(SceneID id);
void scene_cleanup();
int scene_get_vis_rows();

void view_render();
