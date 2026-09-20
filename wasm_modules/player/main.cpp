#include <player.h>
#include <Elementary.h>
#include <app.h>
#include <dlog.h>
#include <string.h>

#define LOG_TAG "TIZEN_PLAYER"

static player_h g_player = nullptr;
static Evas_Object *g_video = nullptr;

static const char *HLS_URL =
    "https://iptv.mobilkod.com/live/test/test/1.m3u8";


static void on_player_prepared(
    player_h player,
    void *user_data)
{
    (void)user_data;

    dlog_print(
        DLOG_INFO,
        LOG_TAG,
        "PLAYER PREPARED"
    );

    int ret = player_start(player);

    dlog_print(
        ret == PLAYER_ERROR_NONE ? DLOG_INFO : DLOG_ERROR,
        LOG_TAG,
        "player_start() = %d",
        ret
    );
}


static void on_player_completed(
    player_h player,
    void *user_data)
{
    (void)player;
    (void)user_data;

    dlog_print(
        DLOG_INFO,
        LOG_TAG,
        "PLAYER COMPLETED"
    );
}


static void on_player_error(
    player_h player,
    int error_code,
    void *user_data)
{
    (void)player;
    (void)user_data;

    dlog_print(
        DLOG_ERROR,
        LOG_TAG,
        "PLAYER ERROR: %d",
        error_code
    );
}


static void on_player_buffering(
    player_h player,
    int percent,
    void *user_data)
{
    (void)player;
    (void)user_data;

    dlog_print(
        DLOG_INFO,
        LOG_TAG,
        "BUFFERING: %d%%",
        percent
    );
}


static void destroy_player()
{
    if (g_player != nullptr) {

        dlog_print(
            DLOG_INFO,
            LOG_TAG,
            "Destroying player"
        );

        player_unprepare(g_player);
        player_destroy(g_player);

        g_player = nullptr;
    }

    g_video = nullptr;
}


static int create_player(Evas_Object *video)
{
    if (video == nullptr) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "video object is NULL"
        );

        return PLAYER_ERROR_INVALID_PARAMETER;
    }

    g_video = video;

    int ret = player_create(&g_player);

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_create() failed: %d",
            ret
        );

        g_player = nullptr;
        return ret;
    }


    /*
     * Error callback
     */
    ret = player_set_error_cb(
        g_player,
        on_player_error,
        nullptr
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_set_error_cb() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    /*
     * Completed callback
     */
    ret = player_set_completed_cb(
        g_player,
        on_player_completed,
        nullptr
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_set_completed_cb() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    /*
     * Buffering callback
     */
    ret = player_set_buffering_cb(
        g_player,
        on_player_buffering,
        nullptr
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_set_buffering_cb() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    /*
     * Display
     *
     * IMPORTANT:
     *
     * PLAYER_DISPLAY_TYPE_OVERLAY için
     * native display/window handle gerekir.
     *
     * Eğer video bir Evas_Object ise,
     * GET_DISPLAY(video) yalnızca senin
     * projende tanımlıysa kullanılmalıdır.
     *
     * Aşağıdaki satır mevcut GET_DISPLAY()
     * tanımını kullanır.
     */
    ret = player_set_display(
        g_player,
        PLAYER_DISPLAY_TYPE_OVERLAY,
        GET_DISPLAY(video)
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_set_display() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    /*
     * Full screen
     */
    ret = player_set_display_mode(
        g_player,
        PLAYER_DISPLAY_MODE_FULL_SCREEN
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_set_display_mode() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    /*
     * HLS URL
     */
    ret = player_set_uri(
        g_player,
        HLS_URL
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_set_uri() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    /*
     * Prepare asynchronously.
     */
    ret = player_prepare_async(
        g_player,
        on_player_prepared,
        nullptr
    );

    if (ret != PLAYER_ERROR_NONE) {
        dlog_print(
            DLOG_ERROR,
            LOG_TAG,
            "player_prepare_async() failed: %d",
            ret
        );

        destroy_player();
        return ret;
    }


    dlog_print(
        DLOG_INFO,
        LOG_TAG,
        "PLAYER CREATED"
    );

    return PLAYER_ERROR_NONE;
}
