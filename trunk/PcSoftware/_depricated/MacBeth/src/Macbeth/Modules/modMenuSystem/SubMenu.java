/*
 * File created by arune
 */
package Macbeth.Modules.modMenuSystem;


/**
 * Keeps track of all modules that have requested as
 * a submenu module.
 * @author arune
 */
public class SubMenu {
	
	private String module;
    private String displayName;
    private String displayDesc;
    private boolean visible;


    /**
     * Creates a new instance of SubMenu.
     */
    public SubMenu(String module, String displayName, String displayDesc, boolean visible) {
    	this.module = module;
    	this.displayName = displayName;
    	this.displayDesc = displayDesc;
    	this.visible = visible;
    }


    public boolean isVisible() {
    	return visible;
    }
    
    public String getDisplayName() {
    	return displayName;
    }
    
    public String getModuleName() {
    	return module;
    }
    
    public String getDisplayDesc() {
    	return displayDesc;
    }

}
