package com.mediatek.blf;

import android.R.integer;
import android.R.string;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.TextureView;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.app.Notification;

public class BlueLightFilter extends Service {
	private static final String TAG = "BlueLightFilter";
	private static WindowManager wm;
	private static WindowManager.LayoutParams params;
    private View view;
    private int mStrenght;
    private int mStrenghtBig;
    private SeekBar mSeekBar;
    private Switch mSwitchButton;
    private CheckBox mCheckBox;
    private TextView mPercent ;
    private String mStringPercent;
    public boolean blf_enabled;
	protected native boolean nativeSetBlueLightFilterStrength(int strength);
	protected native int nativeGetBlueLightFilterStrength();
	protected native boolean nativeEnableBlueLightFilter(boolean enable);
	protected native boolean nativeIsBlueLightFilterEnabled();
	protected native int nativeGetBlueLightFilterStrengthRange();
	protected native boolean nativeBlueLightFilterInit();

	static {
		System.loadLibrary("jni_blf");
	};
	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		mStrenghtBig = nativeGetBlueLightFilterStrengthRange();
		createFloatView();
		nativeBlueLightFilterInit();
	}

	@Override
	public int onStartCommand(Intent intent, int flag, int startId) {
		Log.d(TAG, "onStartCommand intent = " + intent);
		Log.d(TAG, "onStartCommand flag = " + flag);
		Log.d(TAG, "onStartCommand startId = " + startId);
		Notification notification = new Notification();
		notification.flags |= Notification.FLAG_HIGH_PRIORITY;
        startForeground(startId, notification);
		return super.onStartCommand(intent, flag, flag);
	}
	
	@Override
	public void onDestroy() {
		super.onDestroy();
	}

	private void createFloatView() {
    	LayoutInflater layoutInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        view = layoutInflater.inflate(R.layout.color_bar, null);
		initUI();
        wm = (WindowManager) getApplicationContext()
        	.getSystemService(Context.WINDOW_SERVICE);
        params = new WindowManager.LayoutParams();

        params.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;

       params.format = PixelFormat.TRANSLUCENT; 

        params.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                              | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
        params.width = 600;
        params.height = 200;

       view.setOnTouchListener(new OnTouchListener() {
        	int lastX, lastY;
        	int paramX, paramY;

			public boolean onTouch(View v, MotionEvent event) {
				switch(event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					lastX = (int) event.getRawX();
					lastY = (int) event.getRawY();
					paramX = params.x;
					paramY = params.y;
					break;
				case MotionEvent.ACTION_MOVE:
					int dx = (int) event.getRawX() - lastX;
					int dy = (int) event.getRawY() - lastY;
					params.x = paramX + dx;
					params.y = paramY + dy;
			        wm.updateViewLayout(view, params);
					break;
				}
				return true;
			}
		});
        wm.addView(view, params);
	}

    void initUI(){
    	mPercent = (TextView)view.findViewById(R.id.percent);
		mStrenght = nativeGetBlueLightFilterStrength();
		mStringPercent = String.valueOf((int)(((float)mStrenght/mStrenghtBig) * 100));
		mPercent.setText(mStringPercent + "%");
    	mCheckBox = (CheckBox)view.findViewById(R.id.Exit);
    	mCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
		    public void onCheckedChanged(CompoundButton buttonView,
				    boolean isChecked) {
	                // TODO Auto-generated method stub
			    	Log.d(TAG, "mCheckBox isChecked = " + isChecked);
			    	if(isChecked){
			    		mCheckBox.setChecked(false);
						wm.removeView(view);
						stopSelf();
			    	}

				}
	       });
    	mSwitchButton = (Switch) view.findViewById(R.id.mSwitch);
    	blf_enabled = nativeIsBlueLightFilterEnabled();
    	mSwitchButton.setChecked(blf_enabled);
    	mSwitchButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
		    public void onCheckedChanged(CompoundButton buttonView,
			    boolean isChecked) {
                // TODO Auto-generated method stub
		    	Log.d(TAG, "isChecked = " + isChecked);
					mSeekBar.setEnabled(isChecked);
					nativeEnableBlueLightFilter(isChecked);

			}
       });

	   mSeekBar = (SeekBar) view.findViewById(R.id.seekBar1);
	   mSeekBar.setEnabled(blf_enabled);
	   Log.d(TAG, "nativeGetBlueLightFilterStrengthRange :" + nativeGetBlueLightFilterStrengthRange());
	   mSeekBar.setMax(mStrenghtBig);
	   mSeekBar.setProgress(nativeGetBlueLightFilterStrength());
	   mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

		   public void onStopTrackingTouch(SeekBar seekBar) {
				Log.d(TAG, "onStopTrackingTouch:");
		   }

		   public void onStartTrackingTouch(SeekBar seekBar) {
				Log.d(TAG, "onStartTrackingTouch:");
		   }

		   public void onProgressChanged(SeekBar seekBar, int progress,
				   boolean fromUser) {
			    boolean ret;
			    Log.d(TAG, "onProgressChanged:" + progress);
				ret = nativeSetBlueLightFilterStrength(progress);
				mStrenght = nativeGetBlueLightFilterStrength();
				mStringPercent = String.valueOf((int)(((float)mStrenght/mStrenghtBig) * 100));
				mPercent.setText(mStringPercent + "%");
		   }
	   });

	}
}
