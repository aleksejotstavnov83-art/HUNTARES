package com.huntares.launcher;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Color;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

/** Offline skin-preview index; does not modify Standoff 2. */
public final class SkinGalleryActivity extends Activity {
  private WebView gallery;
  @Override public void onCreate(Bundle state) {
    super.onCreate(state);
    getWindow().setStatusBarColor(Color.BLACK);
    getWindow().setNavigationBarColor(Color.BLACK);
    gallery = new WebView(this);
    gallery.setBackgroundColor(Color.BLACK);
    gallery.setWebViewClient(new WebViewClient());
    WebSettings settings = gallery.getSettings();
    settings.setJavaScriptEnabled(true);
    settings.setDomStorageEnabled(true);
    settings.setAllowFileAccess(true);
    settings.setAllowContentAccess(false);
    settings.setAllowFileAccessFromFileURLs(false);
    settings.setAllowUniversalAccessFromFileURLs(false);
    setContentView(gallery);
    gallery.loadUrl("file:///android_asset/gallery/index.html");
  }
  @Override protected void onDestroy() {
    if (gallery != null) {gallery.destroy();gallery=null;}
    super.onDestroy();
  }
}
