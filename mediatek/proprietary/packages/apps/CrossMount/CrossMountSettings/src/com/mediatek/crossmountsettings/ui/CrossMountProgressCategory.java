
package com.mediatek.crossmountsettings.ui;

import android.content.Context;
import android.util.AttributeSet;

import com.mediatek.crossmountsettings.util.ProgressCategory;
import com.mediatek.crossmountsettings.R;

/**
 * A Crossmount discovery progress category.
 */
public class CrossMountProgressCategory extends ProgressCategory {

    /**
     * Constructor.
     * @param context Context
     */
    public CrossMountProgressCategory(Context context) {
        this(context, null);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     */
    public CrossMountProgressCategory(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyleAttr int
     */
    public CrossMountProgressCategory(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyleAttr int
     * @param defStyleRes int
     */
    public CrossMountProgressCategory(Context context, AttributeSet attrs, int defStyleAttr,
            int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }
}
