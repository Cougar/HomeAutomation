/*
 * File created by Jimmy
 * at 2004-jun-09 18:52:31
 */
package Macbeth.System;

/**
 * This class represents a Macbeth Action, that is an action
 * that can be performed by the system.
 *
 * @author Jimmy
 */
public class MbAction {

    //the action string (OWNER.ACTIONNAME)
    private String actionString;
    //the action parameters (comma-separated)
    private String parameters;

    /**
     * Creates a new instance of MbAction.
     *
     * @param name The actionString of the action.
     * @param parameters The action parameters.
     */
    public MbAction(String name, String parameters) {
        this.actionString = name;
        this.parameters = parameters;
    }

    /**
     * Creates a new instance of MbAction.
     */
    public MbAction() {
        this(null, null);
    }

    public String getActionString() {
        return actionString;
    }

    public String getParameters() {
        return parameters;
    }

    public String toString() {
        return actionString + "(" + parameters + ")";
    }

}
