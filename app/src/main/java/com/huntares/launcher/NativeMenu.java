package com.huntares.launcher;

final class NativeMenu {
    static { System.loadLibrary("huntaresmenu"); }
    private NativeMenu() {}
    static native void onSurfaceCreated();
    static native void onSurfaceChanged(int width, int height);
    static native void onDrawFrame();
    static native boolean onTouch(int action, int pointerId, float x, float y);
    static native boolean isOpen();
    static native void setOpen(boolean open);
}
