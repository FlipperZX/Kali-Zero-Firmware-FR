#include "../infrared_app_i.h"

static void infrared_scene_edit_button_select_submenu_callback(void* context, uint32_t index) {
    InfraredApp* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_edit_button_select_on_enter(void* context) {
    InfraredApp* infrared = context;
    Submenu* submenu = infrared->submenu;
    InfraredRemote* remote = infrared->remote;
    InfraredAppState* app_state = &infrared->app_state;

    const char* header = infrared->app_state.edit_mode == InfraredEditModeRename ?
                             "Renommer Bouton:" :
                             "Supprimer Bouton:";
    submenu_set_header(submenu, header);

    const size_t button_count = infrared_remote_get_signal_count(remote);
    for(size_t i = 0; i < button_count; ++i) {
        submenu_add_item(
            submenu,
            infrared_remote_get_signal_name(remote, i),
            i,
            infrared_scene_edit_button_select_submenu_callback,
            context);
    }

    if(button_count && app_state->current_button_index != InfraredButtonIndexNone) {
        submenu_set_selected_item(submenu, app_state->current_button_index);
        app_state->current_button_index = InfraredButtonIndexNone;
    }

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewSubmenu);
}

bool infrared_scene_edit_button_select_on_event(void* context, SceneManagerEvent event) {
    InfraredApp* infrared = context;
    InfraredAppState* app_state = &infrared->app_state;
    SceneManager* scene_manager = infrared->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        app_state->current_button_index = event.event;
        const InfraredEditMode edit_mode = app_state->edit_mode;
        if(edit_mode == InfraredEditModeRename) {
            scene_manager_next_scene(scene_manager, InfraredSceneEditRename);
        } else if(edit_mode == InfraredEditModeDelete) {
            scene_manager_next_scene(scene_manager, InfraredSceneEditDelete);
        } else {
            furi_crash();
        }
        consumed = true;
    }

    return consumed;
}

void infrared_scene_edit_button_select_on_exit(void* context) {
    InfraredApp* infrared = context;
    submenu_reset(infrared->submenu);
}
