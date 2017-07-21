#ifndef MTK_HWC_PRIV_H
#define MTK_HWC_PRIV_H

#include <hardware/hwcomposer.h>

__BEGIN_DECLS

/*****************************************************************************/

typedef struct hwc_feature {
    int externals;
    int virtuals;
    int enhance;
#if defined(MTK_HWC_VER_1_2)
    int stereoscopic;
#else
    int copyvds;
    int svp;
    int phy_rotation_180;
    int cache_caps;
    int cache_mode;
    int rotation_offset;
    int gmo;
    int control_fb;
    int trigger_by_vsync;
    int fbt_bound;
#endif
    int od;
    int fps120;
    int hdmi_s3d;
    int hdmi_s3d_debug;
    int hdmi_s3d_depth;
} hwc_feature_t;


typedef struct hwc_layer_ext_info
{
    int buffer_crop_width;
    int buffer_crop_height;
} hwc_layer_ext_info_t;

// platform support cache feature capability
enum {
    HWC_FEATURES_CACHE_CAPS_GPU_PASSIVE = 0x00000001,

    HWC_FEATURES_CACHE_CAPS_GPU_ACTIVE = 0x00000002,

    HWC_FEATURES_CACHE_CAPS_OVL_ACTIVE = 0x00000004,
};

// determine whether cache feature is on or not
enum {
    HWC_FEATURES_CACHE_MODE_GPU_PASSIVE = 0x00000001,

    HWC_FEATURES_CACHE_MODE_GPU_ACTIVE = 0x00000002,

    HWC_FEATURES_CACHE_MODE_OVL_ACTIVE = 0x00000004,
};

// HWC_IS_S3D_LAYER is set by hwcomposer to indicate that this layer is used
// as a S3D display, and process this layer in S3D flow
enum {
    HWC_IS_S3D_LAYER = 0x00000100,

    HWC_IS_S3D_LAYER_SBS = 0x00000200,

    HWC_IS_S3D_LAYER_TAB = 0x00000400
};

enum {
    // Availability: MTK specific
    // Returns MTK platform features
    HWC_FEATURES_STATE = 100,

    HWC_NUM_EXTRA_BUFFER = 101,
};

/*
 * hwc_layer_t::hints values extension
 * Need to check if these values are conflict to original AOSP
 */
enum {
    /*
     * HWC sets HWC_HINT_SECURE_COMP to tell SurfaceFlinger that
     * this layer is handled by HWC with secure buffer
     */
    HWC_HINT_SECURE_COMP = 0x10000000,
};

/*
 * hwc_layer_1_t::flags values extension
 * Need to check if these values are conflict to original AOSP
 */
enum {
    /* HWC_SECURE_LAYER is set by SurfaceFlinger to indicat that the HAL
     * this layer is secure
     */
    HWC_SECURE_LAYER    = 0x80000000,

    /*
     * HWC_DIRTY_LAYER is set by SurfaceFlinger to indicate that the HAL
     * this layer has updated content.
     */
    HWC_DIRTY_LAYER     = 0x20000000,

    /*
     * HWC_DIM_LAYER is set by SurfaceFlinger to indicate that the HAL
     * this layer is dim
     */
    HWC_DIM_LAYER       = 0x40000000,
};

/*
 * hwc_display_contents_1_t::flags values extension
 * Need to check if these values are conflict to original AOSP
 */
enum {
#if defined(MTK_HWC_VER_1_2)
    HWC_MIRROR_DISPLAY        = 0x80000000,
#else
    HWC_MIRROR_DISPLAY        = 0x20000000,
#endif

    // HWC_SKIP_DISPLAY is set by SurfaceFlinger to indicate that
    // the composition can be skipped this time
    HWC_SKIP_DISPLAY          = 0x40000000,

    // HWC_MIRRORED_DISP_MASK is using second byte for checking mirror display
    HWC_MIRRORED_DISP_MASK    = 0x0000FF00,

    // HWC_ORIENTATION_MASK is using third byte for external orientation
    // Added for HWC_DEVICE_API_VERSION_1_0
    HWC_ORIENTATION_MASK      = 0x00FF0000,
};

/* more display attributes returned by getDisplayAttributes() */
enum {
    HWC_DISPLAY_SUBTYPE = 6,
};

/* Display subtypes */
enum {
    HWC_DISPLAY_LCM       = 0,
    HWC_DISPLAY_HDMI_MHL  = 1,
    HWC_DISPLAY_SMARTBOOK = 2,
    HWC_DISPLAY_MEMORY    = 3,
    HWC_DISPLAY_WIRELESS  = 4,
};

/*****************************************************************************/

__END_DECLS

#endif /* MTK_HWC_PRIV_H */
