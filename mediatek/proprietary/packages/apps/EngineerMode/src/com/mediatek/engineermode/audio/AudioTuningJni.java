package com.mediatek.engineermode.audio;

/**
 * Java JNI interface for Audio.
 *
 */
public class AudioTuningJni {

    static {
        System.loadLibrary("em_audio_jni");
    }

    /**
     * @param param1 category
     * @param param2 type
     * @return detail category type
     */
    public static native String getCategory(String param1, String param2);
    /**
     * @param param1 category
     * @param param2 configuration
     * @param param3 tag for detail information
     * @return detail parameters
     */
    public static native String getParams(String param1, String param2, String param3);

    /**
     * @param param1 category
     * @param param2 configuration
     * @param param3 tag for detail information
     * @return detail list
     */
    public static native String getChecklist(String param1, String param2, String param3);
    /**
     * @param param1 category
     * @param param2 configuration
     * @param param3 tag for detail information
     * @param param4 value to set
     * @return true for success, false for fail
     */
    public static native boolean setParams(String param1, String param2,
            String param3, String param4);
    /**
     * @param param category
     * @return true for success, false for fail
     */
    public static native boolean saveToWork(String param);
    /**
     * @return true for success, false for fail
     */
    public static native boolean registerXmlChangedCallback();

}
