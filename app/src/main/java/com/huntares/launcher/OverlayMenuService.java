package com.huntares.launcher;

import android.app.Service;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.drawable.GradientDrawable;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.IBinder;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/** Native BaseMenuAIDE drawn by Android itself so in-game Unity input hooks are unnecessary.
 * Only the overlay's rectangle receives touches. Area outside the rectangle stays in the game.
 * This service contains no game-material modification code.
 */
public final class OverlayMenuService extends Service {
    private static final int OVERLAY_TYPE = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
    private static final String CHANNEL = "huntares_overlay";
    private WindowManager windowManager;
    private View bubble;
    private FrameLayout panel;

    @Override public IBinder onBind(Intent intent) { return null; }

    @Override public void onCreate() {
        super.onCreate();
        NotificationManager notificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= 26) {
            NotificationChannel channel = new NotificationChannel(CHANNEL,"HUNTARES floating menu",NotificationManager.IMPORTANCE_LOW);
            notificationManager.createNotificationChannel(channel);
        }
        Notification.Builder builder = Build.VERSION.SDK_INT >= 26
                ? new Notification.Builder(this, CHANNEL) : new Notification.Builder(this);
        Notification n = builder.setContentTitle("HUNTARES MENU")
                .setContentText("Нажмите HUNTARES, чтобы открыть меню")
                .setSmallIcon(android.R.drawable.ic_menu_manage).setOngoing(true).build();
        startForeground(512, n);
        windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
    }

    @Override public int onStartCommand(Intent intent, int flags, int startId) {
        if (!Settings.canDrawOverlays(this)) { stopSelf(); return START_NOT_STICKY; }
        if (bubble == null && panel == null) showBubble();
        return START_STICKY;
    }

    private int dp(float n) { return Math.round(n * getResources().getDisplayMetrics().density); }
    private GradientDrawable bg(int color, int radius, int line) {
        GradientDrawable d = new GradientDrawable(); d.setColor(color); d.setCornerRadius(dp(radius));
        d.setStroke(dp(1), line); return d;
    }
    private WindowManager.LayoutParams params(int w, int h, int gravity) {
        WindowManager.LayoutParams p = new WindowManager.LayoutParams(w, h, OVERLAY_TYPE,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL,
                PixelFormat.TRANSLUCENT);
        p.gravity = gravity;
        return p;
    }
    private void remove(View v) {
        if (v != null) { try { windowManager.removeView(v); } catch (Exception ignored) {} }
    }
    private void showBubble() {
        if (bubble != null) return;
        TextView v = new TextView(this);
        v.setGravity(Gravity.CENTER);
        v.setText("HUNTARES  |  MENU");
        v.setTextSize(12f);
        v.setTextColor(Color.WHITE);
        v.setBackground(bg(0xEE131927, 12, 0xFF478EFF));
        v.setOnClickListener(w -> openPanel());
        bubble = v;
        WindowManager.LayoutParams p = params(dp(160), dp(45), Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL);
        p.y = dp(14);
        windowManager.addView(v,p);
    }
    private void openPanel() {
        if (panel != null) return;
        remove(bubble); bubble = null;
        DisplayMetrics dm = getResources().getDisplayMetrics();
        int screenW = dm.widthPixels, screenH = dm.heightPixels;
        int width = Math.min(screenW - dp(12), Math.max(dp(280), Math.round(screenW * 0.62f)));
        int height = Math.min(screenH - dp(12), Math.max(dp(200), Math.round(screenH * 0.72f)));
        FrameLayout root = new FrameLayout(this);
        root.setBackgroundColor(Color.TRANSPARENT);
        MenuGLView graphics = new MenuGLView(this);
        root.addView(graphics,new FrameLayout.LayoutParams(-1,-1));
        // Guaranteed Android-handled close control independent of game input.
        TextView close = new TextView(this);
        close.setText("X");
        close.setTextColor(Color.WHITE); close.setTextSize(18f); close.setGravity(Gravity.CENTER);
        close.setBackground(bg(0xEE293349, 8, 0xFF577CB5));
        close.setOnClickListener(v -> closePanel());
        FrameLayout.LayoutParams closePos = new FrameLayout.LayoutParams(dp(39),dp(39),Gravity.RIGHT|Gravity.TOP);
        closePos.rightMargin = dp(12); closePos.topMargin = dp(8);
        root.addView(close, closePos);
        panel = root;
        windowManager.addView(root,params(width,height,Gravity.CENTER));
    }
    private void closePanel() {
        if (panel == null) return;
        FrameLayout old = panel; panel = null;
        remove(old);
        showBubble();
    }
    @Override public void onDestroy() {
        remove(panel); remove(bubble); panel=null; bubble=null;
        super.onDestroy();
    }

    final class MenuGLView extends GLSurfaceView {
        MenuGLView(Context context) {
            super(context);
            setEGLContextClientVersion(2);
            setEGLConfigChooser(8,8,8,8,0,0);
            getHolder().setFormat(PixelFormat.TRANSLUCENT);
            setZOrderOnTop(true);
            setRenderer(new Renderer() {
                @Override public void onSurfaceCreated(GL10 gl,EGLConfig config) {
                    NativeMenu.onSurfaceCreated();
                }
                @Override public void onSurfaceChanged(GL10 gl,int w,int h) {
                    NativeMenu.onSurfaceChanged(w,h);
                }
                @Override public void onDrawFrame(GL10 gl) {
                    NativeMenu.onDrawFrame();
                }
            });
            setRenderMode(RENDERMODE_CONTINUOUSLY);
        }
        @Override public boolean onTouchEvent(MotionEvent e) {
            boolean consumed = false;
            int idx;
            switch(e.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                case MotionEvent.ACTION_POINTER_DOWN:
                    idx=e.getActionIndex();
                    consumed=NativeMenu.onTouch(0,e.getPointerId(idx),e.getX(idx),e.getY(idx));
                    break;
                case MotionEvent.ACTION_MOVE:
                    for(int i=0;i<e.getPointerCount();i++)
                        consumed |= NativeMenu.onTouch(2,e.getPointerId(i),e.getX(i),e.getY(i));
                    break;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_POINTER_UP:
                    idx=e.getActionIndex();
                    consumed=NativeMenu.onTouch(1,e.getPointerId(idx),e.getX(idx),e.getY(idx));
                    // The AIDE close and handle are supported; Android window also has X.
                    if (!NativeMenu.isOpen()) post(() -> closePanel());
                    break;
                case MotionEvent.ACTION_CANCEL:
                    for(int i=0;i<e.getPointerCount();i++)
                        consumed |= NativeMenu.onTouch(3,e.getPointerId(i),e.getX(i),e.getY(i));
                    break;
            }
            // Our window is not touch-modal: touches outside it reach the game.
            // Returning true here also prevents unpredictable gesture cancellation.
            return true;
        }
    }
}
