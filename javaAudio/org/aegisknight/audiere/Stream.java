package org.aegisknight.audiere;

public class Stream
{
    static final int VOLUME_MIN = 0;
    static final int VOLUME_MAX = 255;
    static final int PAN_LEFT   = -255;
    static final int PAN_CENTER = 0;
    static final int PAN_RIGHT  = 255;


    private Stream() {
    }
    
    protected native void finalize();

    public native void play();
    public native void pause();
    public native void reset();

    public native boolean isPlaying();

    public native boolean getRepeating();
    public native void setRepeating(boolean repeating);

    public native int getVolume();
    public native void setVolume(int volume);

    public native int getPan();
    public native void setPan(int pan);


    // this is our internal pointer data...
    // evil evil evil  :(
    private long internal;
}
