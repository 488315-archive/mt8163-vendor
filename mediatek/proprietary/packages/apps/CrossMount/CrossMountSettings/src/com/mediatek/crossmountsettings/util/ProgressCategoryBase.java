package com.mediatek.crossmountsettings.util;

import android.content.Context;
import android.preference.PreferenceCategory;
import android.util.AttributeSet;

public abstract class ProgressCategoryBase extends PreferenceCategory {
    /**
     * Constructor.
     * @param context Context
     */
    public ProgressCategoryBase(Context context) {
        this(context, null);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     */
    public ProgressCategoryBase(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyleAttr int
     */
    public ProgressCategoryBase(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr, 0);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyleAttr int
     * @param defStyleRes int
     */
    public ProgressCategoryBase(Context context, AttributeSet attrs, int defStyleAttr,
                                int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    /**
     * Turn on/off the progress indicator and text on the right.
     * @param progressOn whether or not the progress should be displayed
     */
    public abstract void setProgress(boolean progressOn);
}