/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __mozilla_widget_nsShmImage_h__
#define __mozilla_widget_nsShmImage_h__

#if defined(MOZ_X11)
#  define MOZ_HAVE_SHMIMAGE
#endif

#ifdef MOZ_HAVE_SHMIMAGE

#include "mozilla/gfx/2D.h"
#include "nsIWidget.h"
#include "nsAutoPtr.h"

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#ifdef MOZ_WIDGET_QT
class QRect;
class QWindow;
#endif

class nsShmImage {
    // bug 1168843, compositor thread may create shared memory instances that are destroyed by main thread on shutdown, so this must use thread-safe RC to avoid hitting assertion
    NS_INLINE_DECL_THREADSAFE_REFCOUNTING(nsShmImage)

public:
    static bool UseShm();
    static already_AddRefed<mozilla::gfx::DrawTarget>
        EnsureShmImage(const mozilla::LayoutDeviceIntSize& aSize,
                       Display* aDisplay, Visual* aVisual, unsigned int aDepth,
                       RefPtr<nsShmImage>& aImage);

    already_AddRefed<mozilla::gfx::DrawTarget> CreateDrawTarget();

#ifdef MOZ_WIDGET_GTK
    void Put(Display* aDisplay, Drawable aWindow,
             const mozilla::LayoutDeviceIntRegion& aRegion);
#elif defined(MOZ_WIDGET_QT)
    void Put(QWindow* aWindow, QRect& aRect);
#endif

    mozilla::LayoutDeviceIntSize Size() const { return mSize; }

private:
    nsShmImage()
        : mImage(nullptr)
        , mDisplay(nullptr)
        , mFormat(mozilla::gfx::SurfaceFormat::UNKNOWN)
        , mXAttached(false)
    { mInfo.shmid = -1; }

    ~nsShmImage();

    bool CreateShmSegment();
    void DestroyShmSegment();

    bool CreateImage(const mozilla::LayoutDeviceIntSize& aSize,
                     Display* aDisplay, Visual* aVisual, unsigned int aDepth);

    XImage*                      mImage;
    Display*                     mDisplay;
    XShmSegmentInfo              mInfo;
    mozilla::LayoutDeviceIntSize mSize;
    mozilla::gfx::SurfaceFormat  mFormat;
    bool                         mXAttached;
};

#endif // MOZ_HAVE_SHMIMAGE

#endif
