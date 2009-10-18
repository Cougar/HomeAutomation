/*
 * File created by Jimmy
 * at 2004-jan-20 16:59:27
 */
package Macbeth.Utilities;

import java.io.File;
import java.io.FileNotFoundException;
import java.net.URL;
import java.net.MalformedURLException;
import java.net.URLClassLoader;
import java.lang.reflect.Method;
import java.util.LinkedList;
import java.util.List;

/**
 * Dynamic class loader that can enumerate and load classes at runtime.
 * @author Jimmy
 */
public class DynamicClassLoader {

    /**
     * Examines a class to find outwhether or not it meets the set requirements.
     * Requirements can be set by passing a list of Superclasses and/or interfaces
     * that the class must inherit/implement.
     * @param theClass The name of the class to examine.
     * @param superClassRequirements A list of names of superclasses that the class must inherit.
     * @param interfaceRequirements A list of names of interfaces that the class must implement.
     * @return true if all the set requirements are met, false otherwise.
     */
    private static boolean examineClass(Class theClass, String[] superClassRequirements, String[] interfaceRequirements) {
        //if there are requirements on superclasses
        if (superClassRequirements!=null) {
            //an array that indicates which superclass requirements have been met
            boolean[] requirementsMet = new boolean[superClassRequirements.length];
            for (int i=0; i<requirementsMet.length; i++) {
                requirementsMet[i] = false;
            }
            //get the first superclass of our class
            Class sup = theClass.getSuperclass();
            //iterate through all superclass requirements
            for (int i=0; i<superClassRequirements.length; i++) {
                String superClassRequirement = superClassRequirements[i];
                //iterate through all superclasses of our class
                while (sup != null) {
                    //if this superclass equals the required superclass
                    if (sup.getName().toString().equals(superClassRequirement)) {
                        //then this requirement has been met
                        requirementsMet[i] = true;
                        //no need to search for this particular superclass anymore
                        break;
                    }
                    //go to next superclass (if any)
                    else {
                        sup = sup.getSuperclass();
                    }
                }
            }
            //now, find out if all superclass requirements were met
            for (int i=0; i<requirementsMet.length; i++) {
                //if any of the requirements were NOT met
                if (!requirementsMet[i]) {
                    //the whole method fails
                    return false;
                }
            }
        }
        //if there are requirements on interfaces
        if (interfaceRequirements != null) {
            //an array that indicates which interface requirements have been met
            boolean[] requirementsMet = new boolean[interfaceRequirements.length];
            for (int i = 0; i < requirementsMet.length; i++) {
                requirementsMet[i] = false;
            }
            //get all interface classes implemented by our class
            Class[] interfaces = theClass.getInterfaces();
            //iterate through all interface requirements
            for (int i = 0; i < interfaceRequirements.length; i++) {
                String interfaceRequirement = interfaceRequirements[i];
                //iterate through all the implemented interfaces
                for (int j=0; j<interfaces.length; j++) {
                    //if this interface equals the required interface
                    if (interfaces[j].getName().toString().equals(interfaceRequirement)) {
                        //then this requirement has been met
                        requirementsMet[i] = true;
                        //no need to search for this particular interface anymore
                        break;
                    }
                }
            }
            //now, find out if all interface requirements were met
            for (int i = 0; i < requirementsMet.length; i++) {
                //if any of the requirements were NOT met
                if (!requirementsMet[i]) {
                    //the whole method fails
                    return false;
                }
            }
        }
        //if this point is reached, the class met all requirements!
        return true;
    }


    /**
     * Searches a directory for classes that fulfill some specified requirements.
     * The class names of the classes that are found will be saved in a user
     * specified list. Subdirectories will be recursively searched as well, and the
     * name of each found subdir will be appended to the class names of the classes
     * that are found inside that directory. For instance, if a class is found in
     * the base directory it is assumed to be part of the given base package (both
     * of which you specify with parameters), but if the class is found insude a
     * subdir with the relative path "Macbeth/Modules" it is assumed to be part of a
     * package called [base package].Macbeth.Modules.
     * @param basedirectory The base directory to start looking in.
     * @param basepackage The corresponing package name for the given base directory.
     * @param superClassRequirements A list of superclass names that all classes
     * must inherit from (or else they will be ignored).
     * @param interfaceRequirements A list of interface names that all classes must
     * implement (or else they will be ignored).
     * @param classList A list into which all class names will be put.
     * @throws FileNotFoundException if the specified directory isn't found.
     */
    public static void locateClasses(String basedirectory, String basepackage,String[] superClassRequirements,
                              String[] interfaceRequirements, List classList)
                              throws FileNotFoundException {
        //create file listing for given directory
        File dir = new File(basedirectory);
        File[] dirListing = dir.listFiles();
        //convert directory to an URL (so it can be used with URLClassLoader)
        URL dirURL;
        URL[] urls;
        try {
            dirURL = dir.toURL();
            urls = new URL[]{dirURL};
        } catch (MalformedURLException e) {
            throw new FileNotFoundException("Invalid directory");
        }
        //create an URLClassLoader that can load classes from our directory
        ClassLoader cl = new URLClassLoader(urls);
        //the object to temporarily hold loaded classes
        Class loadedClass;
        //now, for each file in this dir...
        for (int i=0; i<dirListing.length; i++) {
            //if this is a directory
            if (dirListing[i].isDirectory()) {
                //if no base package is specified
                if (basepackage.equals("")) {
                    //continue searching recursively through the subdirs
                    locateClasses(basedirectory + System.getProperty("file.separator").toString() +
                                  dirListing[i].getName(), dirListing[i].getName(),
                                  superClassRequirements, interfaceRequirements, classList);
                }
                //if base package is specified
                else {
                    //append a dot, and then continue searching recursively through the subdirs
                    locateClasses(basedirectory + System.getProperty("file.separator").toString() +
                                  dirListing[i].getName(), basepackage + "." + dirListing[i].getName(),
                                  superClassRequirements, interfaceRequirements, classList);
                }
            }
            //this wasn't a directory
            else {
                //if this is not a file ending with .class
                if (!dirListing[i].getName().toString().endsWith(".class")) {
                    //ignore it and go on
                    continue;
                }
                //else, work out the class name (so we can try to load the class)
                String filename = dirListing[i].getName().toString();
                String classname;
                //if a base package was given, we need to append a dot as a separator
                if (basepackage.equals("")) {
                    classname = filename.substring(0, filename.length() - 6);
                } else {
                    classname = basepackage + "." + filename.substring(0, filename.length() - 6);
                }
                //try to load class (so we can examine it)
                try {
                    //try to load class
                    loadedClass = cl.loadClass(classname);
                    //and check if class meets requirements
                    if (examineClass(loadedClass,superClassRequirements,interfaceRequirements)) {
                        //if it does, save this classname in class list
                        classList.add(classname);
                    }
                } catch (ClassNotFoundException e) {
                    //something went wrong, so ignore the current class and go on
                    continue;
                }
            }
        }
    }

    /**
     * Loads a class from a directory at runtime.
     * @param baseDirectory The base directory in which the class (or package) is located.
     * @param classname The full classname of the class you want to load.
     * @return The loaded class.
     * @throws FileNotFoundException If the base directory isn't found.
     * @throws ClassNotFoundException If the class isn't found.
     */
    public static Class loadClass(String baseDirectory, String classname)
                                  throws FileNotFoundException, ClassNotFoundException {
        File dir = new File(baseDirectory);
        //convert directory to an URL (so it can be used with URLClassLoader)
        URL dirURL;
        URL[] urls;
        try {
            dirURL = dir.toURL();
            urls = new URL[]{dirURL};
        } catch (MalformedURLException e) {
            throw new FileNotFoundException("Invalid directory");
        }
        //create an URLClassLoader that can load classes from our directory
        ClassLoader cl = new URLClassLoader(urls);
        //try to load and return the class
        return cl.loadClass(classname);
    }

}
