/******************************************************************************
 *  This program is protected under international and U.S. copyright laws as
 *  an unpublished work. This program is confidential and proprietary to the
 *  copyright owners. Reproduction or disclosure, in whole or in part, or the
 *  production of derivative works therefrom without the express permission of
 *  the copyright owners is prohibited.
 *
 *                 Copyright (C) 2011-2012 by Dolby Laboratories,
 *                             All rights reserved.
 ******************************************************************************/

package com.dolby.ds1appUI;

public interface IDsFragProfileEditorObserver {

    public abstract void profileReset(int profile);

    public abstract int getProfileSelected();

    public abstract void onProfileNameEditStarted();

    public abstract void profileEditorIsAlive();

    public abstract void onProfileNameEditEnded();
}
