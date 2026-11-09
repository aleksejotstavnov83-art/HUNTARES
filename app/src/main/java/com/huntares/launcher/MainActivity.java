package com.huntares.launcher;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Color;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.graphics.drawable.GradientDrawable;
import android.util.Log;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.atomic.AtomicBoolean;

public final class MainActivity extends Activity {
    private static final String TAG = "HUNTARES_LOADER";
    private static final String GAME_PACKAGE = "com.axlebolt.standoff2";
    private static final String MODULE_ASSET = "libneptuniumLib.so";
    private static final String INJECTOR_ASSET = "AndKittyInjector";
    private static final String REMOTE_LIB = "/data/local/tmp/libneptuniumLib.so";
    private static final String REMOTE_INJECTOR = "/data/local/tmp/huntares_injector";
    private final AtomicBoolean busy = new AtomicBoolean(false);
    private TextView status;

    @Override public void onCreate(Bundle saved) {
        super.onCreate(saved);
        getWindow().setStatusBarColor(Color.BLACK);
        getWindow().setNavigationBarColor(Color.BLACK);
        FrameLayout root = new FrameLayout(this);
        root.setBackgroundColor(Color.BLACK);
        ImageView bg = new ImageView(this);
        bg.setImageResource(R.drawable.background);
        bg.setScaleType(ImageView.ScaleType.FIT_XY);
        root.addView(bg, new FrameLayout.LayoutParams(-1, -1));
        View clickTarget = new View(this);
        root.addView(clickTarget, new FrameLayout.LayoutParams(-1, -1));
        clickTarget.setOnTouchListener((v, ev) -> {
            if (ev.getAction() == android.view.MotionEvent.ACTION_UP) {
                float x = ev.getX() / Math.max(1, v.getWidth());
                float y = ev.getY() / Math.max(1, v.getHeight());
                if (x > .07f && x < .93f && y > .58f && y < .71f) launch();
            }
            return true;
        });
        status = new TextView(this);
        status.setText("HUNTARES CHEAT | READY");
        status.setTextColor(Color.WHITE);
        status.setTextSize(12);
        status.setGravity(Gravity.CENTER);
        GradientDrawable shape = new GradientDrawable();
        shape.setColor(0xDD131B2D);
        shape.setCornerRadius(18f);
        shape.setStroke(2, 0xFF5E92E6);
        status.setBackground(shape);
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(-1, -2, Gravity.TOP);
        lp.setMargins(18, 64, 18, 0);
        status.setPadding(8, 12, 8, 12);
        root.addView(status, lp);
        setContentView(root);
    }

    private static String quote(String s) {
        return "'" + s.replace("'", "'\"'\"'") + "'";
    }

    private void showStatus(String s) {
        runOnUiThread(() -> {
            status.setText(s);
            Toast.makeText(this, s, Toast.LENGTH_SHORT).show();
        });
    }

    private void extract(String name, File path) throws IOException {
        try (InputStream in = getAssets().open(name); OutputStream out = new FileOutputStream(path)) {
            byte[] buf = new byte[16384];
            int n;
            while ((n = in.read(buf)) != -1) out.write(buf, 0, n);
        }
    }

    private String rootCommand(String command) throws IOException, InterruptedException {
        java.lang.Process proc = new ProcessBuilder("su", "-c", command)
            .redirectErrorStream(true).start();
        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        try (InputStream in = proc.getInputStream()) {
            byte[] b = new byte[4096];
            int n;
            while ((n = in.read(b)) >= 0) {
                if (buffer.size() < 131072) buffer.write(b, 0, Math.min(n, 131072 - buffer.size()));
            }
        }
        int code = proc.waitFor();
        String text = buffer.toString("UTF-8");
        Log.i(TAG, text);
        if (code != 0) throw new IOException("su command exit " + code + ": " + text.substring(0, Math.min(text.length(), 320)));
        return text;
    }

    private void launch() {
        if (!busy.compareAndSet(false, true)) return;
        if (getPackageManager().getLaunchIntentForPackage(GAME_PACKAGE) == null) {
            busy.set(false);
            showStatus("Standoff 2 не установлена");
            return;
        }
        showStatus("HUNTARES | Подготовка библиотеки");
        new Thread(() -> {
            try {
                File injector = new File(getFilesDir(), INJECTOR_ASSET);
                File module = new File(getFilesDir(), MODULE_ASSET);
                extract(INJECTOR_ASSET, injector);
                extract(MODULE_ASSET, module);
                if (!"0".equals(rootCommand("id -u").trim()))
                    throw new IOException("Root недоступен");
                rootCommand("cp " + quote(injector.getAbsolutePath()) + " " + quote(REMOTE_INJECTOR)
                        + " && cp " + quote(module.getAbsolutePath()) + " " + quote(REMOTE_LIB)
                        + " && chmod 700 " + quote(REMOTE_INJECTOR)
                        + " && chmod 644 " + quote(REMOTE_LIB));
                showStatus("HUNTARES | Запуск инжектора");
                String cmd = quote(REMOTE_INJECTOR) + " --package " + quote(GAME_PACKAGE)
                        + " --libs " + quote(REMOTE_LIB)
                        + " --memfd --timeout 3000 "
                        + "$(pidof " + GAME_PACKAGE + " >/dev/null 2>&1 || echo --launch)";
                String log = rootCommand(cmd);
                showStatus("HUNTARES | Команда завершена; см. Logcat");
                Log.i(TAG, "Injector result: " + log);
            } catch (Exception ex) {
                Log.e(TAG, "Injection failed", ex);
                showStatus("HUNTARES | Ошибка загрузки: " + ex.getClass().getSimpleName());
            } finally {
                busy.set(false);
            }
        }, "HUNTARES-Inject").start();
    }
}
