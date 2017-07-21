/**
 * {@hide}
 */
package com.mediatek.wfo;

interface IWifiOffloadListener {
    void onHandover(in int stage, in int ratType);
    void onRoveOut(in boolean roveOut, in int rssi);
}
