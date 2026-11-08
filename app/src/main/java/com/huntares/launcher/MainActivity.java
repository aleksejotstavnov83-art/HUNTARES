package com.huntares.launcher;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.net.Uri;
import android.graphics.Color;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.view.Gravity;
import android.graphics.drawable.GradientDrawable;
import android.widget.Toast;
import android.util.Log;
import java.io.*;
import java.nio.charset.StandardCharsets;
import android.app.AppOpsManager;
import android.content.Context;

public class MainActivity extends Activity {
  @Override public void onCreate(Bundle state) {
    super.onCreate(state);
    getWindow().setStatusBarColor(Color.BLACK);
    getWindow().setNavigationBarColor(Color.BLACK);
    FrameLayout root = new FrameLayout(this);
    root.setBackgroundColor(Color.BLACK);
    ImageView image = new ImageView(this);
    image.setImageResource(R.drawable.background);
    image.setScaleType(ImageView.ScaleType.FIT_XY);
    root.addView(image,new FrameLayout.LayoutParams(-1,-1));
    // Touch targets follow the positions in the approved poster (1024x1792).
    View inject = new View(this);
    FrameLayout.LayoutParams injectParams = new FrameLayout.LayoutParams(-1,-1);
    root.addView(inject,injectParams);
    inject.setOnTouchListener((v,event)->{
      if(event.getAction()==android.view.MotionEvent.ACTION_UP){
        float x=event.getX()/v.getWidth(), y=event.getY()/v.getHeight();
        if(x>.07f && x<.93f && y>.59f && y<.69f){launchGame();return true;}
        if(x>.18f && x<.85f && y>.90f && y<.98f){openTelegram();return true;}
      }
      return true;
    });
    TextView skins = new TextView(this);
    skins.setText("SKIN CATALOG");
    skins.setTextColor(Color.WHITE);
    skins.setTextSize(13);
    skins.setGravity(Gravity.CENTER);
    skins.setLetterSpacing(0.12f);
    int padding = (int)(12 * getResources().getDisplayMetrics().density);
    skins.setPadding(padding, padding/2, padding, padding/2);
    GradientDrawable outline = new GradientDrawable();
    outline.setColor(0xDD141519);
    outline.setCornerRadius(12 * getResources().getDisplayMetrics().density);
    outline.setStroke((int)getResources().getDisplayMetrics().density, 0xFFBBBBBB);
    skins.setBackground(outline);
    FrameLayout.LayoutParams sp = new FrameLayout.LayoutParams(-2, -2, Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL);
    sp.bottomMargin = (int)(105 * getResources().getDisplayMetrics().density);
    root.addView(skins,sp);
    skins.setOnClickListener(v -> startActivity(new Intent(this, SkinGalleryActivity.class)));
    setContentView(root);

  }
  private void openTelegram(){
    try {startActivity(new Intent(Intent.ACTION_VIEW,Uri.parse("https://t.me/huntaresproject")));}
    catch(Exception e){Toast.makeText(this,"Не удалось открыть Telegram",Toast.LENGTH_SHORT).show();}
  }


  // The APK starts the game; Zygisk loads the actual internal module at app creation.
  // Installing/rebooting the Zygisk module is an explicit separate user action.
  private volatile boolean launching = false;

  private void notice(String message) {
    runOnUiThread(() -> Toast.makeText(this, message, Toast.LENGTH_LONG).show());
  }

  private void launchGame() {
    if (launching) return;
    Intent intent = getPackageManager().getLaunchIntentForPackage("com.axlebolt.standoff2");
    if (intent == null) { notice("Standoff 2 не установлена"); return; }
    launching = true;
    new Thread(() -> {
      try {
        Process p = new ProcessBuilder("su", "-c",
          "test -f /data/adb/modules/huntares_internal/zygisk/arm64-v8a.so " +
          "&& test ! -e /data/adb/modules/huntares_internal/disable").start();
        int result = p.waitFor();
        if (result != 0) {
          notice("Сначала установи HUNTARES INTERNAL ZIP в KernelSU/Zygisk и перезагрузи телефон");
          return;
        }
        notice("Модуль найден. Запускаю Standoff 2. Внутреннее меню зависит от Zygisk.");
        runOnUiThread(() -> startActivity(intent));
      } catch (Exception e) {
        Log.e("HUNTARES_INTERNAL", "Cannot check installed module", e);
        notice("Не удалось проверить модуль: " + e.getClass().getSimpleName());
      } finally { launching = false; }
    }, "huntares-internal-loader").start();
  }
}
