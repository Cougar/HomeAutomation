/*
 * MbLocation.java
 *
 * Created on den 28 augusti 2003, 13:49
 */
package Macbeth.System;

/**
 * A Macbeth location. This consists of a kernel name and a module name.
 * @author Jimmy
 */
public class MbLocation {
    //Name of the kernel
    private String kernel;
    //Name of the module within the specified kernel
    private String module;

    /**
     * Creates a new instance of MbLocation.
     */
    public MbLocation() {
        kernel = null;
        module = null;
    }

    /**
     * Creates a new instance of MbLocation.
     * @param kernel The kernel name.
     * @param module The module name.
     */
    public MbLocation(String kernel, String module) {
        this.kernel = kernel;
        this.module = module;
    }

    /**
     * Returns the name of the kernel.
     * @return the name of the kernel.
     */
    public synchronized String getKernel() {
        return kernel;
    }
    /**
     * Sets the name of the kernel.
     * @param kernel The name of the kernel.
     */
    public synchronized void setKernel(String kernel) {
        this.kernel = kernel;
    }

    /**
     * Returns the name of the module.
     * @return the name of the module.
     */
    public synchronized String getModule() {
        return module;
    }
    /**
     * Sets the name of the module.
     * @param module the name of the module.
     */
    public synchronized void setModule(String module) {
        this.module = module;
    }

    /**
     * Prints location as a String.
     * @return A string with location info.
     */
    public String toString() {
        String str = "Kernel:" + kernel + ", Module:" + module;
        return str;
    }
}
