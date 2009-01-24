/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "preinclude.h"
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import "webview_delegate.h"
#import "window_config.h"
//#import "../user_window.h"

@class WebViewDelegate;

using namespace ti;

@interface NativeWindow : NSWindow {
	BOOL canReceiveFocus;
	WindowConfig* config;
	WebView* webView;
	WebViewDelegate* delegate;
	BOOL requiresDisplay;
	SharedBoundObject* userWindow;
}
- (void)setupDecorations:(WindowConfig*)config host:(Host*)h userwindow:(UserWindow*)uw;
- (void)setTransparency:(double)transparency;
- (void)setFullScreen:(BOOL)yn;
- (void)close;
- (void)open;
- (void)frameLoaded;
- (WebView*)webView;
- (WindowConfig*)config;
- (SharedBoundObject)userWindow;
- (void)setInitialWindow:(BOOL)yn;
@end
