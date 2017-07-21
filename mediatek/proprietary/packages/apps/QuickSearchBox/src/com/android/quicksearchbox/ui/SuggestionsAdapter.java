/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.quicksearchbox.ui;

import com.android.quicksearchbox.Promoter;
import com.android.quicksearchbox.SuggestionCursor;
import com.android.quicksearchbox.SuggestionPosition;
import com.android.quicksearchbox.Suggestions;

import android.view.View.OnFocusChangeListener;
import android.widget.ExpandableListAdapter;
import android.widget.ListAdapter;

/**
 * Interface for suggestions adapters.
 *
 * @param <A> the adapter class used by the UI, probably either {@link ListAdapter} or
 *      {@link ExpandableListAdapter}.
 */
public interface SuggestionsAdapter<A> {

    /**
     * Sets the maximum number of promoted suggestions to be provided by this adapter.
     */
    void setMaxPromoted(int maxPromoted);

    /**
     * Sets the suggestion promoter.
     */
    void setPromoter(Promoter promoter);

    /**
     * Sets the listener to be notified of clicks on suggestions.
     */
    void setSuggestionClickListener(SuggestionClickListener listener);

    /**
     * Sets the listener to be notified of focus change events on suggestion views.
     */
    void setOnFocusChangeListener(OnFocusChangeListener l);

    /**
     * Sets the current suggestions.
     */
    void setSuggestions(Suggestions suggestions);

    /**
     * Indicates if there's any suggestions in this adapter.
     */
    boolean isEmpty();

    /**
     * Gets the current suggestions.
     */
    Suggestions getSuggestions();

    /**
     * Gets the cursor and position corresponding to the given suggestion ID.
     * @param suggestionId Suggestion ID.
     */
    SuggestionPosition getSuggestion(long suggestionId);

    /**
     * Gets the current list of promoted suggestions.
     */
    SuggestionCursor getCurrentPromotedSuggestions();

    /**
     * Handles a regular click on a suggestion.
     *
     * @param suggestionId The ID of the suggestion clicked. If the suggestion list is flat, this
     *      will be the position within the list.
     */
    void onSuggestionClicked(long suggestionId);

    /**
     * Handles a click on a quick contact badge.
     *
     * @param suggestionId The ID of the suggestion clicked. If the suggestion list is flat, this
     *      will be the position within the list.
     */
    void onSuggestionQuickContactClicked(long suggestionId);

    /**
     * Handles a request to remove a suggestion from history.
     *
     * @param suggestionId The ID of the suggestion clicked. If the suggestion list is flat, this
     *      will be the position within the list.
     */
    void onSuggestionRemoveFromHistoryClicked(long suggestionId);

    /**
     * Handles a click on the query refinement button.
     *
     * @param suggestionId The ID of the suggestion clicked. If the suggestion list is flat, this
     *      will be the position within the list.
     */
    void onSuggestionQueryRefineClicked(long suggestionId);

    /**
     * Gets the adapter to be used by the UI view.
     */
    A getListAdapter();

}
