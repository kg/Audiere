package org.aegisknight.audiere;

public class Context
{
    static {
        System.loadLibrary("javaAudio");
    }

    public Context()
        throws CreateContextException {

        initialize();
    }

    private native void initialize()
        throws CreateContextException;

    protected native void finalize();

    public native Stream openStream(String filename)
        throws OpenStreamException;

    // this is our internal pointer data...
    // evil evil evil  :(
    private long internal;
}
