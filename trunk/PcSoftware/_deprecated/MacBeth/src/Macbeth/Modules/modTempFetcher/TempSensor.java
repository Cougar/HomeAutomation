/*
 * Date: 2004-sep-09
 * Time: 19:39:30
*/

package Macbeth.Modules.modTempFetcher;

import Macbeth.Utilities.UByte;


/**
 * Represents a sensor on a node
 * (a node can have several sensors)
 * @author arune
 */
public class TempSensor {

    public String name;
    public UByte identByte;
    public String logPath;
    private int nrOfSamples;

    public boolean printToDisplay;

    //data-arrays, build a mean with those
    private double[] tempData;
    private int tempPointer = 0;

    public TempSensor(String name, UByte identByte, String logPath, int nrOfSamples, boolean printToDisplay) {
        this.name = name;
        this.identByte = identByte;
        this.logPath = logPath;
        this.nrOfSamples = nrOfSamples;
        this.printToDisplay = printToDisplay;
        tempData = new double[nrOfSamples];
    }

    /**
     *
     * @param tempValue the value to be stored
     * @return true if a mean can be calculated
     */
    public boolean addValue(double tempValue){

       //spara varden i array
        tempData[tempPointer] = tempValue;
        //uppdatera pekare
        tempPointer = tempPointer + 1;
        if (tempPointer == nrOfSamples) {
            tempPointer = 0;
            return true;
        }
        return false;
    }

    public double getMean() {
        double dummyVar = 0;
        for (int i = 0; i < nrOfSamples; i++) {
            dummyVar = dummyVar + tempData[i];
        }

        return dummyVar/nrOfSamples;
    }

}
