package org.aegisknight.audiere;

public class Context
{
    static {
        System.loadLibrary("javaAudio");
    }

    public Context(String output_device, String parameters)
        throws CreateContextException {

        initialize(output_device, parameters);
    }

    private native void initialize(String output_device, String parameters)
        throws CreateContextException;

    protected native void finalize();

    public native Stream openStream(String filename)
        throws OpenStreamException;

    // this is our internal pointer data...
    // evil evil evil  :(
    private long internal;
}
