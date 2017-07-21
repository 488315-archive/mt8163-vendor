package com.mediatek.crossmountsettings.util;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import com.mediatek.crossmountsettings.R;

/** A category with a progress spinner.
 */
public class ProgressCategory extends ProgressCategoryBase {
    private static final String TAG = "ProgressCategory";
    private int mEmptyTextRes;
    private boolean mProgress = false;

    /** Constructor.
     * @param context Context
     */
    public ProgressCategory(Context context) {
        this(context, null);
    }

    /** Constructor.
     * @param context Context
     * @param attrs AttributeSet
     */
    public ProgressCategory(Context context, AttributeSet attrs) {
        super(context, attrs, 0);
    }

    /** Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyleAttr int
     */
    public ProgressCategory(Context context, AttributeSet attrs,
            int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    /** Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyleAttr int
     * @param defStyleRes int
     */
    public ProgressCategory(Context context, AttributeSet attrs, int defStyleAttr,
            int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        setLayoutResource(R.layout.preference_progress_category);
    }

    /**
     * Set the empty string.
     * @param emptyTextRes
     */
    public void setEmptyTextRes(int emptyTextRes) {
        mEmptyTextRes = emptyTextRes;
    }

    @Override
    public void onBindView(View view) {
        super.onBindView(view);
        final View progressBar = view.findViewById(R.id.scanning_progress);
        Log.d(TAG, "onBindView,mProgress = " + mProgress);
        progressBar.setVisibility(mProgress ? View.VISIBLE : View.GONE);
    }

    @Override
    public void setProgress(boolean progressOn) {
        mProgress = progressOn;
        notifyChanged();
    }
}
