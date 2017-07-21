package com.mediatek.engineermode.mbim;

/**
 * Java JNI interface for Audio.
 *
 */
public class MBIMJni {

    static {
        System.loadLibrary("em_mbim_jni");
    }


    /**
     * @return result of start service
     */
    public static native int startService();

    /**
     * @return result of stop service
     */
    public static native int stopService();


}
