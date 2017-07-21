package com.mediatek.blf;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Window;


public class BlueLightFilterActivity extends Activity{
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        startMiraColorService();
        finish();
    }

    public void startMiraColorService() {
		Intent show = new Intent(this, BlueLightFilter.class);
        startService(show);

	}

}