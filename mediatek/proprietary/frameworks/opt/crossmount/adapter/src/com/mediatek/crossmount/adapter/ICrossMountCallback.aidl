package com.mediatek.crossmount.adapter;

oneway interface ICrossMountCallback
{
  void onFeatureStateUpdate(String featureState);
  void onDiscoverResultUpdate(int reason);
  void onConnectionResultUpdate();
  void onTrustDeviceUpdated();
  void onMyProviderServicesUpdate();
  void onPlugInServiceIconReady();
}
