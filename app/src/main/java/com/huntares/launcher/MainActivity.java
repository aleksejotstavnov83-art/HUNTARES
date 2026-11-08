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

  // Native HUNTARES CHEAT uses a SurfaceComposer overlay and needs root to read game memory.
  // This launcher does not inject a .so into the game process.
  private static final String BIN = "huntares-cheat-1.1.0";
  private static final String DEST = "/data/local/tmp/huntares-cheat-1.1.0";
  private volatile boolean launching = false;

  private void notice(String message) {
    runOnUiThread(() -> Toast.makeText(this, message, Toast.LENGTH_LONG).show());
  }

  private static String shq(String value) {
    return "'" + value.replace("'", "'\"'\"'") + "'";
  }

  private void launchGame() {
    if (launching) return;
    Intent installed = getPackageManager().getLaunchIntentForPackage("com.axlebolt.standoff2");
    if (installed == null) {
      notice("Standoff 2 не найдена");
      return;
    }
    launching = true;
    new Thread(() -> {
      try {
        File source = new File(getFilesDir(), BIN);
        try (InputStream input = getAssets().open(BIN);
             FileOutputStream output = new FileOutputStream(source)) {
          byte[] chunk = new byte[16384];
          int n;
          while ((n = input.read(chunk)) >= 0) output.write(chunk, 0, n);
        }
        // KernelSU/Magisk will display their own root permission prompt.
        Process root = new ProcessBuilder("su", "-c", "id -u").redirectErrorStream(true).start();
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        try (InputStream in = root.getInputStream()) {
          byte[] b = new byte[1024]; int n;
          while ((n = in.read(b)) != -1 && out.size() < 4096) out.write(b, 0, n);
        }
        int status = root.waitFor();
        if (status != 0 || !out.toString("UTF-8").trim().equals("0")) {
          notice("Нужен root-доступ в KernelSU/Magisk");
          return;
        }
        String command = "cp " + shq(source.getAbsolutePath()) + " " + shq(DEST)
            + " && chmod 0700 " + shq(DEST) + " && exec " + shq(DEST);
        Process p = new ProcessBuilder("su", "-c", command).redirectErrorStream(true).start();
        notice("HUNTARES CHEAT запускается. Открой Standoff 2, если игра не запустилась сама.");
        try (BufferedReader logs = new BufferedReader(new InputStreamReader(p.getInputStream()))) {
          String line;
          while ((line = logs.readLine()) != null) Log.i("HUNTARES_CHEAT", line);
        }
        int exit = p.waitFor();
        Log.i("HUNTARES_CHEAT", "HUNTARES CHEAT exited code=" + exit);
        notice("HUNTARES CHEAT завершился, код: " + exit + ". Проверь Logcat.");
      } catch (Exception err) {
        Log.e("HUNTARES_CHEAT", "Unable to launch native program", err);
        notice("Запуск не удался: " + err.getClass().getSimpleName());
      } finally { launching = false; }
    }, "huntares-binary-loader").start();
  }
}
