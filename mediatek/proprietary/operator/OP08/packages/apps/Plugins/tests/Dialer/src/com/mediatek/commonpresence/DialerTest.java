package com.mediatek.commonpresence;

import java.util.ArrayList;
import junit.framework.Assert;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Intent;
import android.provider.CallLog;
import android.test.ActivityInstrumentationTestCase2;
import android.util.Log;
import android.view.View;
import org.gsma.joyn.capability.Capabilities;
import com.jayway.android.robotium.solo.Solo;
import com.android.dialer.calllog.CallLogActivity;

public class DialerTest extends
    ActivityInstrumentationTestCase2<CallLogActivity> {

  private Solo solo;

  public static final String TAG = "DialerTest";
  public static final String CONTACT_NONVIDEO_CAPABLE = "VideoCapableNo Contact";
  public static final String CONTACT_VIDEO_CAPABLE = "VideoCapableYes Contact";
  public static final String CONTACT_NONVIDEO_CAPABLE_NUMBER = "123456";
  public static final String CONTACT_VIDEO_CAPABLE_NUMBER = "1234567";
  public static final int MAX_NUMBER_OF_SCROLLS = 100;
  private static final String INTENT_VIDEO_CAPABILITIES =
      "com.mediatek.commonpresence.capability.VIDEO_CAPABILITIES";

  public DialerTest() {
    super(CallLogActivity.class);
  }

  public void setUp() throws Exception {
    solo = new Solo(getInstrumentation(), getActivity());
    Log.d(TAG, "setUp entry ");

    insertPlaceholderCall(getActivity().getContentResolver(), CONTACT_NONVIDEO_CAPABLE_NUMBER);
    insertPlaceholderCall(getActivity().getContentResolver(), CONTACT_VIDEO_CAPABLE_NUMBER);

    //send intent to make second contact video capable
    Capabilities capabilities = new Capabilities(false, false,
            false, false, false, false, false, null,
            false, false, false, false, false);
    capabilities.setIR94_VideoCall(true);
    Intent intent = new Intent(INTENT_VIDEO_CAPABILITIES);
    intent.putExtra("number", CONTACT_VIDEO_CAPABLE_NUMBER);
    getActivity().sendBroadcast(intent);

  }


  @Override
  public void tearDown() throws Exception {
      Log.d(TAG, "tearDown entry ");
      solo.finishOpenedActivities();
  }

  public void testNonVideoCapableCallLog(){

      Log.d(TAG, "testNonVideoCapableCallLog entry ");
      solo.assertCurrentActivity("wrong activity", CallLogActivity.class);

      //search for non video capable call log
      boolean found = false;
      for(int i=0 ; i <= MAX_NUMBER_OF_SCROLLS ; i++){
          found = solo.searchText("VideoCapableNo");
          if(found == true){
              break;
          }
          solo.scrollDown();
      }
      //if still not found, search bottom
      if(found == false){
          solo.scrollToBottom();
      }
      //click non video capable contact
      //assertTrue(solo.waitForText("VideoCapableNo"));
      solo.clickOnText("VideoCapableNo");
      solo.sleep(2000);

      //check video icon
      View view = solo.getView(com.android.dialer.R.id.video_call_action);
      Log.d(TAG, "testNonVideoCapableContact view: " + view + "vis:"+view.getVisibility());
      // view visibility should be GONE/INVISIBLE to make test case pass
      assertTrue(view.getVisibility() == View.GONE);
  }

  public void testVideoCapableCallLog(){

      Log.d(TAG, "testVideoCapableCallLog entry ");
      solo.assertCurrentActivity("wrong activity", CallLogActivity.class);

      //search for video capable contact
      boolean found = false;
      for(int i=0 ; i <= MAX_NUMBER_OF_SCROLLS ; i++){
          found = solo.searchText("VideoCapableYes");
          if(found == true){
              break;
          }
          solo.scrollDown();
      }
      //if still not found, search bottom
      if(found == false){
          solo.scrollToBottom();
      }
      //click non video capable contact
      //assertTrue(solo.waitForText("VideoCapableYes"));
      solo.clickOnText("VideoCapableYes");
      solo.sleep(8000);

      //check video icon
      View view = solo.getView(com.android.dialer.R.id.video_call_action);
      Log.d(TAG, "testVideoCapableContact view: "+ view + "vis:"+ view.getVisibility());
      //view should not be null and visibility VISIBLE to make test case pass
      assertTrue(view.getVisibility() == View.VISIBLE);

  }

  private void insertPlaceholderCall(ContentResolver contentResolver, String number){
      ContentValues values = new ContentValues();
      values.put(CallLog.Calls.NUMBER, number);
      values.put(CallLog.Calls.DATE, System.currentTimeMillis());
      values.put(CallLog.Calls.DURATION, 0);
      values.put(CallLog.Calls.TYPE, CallLog.Calls.OUTGOING_TYPE);
      values.put(CallLog.Calls.NEW, 1);
      values.put(CallLog.Calls.CACHED_NAME, "");
      values.put(CallLog.Calls.CACHED_NUMBER_TYPE, 0);
      values.put(CallLog.Calls.CACHED_NUMBER_LABEL, "");
      Log.d(TAG, "Inserting call log placeholder for " + number);
      contentResolver.insert(CallLog.Calls.CONTENT_URI, values);
  }
}