#include "game.h"

#include <orb/core/event.h>
#include <orb/core/input.h>
#include <orb/core/logger.h>
#include <orb/core/orb_memory.h>
#include <orb/math/orb_math.h>

// HACK: remove this
#include <orb/renderer/renderer_frontend.h>

const f32 near_clip = 0.01f;
const f32 far_clip = 80.0f;

b8 initialize(orb_game *game_instance) {
    (void)game_instance;

    return true;
}

void recalculate_view_matrix(game_state *state) {
    // pitch, yaw, roll
    orb_mat4 rotation = orb_mat4_from_euler_rotation_xyz(
        state->camera_euler.x, state->camera_euler.y, state->camera_euler.z);

    orb_mat4 translation = orb_mat4_from_translation(state->camera_position);

    state->camera_transform = orb_mat4_mul(&rotation, &translation);
    state->camera_transform = orb_mat4_inverse(&state->camera_transform);
}

void camera_yaw(game_state *state, f32 amount) { state->camera_euler.y += amount; }
void camera_pitch(game_state *state, f32 amount) {
    state->camera_euler.x += amount;

    f32 limit = orb_degrees_to_radians(89.0f);
    state->camera_euler.x = ORB_CLAMP(state->camera_euler.x, -limit, limit);
}

b8 update(orb_game *game_instance, f32 delta) {
    game_state *state = (game_state *)(game_instance)->state;
    (void)delta;

    if (orb_input_is_key_pressed(KEY_ESCAPE)) {
        orb_event_send(ORB_EVENT_APPLICATION_QUIT, nullptr, (orb_event_context){0});
        return true;
    }

    // Mouse Rotation
    const f32 sensitivity = 1.0f / 1000.0f;
    i32 x, y;
    orb_input_get_mouse_delta(&x, &y);
    camera_pitch(state, -(f32)y * sensitivity);
    camera_yaw(state, -(f32)x * sensitivity);

    orb_mat4 identity = state->camera_transform;

    // Movement
    orb_vec3 direction = orb_vec3_zero();

    if (orb_input_is_key_down(KEY_W)) {
        direction = orb_vec3_add(direction, orb_mat4_forward(&identity));
    }
    if (orb_input_is_key_down(KEY_S)) {
        direction = orb_vec3_add(direction, orb_mat4_backward(&identity));
    }

    // TODO: investigate why this is swapped
    if (orb_input_is_key_down(KEY_A)) {
        direction = orb_vec3_add(direction, orb_mat4_right(&identity));
    }
    if (orb_input_is_key_down(KEY_D)) {
        direction = orb_vec3_add(direction, orb_mat4_left(&identity));
    }

    // TODO: investigate why this is swapped
    if (orb_input_is_key_down(KEY_SPACE)) {
        direction.y -= 1.0f;
    }
    if (orb_input_is_key_down(KEY_LSHIFT)) {
        direction.y += 1.0f;
    }

    f32 magnitude = orb_vec3_magnitude(direction);
    if (magnitude > ORB_FLOAT_EPSILON) {
        state->camera_position =
            orb_vec3_add(state->camera_position, orb_vec3_scale(direction, 1.0f / magnitude));
    }

    // set camera matrix
    recalculate_view_matrix(state);
    orb_renderer_set_camera(&state->camera_transform, state->camera_fov, near_clip, far_clip);

    return true;
}

b8 render(orb_game *game_instance, f32 delta) {
    (void)game_instance;
    (void)delta;
    return true;
}

void on_resize(orb_game *game_instance, u16 width, u16 height) {
    (void)game_instance;
    (void)width;
    (void)height;
}

void shutdown(orb_game *game) { orb_free(game->state, sizeof(game_state), MEMORY_TAG_GAME); }
