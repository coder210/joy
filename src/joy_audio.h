/******************************************************************************
 * src/joy_audio.h —— audio 域声明头（joy_audio.c 的配套头）
 * 职责：音频
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_AUDIO_IMPL_H
#define JOY_AUDIO_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        /* ===== 运行与生命周期管理 ===== */
        joy_bt_status_t joy_scene_bt_node_run(joy_bt_node_t* node, float dt);
        void        joy_scene_bt_node_reset(joy_bt_node_t* node);
        void        joy_scene_bt_node_destroy(joy_bt_node_t* node);
        /* Action: 执行动作，返回 SUCCESS / FAILURE / RUNNING */
        joy_bt_node_t* joy_scene_bt_action_create(joy_bt_action_fn fn);
        joy_bt_node_t* joy_scene_bt_action_createv(joy_bt_action_fn fn, void* userdata);
        void joy_scene_bt_action_set_userdata(joy_bt_node_t* node, void* userdata);
        void* joy_scene_bt_action_get_userdata(joy_bt_node_t* node);
        /* Condition: 条件判断，true → SUCCESS, false → FAILURE */
        joy_bt_node_t* joy_scene_bt_condition_create(joy_bt_condition_fn pred);
        /* Wait: 等待指定秒数 */
        joy_bt_node_t* joy_scene_bt_wait_create(float seconds);
        /* ===== 组合节点 ===== */
        /* Sequence: 顺序执行全部子节点，全部成功才返回 SUCCESS */
        joy_bt_node_t* joy_scene_bt_sequence_create(joy_bt_node_t** children, int count);
        /* Selector: 顺序尝试子节点，任一成功即返回 SUCCESS */
        joy_bt_node_t* joy_scene_bt_selector_create(joy_bt_node_t** children, int count);
        /* Parallel: 并行执行，success_count 个成功即成功 */
        joy_bt_node_t* joy_scene_bt_parallel_create(joy_bt_node_t** children, int count, int success_count);
        /* Inverter: 反转子节点结果 */
        joy_bt_node_t* bt_inverter_create(joy_bt_node_t* child);
        /* Succeeder: 总是返回 SUCCESS */
        joy_bt_node_t* bt_succeeder_create(joy_bt_node_t* child);
        /* Repeater: 重复子节点 times 次（-1 无限） */
        joy_bt_node_t* bt_repeater_create(joy_bt_node_t* child, int times);
        joy_bt_node_t* joy_scene_bt_statemachine_create(joy_bt_state_slot_t* states, int count, const char* initial);
        void       joy_scene_bt_statemachine_transition(joy_bt_node_t* node, const char* state_name);


        JOY_API uint32_t joy_audio_open_device();
        JOY_API void joy_audio_close_device(uint32_t device_id);
        JOY_API bool joy_audio_create_wav(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_mp3(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_flac(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_ogg(const char* filepath, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create(const char* wav_path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_mp3(const char* path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_flac(const char* path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_ogg(const char* path, uint32_t device_id);
        JOY_API void joy_audio_shot_play(joy_audio_shot_p shot);
        JOY_API void joy_audio_shot_set_gain(joy_audio_shot_p shot, float gain);
        JOY_API void joy_audio_shot_destroy(joy_audio_shot_p shot);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create(const char* wav_path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_mp3(const char* path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_flac(const char* path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_ogg(const char* path, uint32_t device_id);
        JOY_API void joy_audio_bgm_stop(joy_audio_bgm_p bgm);
        JOY_API void joy_audio_bgm_set_gain(joy_audio_bgm_p bgm, float gain);
        JOY_API void joy_audio_bgm_destroy(joy_audio_bgm_p bgm);

#ifdef __cplusplus
}
#endif

#endif /* JOY_AUDIO_IMPL_H */
