package com.huntares.launcher;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.net.Uri;
import android.provider.Settings;
import android.os.Build;
import android.graphics.Color;
import android.view.View;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.graphics.drawable.GradientDrawable;
import android.util.Log;

import java.io.*;
import java.util.concurrent.atomic.AtomicBoolean;

public final class MainActivity extends Activity {
    private static final String TAG = "HUNTARES_LOADER";
    private static final String PACKAGE = "com.axlebolt.standoff2";
    private static final String INJECTOR_ASSET = "AndKittyInjector";
    private static final String MODULE_ASSET = "libhuntares_internal.so";
    private static final String INJECTOR_PATH = "/data/local/tmp/huntares_injector";
    private static final String MODULE_PATH = "/data/local/tmp/libhuntares_internal.so";
    private final AtomicBoolean busy = new AtomicBoolean(false);
    private TextView status;

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().setStatusBarColor(Color.BLACK);
        getWindow().setNavigationBarColor(Color.BLACK);

        FrameLayout root = new FrameLayout(this);
        root.setBackgroundColor(Color.BLACK);
        ImageView image = new ImageView(this);
        image.setImageResource(R.drawable.background);
        image.setScaleType(ImageView.ScaleType.FIT_XY);
        root.addView(image, new FrameLayout.LayoutParams(-1, -1));

        View hitArea = new View(this);
        root.addView(hitArea, new FrameLayout.LayoutParams(-1, -1));
        hitArea.setOnTouchListener((view, event) -> {
            if (event.getAction() == android.view.MotionEvent.ACTION_UP) {
                float x = event.getX() / view.getWidth();
                float y = event.getY() / view.getHeight();
                if (x > .07f && x < .93f && y > .59f && y < .69f) {
                    launch(); return true;
                }
                if (x > .18f && x < .85f && y > .90f && y < .98f) {
                    openTelegram(); return true;
                }
            }
            return true;
        });

        final int pad = (int) (12 * getResources().getDisplayMetrics().density);
        GradientDrawable border = new GradientDrawable();
        border.setColor(0xE9161B27);
        border.setCornerRadius(pad);
        border.setStroke(Math.max(1, pad / 12), 0xFF72A8FF);

        TextView menu = new TextView(this);
        menu.setText("HUNTARES CHEAT  |  МЕНЮ");
        menu.setTextColor(Color.WHITE);
        menu.setTextSize(12);
        menu.setGravity(Gravity.CENTER);
        menu.setBackground(border.getConstantState().newDrawable().mutate());
        FrameLayout.LayoutParams mp = new FrameLayout.LayoutParams(-2, -2, Gravity.BOTTOM | Gravity.RIGHT);
        mp.rightMargin = pad;
        mp.bottomMargin = (int)(72 * getResources().getDisplayMetrics().density);
        menu.setPadding(pad, pad / 2, pad, pad / 2);
        root.addView(menu, mp);
        menu.setOnClickListener(v -> openOverlay());

        status = new TextView(this);
        status.setText("HUNTARES CHEAT  |  READY");
        status.setTextColor(Color.WHITE);
        status.setTextSize(11);
        status.setGravity(Gravity.CENTER);
        status.setBackgroundColor(0xAA000000);
        FrameLayout.LayoutParams st = new FrameLayout.LayoutParams(-1, -2, Gravity.TOP);
        st.topMargin = (int)(42 * getResources().getDisplayMetrics().density);
        root.addView(status, st);
        setContentView(root);
    }

    private boolean openOverlay() {
        if (Build.VERSION.SDK_INT >= 23 && !Settings.canDrawOverlays(this)) {
            show("Разреши 'Поверх других приложений' для сенсорного меню");
            Intent settings = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                    Uri.parse("package:" + getPackageName()));
            startActivity(settings);
            return false;
        }
        Intent service = new Intent(this, OverlayMenuService.class);
        if (Build.VERSION.SDK_INT >= 26) startForegroundService(service);
        else startService(service);
        show("HUNTARES: сенсорное меню включено");
        return true;
    }

    private void openTelegram() {
        try { startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://t.me/huntaresproject"))); }
        catch (Exception e) { show("Не удалось открыть Telegram"); }
    }

    private void show(String line) {
        runOnUiThread(() -> {
            status.setText(line);
            Toast.makeText(this, line, Toast.LENGTH_LONG).show();
        });
    }

    private void extract(String asset, File dest) throws IOException {
        try (InputStream in = getAssets().open(asset);
             OutputStream out = new FileOutputStream(dest)) {
            byte[] buf = new byte[16384]; int n;
            while ((n = in.read(buf)) != -1) out.write(buf, 0, n);
        }
    }

    private static String quote(String s) {
        return "'" + s.replace("'", "'\"'\"'") + "'";
    }

    private String runShell(String command) throws IOException, InterruptedException {
        java.lang.Process p = new ProcessBuilder("su", "-c", command)
                .redirectErrorStream(true).start();
        ByteArrayOutputStream result = new ByteArrayOutputStream();
        try (InputStream in = p.getInputStream()) {
            byte[] buf = new byte[4096]; int n;
            while ((n = in.read(buf)) != -1) {
                if (result.size() < 200000) result.write(buf, 0, Math.min(n, 200000 - result.size()));
            }
        }
        int code = p.waitFor();
        String log = result.toString("UTF-8");
        for (String line : log.split("\\r?\\n")) if (!line.isEmpty()) Log.i(TAG, line);
        if (code != 0) throw new IOException("root command exit=" + code + ": " +
                log.substring(0, Math.min(log.length(), 600)));
        return log;
    }

    private void launch() {
        if (!busy.compareAndSet(false, true)) return;
        if (getPackageManager().getLaunchIntentForPackage(PACKAGE) == null) {
            busy.set(false); show("Standoff 2 не найдена"); return;
        }
        if (!openOverlay()) { busy.set(false); return; }
        show("HUNTARES: запрашиваю root и подготавливаю запуск");
        new Thread(() -> {
            File bin = new File(getFilesDir(), INJECTOR_ASSET);
            File lib = new File(getFilesDir(), MODULE_ASSET);
            try {
                extract(INJECTOR_ASSET, bin);
                extract(MODULE_ASSET, lib);
                if (!runShell("id -u").trim().equals("0"))
                    throw new IOException("su не предоставил uid=0");
                runShell("cp " + quote(bin.getAbsolutePath()) + " " + quote(INJECTOR_PATH)
                    + " && cp " + quote(lib.getAbsolutePath()) + " " + quote(MODULE_PATH)
                    + " && chmod 0700 " + quote(INJECTOR_PATH)
                    + " && chmod 0644 " + quote(MODULE_PATH));
                show("HUNTARES: запуск Standoff 2 через native injector");
                String cmd = quote(INJECTOR_PATH) + " --package " + quote(PACKAGE)
                    + " --libs " + quote(MODULE_PATH)
                    + " --memfd --timeout 3000 "
                    + "$(pidof " + PACKAGE + " >/dev/null 2>&1 || echo --launch)";
                String log = runShell(cmd);
                if (log.contains("Successfully injected") || log.contains("Injected libhuntares_internal.so successfully"))
                    show("HUNTARES: инжектор сообщил об успешной загрузке; проверь меню");
                else show("HUNTARES: команда завершилась, проверь логи");
            } catch (Exception e) {
                Log.e(TAG, "Native launch error", e);
                show("HUNTARES: ошибка запуска, см. Logcat (" + e.getClass().getSimpleName() + ")");
            } finally {
                busy.set(false);
            }
        }, "HUNTARES-Inject").start();
    }
}
