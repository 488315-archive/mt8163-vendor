package com.mediatek.multiwindow.service;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileOutputStream;
import java.io.BufferedOutputStream;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;
import com.android.internal.util.FastXmlSerializer;

import android.util.ArraySet;
import android.util.Slog;
import android.os.FileUtils;

import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.List;
import java.util.ArrayList;
import android.util.Log;
import android.content.Context;
import android.content.res.XmlResourceParser;

import java.util.concurrent.CopyOnWriteArraySet;

import static android.os.Process.PACKAGE_INFO_GID;
import static android.os.Process.SYSTEM_UID;
/**
test xml
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <MinimaxRestartList>
        <PackageName>com.android.calendar</PackageName>
        <WindowName>com.android.contacts</WindowName>
    </MinimaxRestartList>
    <ConfigNotChangeList>
        <PackageName>com.android.calculator2</PackageName>
        <WindowName>com.android.calculator2</WindowName>
    </ConfigNotChangeList>
    <DisableFloatList>
        <PackageName>com.android.settings</PackageName>
        <WindowName>com.android.settings</WindowName>
    </DisableFloatList>
</root>
**/

class BlackListParser {
    private static final String TAG = "MW.BlackListParser";
    private static final String DBG = "MW.BlackListParser";

    private Context mContext;
    ArrayList<BaseNameList> mLists = new ArrayList<BaseNameList>();
    BaseNameList mRestartPkgList = null;
    //BaseNameList mConfigChangePkgList = null;
    BaseNameList mConfigNotChangePkgList = null;
    BaseNameList mSupportFloatPkgList = null;
    private final Object mLock = new Object();

    interface MajorTags{
        static final String Restart = "MinimaxRestartList";
        static final String ConfigNotChange = "ConfigNotChangeList";
        //static final String ConfigChange = "ConfigChangeList";
        static final String SupportFloat = "SupportFloatList";
    }

    interface MinorTags{
        static final String Package = "PackageName";
        //static final String Activity = "ActivityName";
        //static final String Window = "WindowName";
    }

    BlackListParser(Context context) {
        mContext = context;
        mRestartPkgList =
                new BaseNameList(MajorTags.Restart, MinorTags.Package);
        mConfigNotChangePkgList =
                new BaseNameList(MajorTags.ConfigNotChange, MinorTags.Package);
        mSupportFloatPkgList =
                new BaseNameList(MajorTags.SupportFloat, MinorTags.Package);
        mLists.add(mRestartPkgList);
        mLists.add(mConfigNotChangePkgList);
        mLists.add(mSupportFloatPkgList);
    }

    void clearAllList() {
        for (BaseNameList list : mLists)
            list.clearAllName();
    }

    boolean isMajorTag(String tag) {
        if (MajorTags.Restart.equals(tag))
            return true;
        if (MajorTags.ConfigNotChange.equals(tag))
            return true;
        //if (MajorTags.ConfigChange.equals(tag))
        //    return true;
        //if (MajorTags.DisableFloat.equals(tag))
        //    return true;
        if (MajorTags.SupportFloat.equals(tag))
            return true;
        return false;
    }

    boolean isMinorTag(String tag) {
        if (MinorTags.Package.equals(tag))
            return true;
        /*if (MinorTags.Activity.equals(tag))
            return true;
        if (MinorTags.Window.equals(tag))
            return true;
        */
        return false;
    }

    void LoadBlackList() {
            synchronized (this) {
            readFromXmlLocked();
        }
    }

    private void readFromXmlLocked(){
        try {
                /*
            String xml = read(file);
            ByteArrayInputStream bin = new ByteArrayInputStream(xml.getBytes());
            InputStreamReader in = new InputStreamReader(bin);

            XmlPullParserFactory pullFactory = XmlPullParserFactory.newInstance();
            XmlPullParser parser = pullFactory.newPullParser();
            */
            XmlResourceParser parser = mContext.getResources().getXml(R.xml.blacklist);

            String inMajorTag = "";

            clearAllList();

            //parser.setInput(in);
            int eventType = parser.getEventType();
            while (eventType != XmlPullParser.END_DOCUMENT) {
                String nodeName = parser.getName();
                switch (eventType) {
                case XmlPullParser.START_TAG:
                    if (isMajorTag(nodeName)) {
                        inMajorTag = nodeName;
                    } else if (isMinorTag(nodeName)) {
                        readByTag(inMajorTag, nodeName, parser.nextText());
                    }
                    break;
                case XmlPullParser.END_TAG:
                    inMajorTag = "";
                default:
                    break;
                }
                eventType = parser.next();
            }
        } catch (XmlPullParserException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void readByTag(String majorTag, String minorTag, String name) {
        for (BaseNameList list : mLists) {
            if (list.matchTag(majorTag, minorTag)) {
                list.add(name);
            }
        }
    }

    String read(File src) {
        StringBuffer res = new StringBuffer();
        String line = null;
        try {
            BufferedReader reader = new BufferedReader(new FileReader(src));
            while ((line = reader.readLine()) != null) {
                res.append(line + "\n");
            }
            reader.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return res.toString();
    }
    
    void updateBlackListToXml() {
        writeToXmlLocked();
    }

    private void writeToXmlLocked() {
        try {
            FileOutputStream fstr = new FileOutputStream("data/system/blacklist.xml");
            BufferedOutputStream str = new BufferedOutputStream(fstr);
            FileUtils.setPermissions("data/system/blacklist.xml", 0660, SYSTEM_UID, SYSTEM_UID);
            // XmlSerializer serializer = XmlUtils.serializerInstance();
            XmlSerializer serializer = new FastXmlSerializer();
            serializer.setOutput(str, "utf-8");
            serializer.startDocument(null, true);
            serializer.startTag(null, "root");

            for (BaseNameList list : mLists)
                list.writeToXml(serializer);

            serializer.endTag(null, "root");
            serializer.endDocument();

            str.flush();
            FileUtils.sync(fstr);
            str.close();
        } catch (Exception e) {
            Slog.e("XmlParser", "IOException " + e.toString());
        }
    }

    public boolean shouldChangeConfig(String packageName) {
                if (mConfigNotChangePkgList.contains(packageName))
                    return false;
        return true;
    }

    public boolean shouldRestartWhenMiniMax(String packageName) {
                if (mRestartPkgList.contains(packageName))
                      return true;
        return false;
    }

    public boolean inWhiteList(String packageName) {
            if (mSupportFloatPkgList.contains(packageName))
                  return true;
        return false;
    }

    public List<String> getWhiteList() {
          return mSupportFloatPkgList.getList();
    }

    public void addIntoWhiteList(String packageName) {
          mSupportFloatPkgList.add(packageName);
    }

    public void removeFromWhiteList(String packageName) {
          mSupportFloatPkgList.remove(packageName);
    }

    void dumpAllList(FileDescriptor fd, PrintWriter pw) {
        String prefix = "    ";
        pw.println();
            pw.println();
                synchronized (this) {
            // BlackNameList
            for (BaseNameList list : mLists)
                list.dump(fd, pw, prefix);
        }
    }

    class BaseNameList{
        CopyOnWriteArraySet<String> mNameList = new CopyOnWriteArraySet<String>();
        String mMajorTag;
        String mMinorTag;
        private final Object mLock = new Object();


        BaseNameList(String majorTag, String minorTag) {
            mMajorTag = majorTag;
            mMinorTag = minorTag;
        }

        boolean matchTag(String majorTag, String minorTag) {
            if (mMajorTag.equals(majorTag) && mMinorTag.equals(minorTag))
                return true;
            return false;
        }
        void add(String name) {
                mNameList.add(name);
        }

        void remove(String name) {
                mNameList.remove(name);
        }

        void writeToXml(XmlSerializer serializer) throws java.io.IOException {
            serializer.startTag(null, mMajorTag);
            for (String s : mNameList) {
                serializer.startTag(null, mMinorTag);
                serializer.text(s);
                serializer.endTag(null, mMinorTag);
            }
            serializer.endTag(null, mMajorTag);
        }

        boolean contains(String name){
            return mNameList.contains(name);
        }

        void dump(FileDescriptor fd, PrintWriter pw, String prefix){
            pw.println(mMajorTag + ":");
            pw.print(prefix);
            pw.println(mMinorTag + ":");
            for (String name : mNameList) {
                pw.print(prefix);
                pw.print(prefix);
                pw.println(name);
            }
        }

        List<String> getList(){
            List<String> list = new ArrayList<String>();
            for (String name : mNameList)
                list.add(name);
            return list;
        }

        void clearAllName(){
            mNameList.clear();
        }
    }
}

