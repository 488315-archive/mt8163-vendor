package com.mediatek.commonpresence;

import java.util.ArrayList;

import junit.framework.Assert;
import android.content.ContentProviderOperation;
import android.content.ContentProviderResult;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.RemoteException;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.provider.ContactsContract.CommonDataKinds.StructuredName;
import android.provider.ContactsContract.Contacts.Data;
import android.provider.ContactsContract.RawContacts;
import android.test.ActivityInstrumentationTestCase2;
import android.util.Log;
import android.view.View;

import org.gsma.joyn.capability.Capabilities;

import com.jayway.android.robotium.solo.Solo;
import com.android.contacts.activities.PeopleActivity;

public class ContactsTest extends
    ActivityInstrumentationTestCase2<PeopleActivity> {

  private Solo solo;

  public static final String TAG = "ContactsTest";
  public static final String CONTACT_NONVIDEO_CAPABLE = "VideoCapableNo Contact";
  public static final String CONTACT_VIDEO_CAPABLE = "VideoCapableYes Contact";
  public static final String CONTACT_NONVIDEO_CAPABLE_NUMBER = "123456";
  public static final String CONTACT_VIDEO_CAPABLE_NUMBER = "1234567";
  public static final int MAX_NUMBER_OF_SCROLLS = 100;
  private static final String INTENT_VIDEO_CAPABILITIES =
      "com.mediatek.commonpresence.capability.VIDEO_CAPABILITIES";
  public ContactsTest() {
    super(PeopleActivity.class);
  }

  public void setUp() throws Exception {
    solo = new Solo(getInstrumentation(), getActivity());
    Log.d(TAG, "setUp entry ");
    insertContactNonVideoCapable();
    insertContactVideoCapable();
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
  public void testNonVideoCapableContact(){

      Log.d(TAG, "testNonVideoCapableContact entry ");
      solo.assertCurrentActivity("wrong activity", PeopleActivity.class);

      //search for non video capable contact
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
      View view = solo.getView(com.android.contacts.R.id.third_icon);
      Log.d(TAG, "testNonVideoCapableContact view: " + view + "vis:"+view.getVisibility());
      // view visibility should be GONE/INVISIBLE to make test case pass
      assertTrue(view.getVisibility() == View.GONE);
  }

  public void testVideoCapableContact(){

      Log.d(TAG, "testVideoCapableContact entry ");
      solo.assertCurrentActivity("wrong activity", PeopleActivity.class);
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
      solo.sleep(2000);
      //check video icon
      View view = solo.getView(com.android.contacts.R.id.third_icon);
      Log.d(TAG, "testVideoCapableContact view: "+ view + "vis:"+ view.getVisibility());
      //view should not be null and visibility VISIBLE to make test case pass
      assertTrue(view.getVisibility() == View.VISIBLE);
  }

  public void insertContactNonVideoCapable(){
      ArrayList<ContentProviderOperation> ops = new ArrayList<ContentProviderOperation>();
      int rawContactInsertIndex = ops.size();

      ops.add(ContentProviderOperation.newInsert(RawContacts.CONTENT_URI)
        .withValue(RawContacts.ACCOUNT_TYPE, null)
        .withValue(RawContacts.ACCOUNT_NAME, null).build());

      //Phone Number
      ops.add(ContentProviderOperation
        .newInsert(ContactsContract.Data.CONTENT_URI)
        .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID,
          rawContactInsertIndex)
        .withValue(Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE)
        .withValue(Phone.NUMBER, CONTACT_NONVIDEO_CAPABLE_NUMBER)
        .withValue(Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE)
        .withValue(Phone.TYPE, "1").build());

      //Display name/Contact name
      ops.add(ContentProviderOperation
        .newInsert(ContactsContract.Data.CONTENT_URI)
        .withValueBackReference(Data.RAW_CONTACT_ID,
          rawContactInsertIndex)
        .withValue(Data.MIMETYPE, StructuredName.CONTENT_ITEM_TYPE)
        .withValue(StructuredName.DISPLAY_NAME, CONTACT_NONVIDEO_CAPABLE)
        .build());
      try {
          ContentProviderResult[] res = getActivity().getContentResolver().applyBatch(
            ContactsContract.AUTHORITY, ops);
         } catch (RemoteException e) {
          e.printStackTrace();
         } catch (Exception e) {
          e.printStackTrace();
         }
  }

  public void insertContactVideoCapable(){
      ArrayList<ContentProviderOperation> ops = new ArrayList<ContentProviderOperation>();
      int rawContactInsertIndex = ops.size();

      ops.add(ContentProviderOperation.newInsert(RawContacts.CONTENT_URI)
        .withValue(RawContacts.ACCOUNT_TYPE, null)
        .withValue(RawContacts.ACCOUNT_NAME, null).build());

      //Phone Number
      ops.add(ContentProviderOperation
        .newInsert(ContactsContract.Data.CONTENT_URI)
        .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID,
          rawContactInsertIndex)
        .withValue(Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE)
        .withValue(Phone.NUMBER, CONTACT_VIDEO_CAPABLE_NUMBER)
        .withValue(Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE)
        .withValue(Phone.TYPE, "1").build());

      //Display name/Contact name
      ops.add(ContentProviderOperation
        .newInsert(ContactsContract.Data.CONTENT_URI)
        .withValueBackReference(Data.RAW_CONTACT_ID,
          rawContactInsertIndex)
        .withValue(Data.MIMETYPE, StructuredName.CONTENT_ITEM_TYPE)
        .withValue(StructuredName.DISPLAY_NAME, CONTACT_VIDEO_CAPABLE)
        .build());

      try {
          ContentProviderResult[] res = getActivity().getContentResolver().applyBatch(
            ContactsContract.AUTHORITY, ops);
         } catch (RemoteException e) {
          // TODO Auto-generated catch block
          e.printStackTrace();
         } catch (Exception e) {
          // TODO Auto-generated catch block
          e.printStackTrace();
         }
  }
}