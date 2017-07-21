package com.mediatek.smartmotion;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.preference.PreferenceFragment;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Switch;

import com.mediatek.smartmotion.enabler.QuickAnswerEnabler;
import com.mediatek.smartmotion.utils.LogUtils;

public class QuickAnswerFragment extends ParentFragment {
    private static final String TAG = "QuickAnswerFragment";

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        LogUtils.d(TAG, "onActivityCreated");
        QuickAnswerEnabler.registerSwitch(mActivity, mActionBarSwitch);
    }

    @Override
    public void onDestroy() {
        LogUtils.d(TAG, "onDestroy");
        QuickAnswerEnabler.unregisterSwitch(mActionBarSwitch);
        super.onDestroy();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        LogUtils.d(TAG, "onCreateView");
        View view = inflater.inflate(R.layout.quick_answer_fragment, container, false);
        return view;
    }

}
